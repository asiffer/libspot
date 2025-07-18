import { wasmBinary } from "./binary.ts";

export interface Libspot {
  spot_size: () => number;
  spot_init: (
    ptr: number,
    q: number,
    low: number,
    discardAnomalies: number,
    level: number,
    maxExcess: number
  ) => number;
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
  malloc: (size: number) => number;
  free: (ptr: number) => void;
  start: () => void;
}

type LibspotExports = Libspot & {
  memory: WebAssembly.Memory;
};

const loadWASM = async (imports = {}): Promise<LibspotExports> => {
  const { instance } = await WebAssembly.instantiate(wasmBinary.buffer, {
    ...imports,
  });
  // @ts-ignore
  instance.exports.libspot_init();

  // @ts-ignore
  return {
    // @ts-ignore
    ...instance.exports,
  } as LibspotExports;
};

export const {
  memory,
  spot_fit,
  spot_init,
  spot_free,
  spot_probability,
  spot_quantile,
  spot_size,
  spot_step,
  libspot_error,
  malloc,
  free,
  libspot_version,
} = await loadWASM();
