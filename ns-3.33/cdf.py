#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Aug 24 11:44:00 2021

@author: raviravindran
"""

import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('rlf_dl_thrput_1_eNB_URLLC6ideal_rrc',  usecols=(1,))

x= np.sort(data)

y = np.arange(1, len(data)+1)/len(data)

plt.plot(x, y, marker='.', linestyle='-')

plt.xlabel('RAN Delay')

plt.ylabel('ECDF')

plt.show()
