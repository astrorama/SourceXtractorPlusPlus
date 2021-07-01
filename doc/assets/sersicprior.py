#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# EB 24/07/2019
# Plot the Sérsic prior as defined in the documentation

import numpy as np
import matplotlib
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['text.latex.preamble'] = [r"\usepackage{amsmath}",r"\usepackage{sansmath}", r"\sansmath"]
matplotlib.rcParams['font.family'] = ['sans-serif']
matplotlib.rcParams.update({'font.size': 20})

import matplotlib.pyplot as plt


f, ax = plt.subplots(1,1, figsize=(12,9), dpi=150)
plt.subplots_adjust(left=0.08, right=0.99, bottom=0.11, top=0.98, wspace=0.3, hspace=0.4)
xmin = 0.3
xmax = 8.0
npoints = 400
x = np.linspace(xmin, xmax, npoints)
s0 = 4
s2 = np.exp(x - s0)
y = np.exp(-s2*s2 / 2.0)
norm = sum(y) * (xmax - xmin) / npoints
y /= norm
ax.set_xlim(0, 8.3)
ax.plot(x,y,linewidth=3)
ax.set_xlabel("Sérsic index", fontsize=24)
ax.set_ylabel("prior", fontsize=24)
ax.grid(linewidth=0.5, linestyle=':')
plt.savefig("sersicprior.svg")
plt.savefig("sersicprior.pdf")

