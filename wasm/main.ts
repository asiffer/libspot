import { Spot, libspotError } from "./libspot.ts";

// const libspot = await loadWASM();

console.log(
  "libspot",
  new Spot({
    q: 1e-4,
    low: 0,
    discardAnomalies: 1,
    level: 0.98,
    maxExcess: 500,
  })
);

// libspot.libspot_init();

// const allocator = createAllocator(libspot.memory, 1024);
// libspot.set_allocators(allocator.malloc, allocator.free);

// const spot = libspot.spot_new();
// libspot.spot_init(spot, 1e-6, 0.01, 0, 0.99, 1000);
// console.log(
//   "Spot created at",
//   spot,
//   libspot.memory.buffer.slice(spot, spot + libspot.spot_size())
// );

/**
 *
 * @param code error code
 * @returns the message related to the error code
 */
// export const libspotError = (code: number): string => {
//   const size = 128;
//   const ptr = allocator.malloc(size);
//   //   allocator.heap8.fill(0, ptr, ptr + size); // fill with zeros
//   console.log("PTR", ptr);
//   //   const memory = new Uint8Array(libspot.memory.buffer);
//   //   memory.fill(0); // fill with zeros
//   //   console.log(memory);
//   libspot.libspot_error(code, ptr, size);
//   console.log("ARRAY", allocator.heap8.slice(ptr, ptr + size));
//   const msg = stringify(allocator.heap8.slice(ptr, ptr + size)); // fill with zeros
//   allocator.free(ptr);
//   return msg;
// };

// console.log(libspot.memory);
console.log("libspotError(1000)", libspotError(1000));
