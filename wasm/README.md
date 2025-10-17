# libspot.js

`libspot` in the browser.

This sub-project ports `libspot` into javascript through webassembly (using `llvm`).

## Getting Started

```shell
npm install libspot
```

Here is a simple example.

```ts
import { Spot, ANOMALY, EXCESS, NORMAL } from "libspot";

const gaussianRandom = () => {
  // N(0, 1)
  const u = 1 - Math.random(); // Converting [0,1) to (0,1]
  const v = Math.random();
  return Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v);
};

const trainSize = 20000;
const testSize = 500000;

// Spot job ------------------------------------------------------------------
const s = new Spot({ q: 1e-4, level: 0.99, maxExcess: 200 });

// fit to input data
const train = Float64Array.from({ length: trainSize }, () => gaussianRandom());
s.fit(train);

// run
let anomaly = 0;
let excess = 0;
let normal = 0;

// measured in milliseconds
const start = performance.now();

for (let i = 0; i < testSize; i++) {
  switch (s.step(gaussianRandom())) {
    case ANOMALY:
      anomaly++;
      break;
    case EXCESS:
      excess++;
      break;
    case NORMAL:
      normal++;
      break;
  }
}

const end = performance.now();

console.log(`Time: ${(end - start).toFixed(2)} ms`);
console.log(`Throughput: ${(1000*testSize/(end - start)).toFixed(2)} value/s`);
console.log(`ANOMALY: ${anomaly}, EXCESS: ${excess}, NORMAL: ${normal}`);
```