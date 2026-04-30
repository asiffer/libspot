import { wasmBinary } from "./binary.ts";

export interface Libspot {
  spot_size: () => number;
  spot_init: (
    ptr: number,
    q: number,
    low: number,
    discardAnomalies: number,
    level: number,
    buffer: number,
    maxExcess: number,
  ) => number;
  spot_reset(ptr: number): void;
  spot_fit: (ptr: number, arrayPtr: number, size: number) => number;
  spot_step: (ptr: number, x: number) => number;
  spot_quantile: (ptr: number, q: number) => number;
  spot_probability: (ptr: number, z: number) => number;
  spot_free: (ptr: number) => void;
  libspot_error: (code: number, ptr: number, size: number) => string;
  libspot_version: (ptr: number, size: number) => void;
  // set_allocators: (
  //   malloc: (size: number) => number,
  //   free: (ptr: number) => void
  // ) => void;
  // malloc: (size: number) => number;
  // free: (ptr: number) => void;
  // start: () => void;
}

type LibspotExports = Libspot & {
  memory: WebAssembly.Memory;
  __heap_base: WebAssembly.Global;
};

const loadWASM = async (imports = {}): Promise<LibspotExports> => {
  const { instance } = await WebAssembly.instantiate(wasmBinary.buffer, {
    ...imports,
  });
  // @ts-ignore
  // instance.exports.libspot_init();

  // @ts-ignore
  return {
    // @ts-ignore
    ...instance.exports,
  } as LibspotExports;
};

const _wasm = await loadWASM();

export const {
  memory,
  spot_fit,
  spot_init,
  spot_reset,
  spot_probability,
  spot_quantile,
  spot_size,
  spot_step,
  libspot_error,
  libspot_version,
} = _wasm;

export const heapBase: number = _wasm.__heap_base.value as number;
