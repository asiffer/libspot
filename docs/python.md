# Python

Yes, `python3` bindings exist.
Thanks to the powerful built-in package `ctypes` we make the use of `libspot` through python possible.
Tou can download these bindings through `pip`:

```shell
pip3 install pylibspot
```

!!! warning
	You need to install the [C++ library](../cpp/) first to use these python interface

## Introduction

The `Spot` constructor is very straightforward. We reuse the full constructor of the `C++` library but without the positional argument constraint:
```python
Spot(q=1e-4, n_init=2000, level=0.99,
	 up=True, down=True, alert=True, 
	 bounded=True, max_excess=200)
```

!!! warning
	Be careful, the default values of the python constructor may not be the same as in the native library.

## Quick example

Basically, you need to init an instance of the algorithm and then feed some data with the `.step()` method.
```python
import pylibspot as ps
import numpy as np

N = 200000
X = np.random.normal(0, 1, N)

s = ps.Spot(q=1e-4, up=True, down=False, n_init=2000)

for x in X:
    if s.step(x) == 1: # check the return code
        print(f"Anomaly found: {x} > {s.get_upper_threshold()}")
```


## Another example

Here we give an application of the `SPOT` algorithm. The script and the data are available on the github repository [python3-libspot](https://github.com/asiffer/python3-libspot) (in the 'example' folder). 

In this example we monitor the ratio of SYN packets in a network traffic capture. The original `.pcap` files are provided by the [MAWI](http://www.fukuda-lab.org/mawilab/v1.1/index.html) platform. Here we use aggregated data (measures over time slots) from two consecutive capture days (August 17 and 18, 2012).

```python
import pyspot as ps # name of the python3 package
import pandas as pd
import matplotlib.pyplot as plt

# loading data
MAWI_17 = pd.read_csv("mawi_170812_50_50.csv", index_col=[0])
MAWI_18 = pd.read_csv("mawi_180812_50_50.csv", index_col=[0])

# we keep only the ratio of SYN packets
rSYN_17 = MAWI_17['rSYN']
rSYN_18 = MAWI_18['rSYN']

# initialization of the Spot object
q = 5e-4
n_init = 2000
spot = ps.Spot(q, n_init, up=True, down=False)


## CALIBRATION
for r in rSYN_17[-n_init:]:
	spot.step(r)


## ANOMALY DETECTION

# we init some structure to log what flags the algorithm
up_threshold = [0.0] * len(rSYN_18)
anomalies_x = []
anomalies_y = []
i = 0
for r in rSYN_18:
	event = spot.step(r)
	
	if event == 1: # anomaly case
		anomalies_x.append(i)
		anomalies_y.append(r)
		
	up_threshold[i] = spot.get_upper_threshold()
	i = i+1

# Plotting stuff 
plt.plot(rSYN_18, lw=2, color="#1B4B5A")
f1, = plt.plot(up_threshold, ls='dashed', color="#AE81FF", lw=2)
f2 = plt.scatter(anomalies_x, anomalies_y, color="#F55449")
plt.legend([f1, f2], ['Threshold', 'Anomalies'])


plt.xticks(range(0,18001,3000), range(0,901,150)) # we change the ticks to make it corresponds to the true time : 1 it = 50 ms
plt.xlabel('Time (s)')
plt.ylabel('Ratio of SYN packets')
```

The output is given below. In this case, we can notice that the algorithm builds a relevant threshold able to flag abnormal peaks.

![Matplotlib](/libspot/assets/example.svg)




