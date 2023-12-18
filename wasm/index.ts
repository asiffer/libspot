import loadWASM from "./libspot.core.js";

const libspot = await loadWASM();

/**
 * The size in bytes of the underlying Spot structure (C -> compiled to WASM)
 */
export const spotSize: { (): number } = libspot.cwrap(
  "spot_size",
  "number",
  null
);

const spotNew: { (): number } = libspot.cwrap("spot_new", "number", null);

const spotFree: {
  (ptr: number): void;
} = libspot.cwrap("spot_free", null, ["number"]);

const spotInit: {
  (
    ptr: number,
    q: number,
    low: number,
    discardAnomalies: number,
    level: number,
    maxExcess: number
  ): number;
} = libspot.cwrap("spot_init", "number", [
  "number",
  "number",
  "number",
  "number",
  "number",
  "number",
]);

const spotFit: {
  (ptr: number, arrayPtr: number, size: number): number;
} = libspot.cwrap("spot_fit", "number", ["number", "number", "number"]);

const spotStep: {
  (ptr: number, x: number): number;
} = libspot.cwrap("spot_step", "number", ["number", "number"]);

const spotQuantile: {
  (ptr: number, q: number): number;
} = libspot.cwrap("spot_quantile", "number", ["number", "number"]);

const spotProbability: {
  (ptr: number, z: number): number;
} = libspot.cwrap("spot_probability", "number", ["number", "number"]);

const stringify = (raw: Uint8Array) => {
  return new TextDecoder("utf-8").decode(raw).replace(/\0/g, "");
};

/**
 *
 * @returns the version of the library
 */
export const libspotVersion = (): string => {
  const size = 24;
  let ptr = libspot._malloc(size);
  libspot.ccall("libspot_version", null, ["number", "number"], [ptr, size]);
  const version = stringify(libspot.HEAPU8.slice(ptr, ptr + size).buffer);
  libspot._free(ptr);
  return version;
};

/**
 *
 * @param code error code
 * @returns the message related to the error code
 */
export const libspotError = (code: number): string => {
  const size = 256;
  let ptr = libspot._malloc(size);
  // fill with zeros
  libspot.HEAPU8.fill(0, ptr, ptr + size);
  libspot.ccall(
    "libspot_error",
    null,
    ["number", "number", "number"],
    [code, ptr, size]
  );
  const msg = stringify(libspot.HEAPU8.slice(ptr, ptr + size));
  libspot._free(ptr);
  return msg;
};

/**
 * General interface to config a Spot object
 */
export interface SpotConfig {
  /**
   * Decision probability (Spot will flag extreme events that will have
   * a probability lower than q)
   */
  q: number;
  /**
   * Lower tail mode (0 by defaut for upper tail and 1 for lower tail)
   */
  low?: number;
  /**
   * Do not include anomalies in the model (default: 1, 0 otherwise)
   */
  discardAnomalies?: number;
  /**
   * Excess level (it is a high quantile that delimits the tail)
   */
  level?: number;
  /**
   * Maximum number of data that are kept to analyze the tail
   */
  maxExcess?: number;
}

const freg = new FinalizationRegistry((ptr: number) => {
  // this function is called when the registered object is garbage collected
  spotFree(ptr); // release internal memory (created by spotInit)
  libspot._free(ptr); // release heap allocated object (created by spotNew)
});

/**
 * Main structure to run the SPOT algorithm
 */
export class Spot {
  ptr: number;

  // Offset table
  /*
    0                   4                   8                  12                  16
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
  0 |                   q                   |                 level                 |
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 16 | discard_anomalies |        low        |               __up_down               |
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 32 |           anomaly_threshold           |            excess_threshold           |
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 48 |         Nt        |         n         |               tail.gamma              |
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 64 |               tail.sigma              |              tail.peaks.e             |
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 80 |             tail.peaks.e2             |             tail.peaks.min            |
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 96 |             tail.peaks.max            |tail.peaks.contain.|tail.peaks.contain.|
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
112 | tail.peaks.container.last_erased_data |tail.peaks.contain.|0000000000000000000|
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
128 |       tail.peaks.container.data       |

*/

  constructor({
    q,
    low = 0,
    discardAnomalies = 1,
    level = 0.98,
    maxExcess = 500,
  }: SpotConfig) {
    this.ptr = spotNew();
    freg.register(this, this.ptr); // kind of destructor
    const code = spotInit(this.ptr, q, low, discardAnomalies, level, maxExcess);
    if (code < 0) {
      throw new Error(libspotError(-code));
    }
  }

  anomaly_threshold() {
    const buffer = libspot.HEAPU8.slice(this.ptr + 32, this.ptr + 40).reverse()
      .buffer;
    const view = new DataView(buffer);
    // console.log(this.ptr, libspot.HEAPU8[this.ptr + 4]);
    return view.getFloat64(0);
  }

  excess_threshold() {
    const buffer = libspot.HEAPU8.slice(this.ptr + 40, this.ptr + 48).reverse()
      .buffer;
    const view = new DataView(buffer);
    // console.log(this.ptr, libspot.HEAPU8[this.ptr + 4]);
    return view.getFloat64(0);
  }

  /**
   *
   * @param data Input data
   * @returns 0 if everything is ok
   * @throws {Error} When the fit has failed. It generally happens when either the anomaly or excess threshold is NaN.
   */
  fit(data: Float64Array) {
    // reserve heap
    const arrayPtr = libspot._malloc(data.length * data.BYTES_PER_ELEMENT);
    // copy array (converted to bytes) to the heap
    libspot.HEAPU8.set(new Uint8Array(data.buffer), arrayPtr);
    // call the function
    const code = spotFit(this.ptr, arrayPtr, data.length);
    if (code < 0) {
      throw new Error(libspotError(-code));
    }
    // release heap
    libspot._free(arrayPtr);
    return code;
  }

  /**
   * Fit-predict step
   *
   * @param x new incoming data
   * @returns {number} 0: NORMAL, 1: EXCESS, 2: ANOMALY
   */
  step(x: number) {
    const code = spotStep(this.ptr, x);
    if (code < 0) {
      throw new Error(libspotError(-code));
    }
    return code;
  }

  /**
   * Compute the value zq such that P(X>zq) = q
   *
   * @param q Low probability (it must be within the tail)
   * @returns the value z such that P(X>z) = q
   */
  quantile(q: number) {
    return spotQuantile(this.ptr, q);
  }

  /**
   * Compute the probability p such that P(X>z) = p
   *
   * @param z High quantile (it must be within the tail)
   * @returns the probability p such that p = P(X>z)
   */
  probability(z: number) {
    return spotProbability(this.ptr, z);
  }
}

export default Spot;
