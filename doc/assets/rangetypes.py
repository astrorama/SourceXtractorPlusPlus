#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# EB 04/05/2019
# Plot range types

import numpy as np
import matplotlib
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['text.latex.preamble'] = [r"\usepackage{amsmath}",r"\usepackage{sansmath}", r"\sansmath"]
matplotlib.rcParams['font.family'] = ['sans-serif']
matplotlib.rcParams.update({'font.size': 14})

import matplotlib.pyplot as plt

f, ax = plt.subplots(1,2, figsize=(12,5), dpi=150)
plt.subplots_adjust(left=0.08, right=0.99, bottom=0.11, top=0.98, wspace=0.3, hspace=0.4)
Qmin = -10.0
Qmax = 10.0
Q = np.linspace(Qmin, Qmax, 100)
qmin = -1.0
qmax = 1.0
q = (qmax - qmin) / (1 + np.exp(-Q)) + qmin
#ax[0].set_xlim(qmin, qmax)
ax[0].plot(q,Q)
ax[0].set_xlabel(r"$q_{x}(\textsf{model})$", fontsize=16)
ax[0].set_ylabel(r"$Q_{x}(\textsf{engine})$", fontsize=16)
ax[0].grid(linewidth=0.5, linestyle=':')

qmin = 0.01
qmax = 100.0
q = qmin * np.exp((np.log(qmax) - np.log(qmin)) / (1 + np.exp(-Q)))
ax[1].set_xscale("log")
ax[1].xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda f, _: '{:.16g}'.format(f)))
#ax[1].set_xlim(qmin, qmax)
ax[1].plot(q,Q)
ax[1].set_xlabel(r"$q_\textsf{aspect}(\textsf{model})$", fontsize=16)
ax[1].set_ylabel(r"$Q_\textsf{aspect}(\textsf{engine})$", fontsize=16)
ax[1].grid(linewidth=0.5, linestyle=':')
plt.savefig("rangetypes.svg")
plt.savefig("rangetypes.pdf")

