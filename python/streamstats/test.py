#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Sep  1 18:16:49 2017

@author: asr
"""

import libstream as ls
import numpy as np

ss = ls.StreamStats(1000)
X = np.random.normal(0,1,10000)

for x in X:
    ss.step(x)

