# libspot.js

`libspot` in the browser.

This sub-project ports `libspot` into javascript through webassembly (using `llvm`).

## Getting Started

```shell
npm install libspot
```

Here is a minimal example.

```ts
import { Spot, ANOMALY } from "libspot";

const gaussianRandom = () => {
  // N(0, 1)
  const u = 1 - Math.random(); 
  const v = Math.random();
  return Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v);
};


// Spot job ------------------------------------------------------------------
const trainSize = 10000;
const level = 0.98;
const maxExcess = (1-level) * trainSize;

const spot = new Spot({ q: 1e-4, level: level, maxExcess: maxExcess });
console.log("Spot initialized");

// fit to input data
const train = Float64Array.from({ length: trainSize }, () => gaussianRandom());
spot.fit(train);
console.log("Spot fitted");

// run
for (let i = 0; i < 100000; i++) {
  const x = gaussianRandom();
  let r = spot.step(x);

  if (r === ANOMALY) {
    console.warn(`ANOMALY DETECTED! value=${x.toFixed(3)} probability=${spot.probability(x).toExponential(3)}`)
  }
}
```