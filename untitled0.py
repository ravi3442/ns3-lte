#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Aug 24 11:44:00 2021

@author: raviravindran
"""

import numpy as np
import matplotlib.pyplot as plt

x = np.loadtxt('rlf_dl_thrput_1_eNB_EMBB5ideal_rrc')

sorted_data = np.sort(data)
cumulative = np.cumsum(sorted_data)

plt.plot(cumulative)
plt.show()

