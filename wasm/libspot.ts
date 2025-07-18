import {
  libspot_error,
  malloc as _malloc,
  free,
  spot_fit,
  spot_free,
  spot_init,
  spot_probability,
  spot_quantile,
  spot_size,
  spot_step,
  libspot_version,
  memory,
} from "./libspot.core.ts";

export const NORMAL = 0;
export const EXCESS = 1;
export const ANOMALY = 2;

let heap8 = new Uint8Array(memory.buffer);

/**
 * Wrapper around malloc to check if the requested memory fits the webassembly memory
 * @param size Size in bytes to allocate
 * @returns
 */
const malloc = (size: number): number => {
  const ptr = _malloc(size);
  if (ptr <= 0) {
    return ptr;
  }
  if (ptr + size > memory.buffer.byteLength) {
    // grow memory if needed
    const pages = Math.ceil(size / 65536); // 64 KiB per page
    memory.grow(pages);
    heap8 = new Uint8Array(memory.buffer); // reinitialize the view
  }
  return ptr;
};

const stringify = (raw: ArrayBuffer) => {
  return new TextDecoder("utf-8").decode(raw).replace(/\0/g, "");
};

export const libspotError = (code: number): string => {
  const size = 256;
  const ptr = malloc(size);
  heap8.fill(0, ptr, ptr + size); // fill with zeros
  libspot_error(code, ptr, size);
  const msg = stringify(heap8.slice(ptr, ptr + size).buffer as ArrayBuffer);
  free(ptr);
  return msg;
};

/**
 *
 * @returns the version of the library
 */
export const libspotVersion = (): string => {
  const size = 24;
  let ptr = malloc(size);
  libspot_version(ptr, size);
  const version = stringify(heap8.slice(ptr, ptr + size).buffer as ArrayBuffer);
  free(ptr);
  return version;
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
  spot_free(ptr); // release heap allocated object (created by spot_init)
  free(ptr); // release internal memory (created by malloc)
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
    q = 5e-4,
    low = 0,
    discardAnomalies = 1,
    level = 0.98,
    maxExcess = 500,
  }: SpotConfig) {
    this.ptr = malloc(spot_size());
    freg.register(this, this.ptr); // kind of destructor
    const code = spot_init(
      this.ptr,
      q,
      low,
      discardAnomalies,
      level,
      maxExcess
    );
    if (code < 0) {
      throw new Error(libspotError(-code));
    }
  }

  anomaly_threshold() {
    const buffer = heap8.slice(this.ptr + 32, this.ptr + 40).reverse().buffer;
    const view = new DataView(buffer);
    return view.getFloat64(0);
  }

  excess_threshold() {
    const buffer = heap8.slice(this.ptr + 40, this.ptr + 48).reverse().buffer;
    const view = new DataView(buffer);
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
    const arrayPtr = malloc(data.length * data.BYTES_PER_ELEMENT);
    // copy array (converted to bytes) to the heap
    heap8.set(new Uint8Array(data.buffer), arrayPtr);

    // call the function
    const code = spot_fit(this.ptr, arrayPtr, data.length);
    if (code < 0) {
      throw new Error(libspotError(-code));
    }
    // release heap
    free(arrayPtr);
    return code;
  }

  /**
   * Fit-predict step
   *
   * @param x new incoming data
   * @returns {number} 0: NORMAL, 1: EXCESS, 2: ANOMALY
   */
  step(x: number) {
    const code = spot_step(this.ptr, x);
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
    return spot_quantile(this.ptr, q);
  }

  /**
   * Compute the probability p such that P(X>z) = p
   *
   * @param z High quantile (it must be within the tail)
   * @returns the probability p such that p = P(X>z)
   */
  probability(z: number) {
    return spot_probability(this.ptr, z);
  }
}

// console.log("HEAP8", heap8);

export default Spot;
export { spot_size } from "./libspot.core.ts";
