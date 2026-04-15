import { expect, test } from "bun:test";
import {
  Spot,
  libspotVersion,
  spot_size,
  libspotError,
  EXCESS,
  ANOMALY,
} from "./libspot.ts";
import * as fs from "fs";

const stdNormal = () => {
  const u = Math.random();
  const v = Math.random();
  return Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v);
};

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
  expect(spot_size()).toBe(128);
});

test("Spot(1e-6)", () => {
  expect(new Spot({ q: 1e-6 })).toBeDefined();
});

test("libspotVersion()", () => {
  const s = fs.readFileSync("../Makefile", "utf-8");
  const arr = s.match(/VERSION.*=(.*)/);
  expect(arr).toBeDefined();
  if (arr && arr.length > 1) {
    // @ts-ignore
    expect(libspotVersion()).toBe(arr[1].trim());
  }
});

test("Spot.fit", () => {
  const level = 0.99;
  const q = 1e-6;
  const s = new Spot({ q: q, level: level, maxExcess: 1000 });
  const train = Float64Array.from({ length: 30000 }, () => Math.random());
  expect(s.fit(train)).toBe(0);
  expect(s.anomaly_threshold()).toBeCloseTo(1.0, 0.05);
  expect(s.excess_threshold()).toBeCloseTo(level, 0.005);
});

test("Spot.step", () => {
  const level = 0.99;
  const q = 1e-6;
  const s = new Spot({ q: q, level: level, maxExcess: 200 });
  const train = Float64Array.from({ length: 30000 }, stdNormal);
  expect(s.fit(train)).toBe(0);

  for (let i = 0; i < 100000; i++) {
    const x = stdNormal();
    let r = s.step(x);
    if (r === EXCESS) {
      // this test may fail since the interpretation of
      // can change along time
      expect(1 - level - s.probability(x)).toBeGreaterThanOrEqual(-0.001);
    } else if (r === ANOMALY) {
      const p = s.probability(x);
      if (Number.isNaN(p)) {
        // it returns NaN when the value is above the maximum quantile,
        // which is the quantile of the smallest possible probability (MIN_VALUE)
        const bound = s.quantile(Number.MIN_VALUE);
        expect(x).toBeGreaterThan(bound);
      } else {
        expect(p).toBeLessThanOrEqual(q);
      }
    }
  }
});

const range = (start: number, stop: number, step: number = 1) =>
  Array.from({ length: (stop - start) / step + 1 }, (_, i) => start + i * step);

test("libspotError()", () => {
  const errors = range(1000, 1010)
    .map(libspotError)
    .map((msg, index) => {
      if (index > 5) {
        expect(msg).toBe("");
      } else {
        expect(msg).not.toBe("");
      }
    });
});
