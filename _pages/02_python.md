---
layout: default
title: Python
permalink: python/
---

Yes, `python3` bindings exist.
Thanks to the powerful built-in package `ctypes` we make the use of `libspot` through python possible.

#### INTRODUCTION

The `Spot` constructor is very straightforward. We reuse the full constructor of the `C++` library but without the positional argument constraint:
```python
Spot(	q=1e-4, n_init=2000, level=0.99,
	up=True, down=True, alert=True, 
	bounded=True, max_excess=200)
```

<p></p>
<div class="alert">
Be careful, the default values of the python constructor may not be the same as in the native library.
</div>


#### QUICK EXAMPLE

Here we give an application of the `SPOT` algorithm. The script and the data are available on the github repository <a href="https://github.com/asiffer/python3-libspot">python3-libspot</a> (in the 'example' folder). 

In this example we monitor the ratio of SYN packets in a network traffic capture. The original `.pcap` files are provided by the <a href="http://www.fukuda-lab.org/mawilab/v1.1/index.html">MAWI</a> platform. Here we use aggregated data (measures over time slots) from two consecutive capture days (August 17 and 18, 2012).

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




