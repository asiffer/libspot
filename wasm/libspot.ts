import {
  libspot_error,
  spot_fit,
  spot_reset,
  spot_init,
  spot_probability,
  spot_quantile,
  spot_size,
  spot_step,
  libspot_version,
  memory,
  heapBase,
} from "./libspot.core.ts";

export const NORMAL = 0;
export const EXCESS = 1;
export const ANOMALY = 2;

let offset = heapBase;

const malloc = (size: number): number => {
  // Align to 8 bytes for safe access to any typed array
  const aligned = (offset + 7) & ~7;
  const ptr = aligned;
  const needed = aligned + size;

  // Grow memory if we'd exceed current size
  const currentBytes = memory.buffer.byteLength;
  if (needed > currentBytes) {
    const pagesToGrow = Math.ceil((needed - currentBytes) / 65536);
    memory.grow(pagesToGrow);
  }

  offset = needed;
  return ptr;
};

const free = (ptr: number, size: number): void => {
  if (ptr + size === offset) {
    offset -= size;
  }
};

const stringify = (raw: ArrayBufferView) => {
  return new TextDecoder("utf-8").decode(raw).replace(/\0/g, "");
};

export const libspotError = (code: number): string => {
  const size = 256;
  const ptr = malloc(size);
  const msgBuffer = new Uint8Array(memory.buffer, ptr, size);
  msgBuffer.fill(0);

  libspot_error(code, ptr, size);
  const msg = stringify(msgBuffer);
  free(ptr, size);
  return msg;
};

/**
 *
 * @returns the version of the library
 */
export const libspotVersion = (): string => {
  const size = 24;
  let ptr = malloc(size);
  const versionBuffer = new Uint8Array(memory.buffer, ptr, size);
  versionBuffer.fill(0);
  libspot_version(ptr, size);
  const version = stringify(versionBuffer);
  free(ptr, size);
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
    const bufferPtr = malloc(maxExcess * 8); // 8 bytes per double
    const code = spot_init(
      this.ptr,
      q,
      low,
      discardAnomalies,
      level,
      bufferPtr,
      maxExcess,
    );
    if (code < 0) {
      throw new Error(libspotError(-code));
    }
  }

  /**
   * Reset the model to its initial state (keep the same config)
   */
  reset() {
    spot_reset(this.ptr);
  }

  /**
   * @returns the anomaly threshold
   */
  anomaly_threshold() {
    const view = new DataView(memory.buffer, this.ptr + 32, 8);
    return view.getFloat64(0, true);
  }

  /**
   * @returns the excess threshold (tail delimiter)
   */
  excess_threshold() {
    const view = new DataView(memory.buffer, this.ptr + 40, 8);
    return view.getFloat64(0, true);
  }

  /**
   * Fit the model to the input data
   * @param data Input data
   * @returns 0 if everything is ok
   * @throws {Error} When the fit has failed. It generally happens when either the anomaly or excess threshold is NaN.
   */
  fit(data: Float64Array) {
    const size = data.length * data.BYTES_PER_ELEMENT;
    // reserve heap
    const arrayPtr = malloc(size);
    // copy array (converted to bytes) to the heap
    const view = new Float64Array(memory.buffer, arrayPtr, data.length);
    view.set(data);

    // call the function
    const code = spot_fit(this.ptr, arrayPtr, data.length);
    if (code < 0) {
      throw new Error(libspotError(-code));
    }
    // release heap
    free(arrayPtr, size);
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

export default Spot;
export { spot_size } from "./libspot.core.ts";
