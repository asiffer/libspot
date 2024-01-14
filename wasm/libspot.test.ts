import { expect, test } from "bun:test";
import {
  Spot,
  libspotVersion,
  spotSize,
  libspotError,
  EXCESS,
} from "./index.ts";
import * as fs from "fs";

test("sizeof(Spot)", () => {
  // WASM (size in bytes)
  //           int: 4
  //        size_t: 4
  // unsigned long: 4
  //        double: 8
  //       double*: 4
  // ----------------------------------------------------------------------
  // So      Ubend: 4 + 4 + 8 + 4 + 4 = 24
  //         Peaks: 8 + 8 + 8 + 8 + 24 = 56
  //          Tail: 8 + 8 + 56 = 72
  //          Spot: 8 + 8 + 4 + 4 + 8 + 8 + 8 + 4 + 4 + 72 = 56 + 72 = 128
  expect(spotSize()).toBe(128);
});

test("Spot(1e-6)", () => {
  expect(new Spot({ q: 1e-6 })).toBeDefined();
});

test("libspotVersion()", () => {
  const s = fs.readFileSync("../Makefile", "utf-8");
  const arr = s.match(/VERSION.*=(.*)/);
  expect(arr).toBeDefined();
  if (arr) {
    expect(libspotVersion()).toBe(arr[1].trim());
  }
});

test("Spot.fit", () => {
  const level = 0.99;
  const s = new Spot({ q: 1e-6, level: level, maxExcess: 1000 });
  const train = Float64Array.from({ length: 20000 }, () => Math.random());
  expect(s.fit(train)).toBe(0);
  console.log(s.anomaly_threshold().toExponential());
  for (let i = 0; i < 1000; i++) {
    const x = Math.random();
    let r = s.step(x);
    if (r === EXCESS) {
      // this test may fail since the interpretation of
      // can change along time
      expect(s.probability(x)).toBeLessThanOrEqual(1 - level);
    }
  }
});

const range = (start: number, stop: number, step: number = 1) =>
  Array.from({ length: (stop - start) / step + 1 }, (_, i) => start + i * step);

test("libspotError()", () => {
  const errors = range(1000, 1010)
    .map(libspotError)
    .map((msg, index) => {
      if (index > 4) {
        expect(msg).toBe("");
      } else {
        expect(msg).not.toBe("");
      }
    });
});
