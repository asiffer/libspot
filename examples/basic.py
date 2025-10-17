#!/usr/bin/env -S uv run --script
#
# /// script
# dependencies = ["libspot"]
# ///

import math, random
from time import perf_counter

from libspot import Spot, ANOMALY, EXCESS, NORMAL


def gaussian_random() -> float:
    # N(0, 1)
    u: float = 1.0 - random.random()  # map [0,1) -> (0,1]
    v: float = random.random()
    return math.sqrt(-2.0 * math.log(u)) * math.cos(2.0 * math.pi * v)


TRAIN_SIZE = 20_000
TEST_SIZE = 500_000

spot = Spot(q=1e-4, level=0.99, max_excess=200)

train = [gaussian_random() for _ in range(TRAIN_SIZE)]
spot.fit(train)

# run
test = [gaussian_random() for _ in range(TEST_SIZE)]
anomaly: int = 0
excess: int = 0
normal: int = 0

# measured in seconds
start: float = perf_counter()

for x in test:
    r = spot.step(x)
    if r == ANOMALY:
        anomaly += 1
    elif r == EXCESS:
        excess += 1
    elif r == NORMAL:
        normal += 1

end: float = perf_counter()

print(f"Time: {1000.0*(end - start):.2f} ms")
print(f"Throughput: {(TEST_SIZE/(end - start)):.2f} value/s")
print(f"ANOMALY: {anomaly}, EXCESS: {excess}, NORMAL: {normal}")
