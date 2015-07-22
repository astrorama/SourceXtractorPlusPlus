#!/usr/bin/env python

import sys
# Add in the pythonpath the dirctory with the modules
sys.path.append('..')

import modelfitting.parameters as p
import modelfitting.minimization as m
import modelfitting.models as models

from astropy.table import Table
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import time

def showResults(title):
    print ''
    print 'Time of fitting:', (t2-t1)
    print ''
    print 'From modeling:'
    print '    B-V =', scaleB.getValue() - scaleV.getValue()
    print '    ScaleB =', scaleB.getValue()
    print '    ScaleV =', scaleV.getValue()
    print '    Mean =', mean.getValue()
    print '    Sigma =', sigma.getValue()
    print ''

    x = table['x']
    fig = plt.figure(figsize=(15,5))
    fig.canvas.set_window_title(title)
    xmin = min(table['x'])
    xmax = max(table['x'])
    ymin = min([min(table[n]) for n in noise_B_cols+noise_V_cols])
    ymax = max([max(table[n]) for n in noise_B_cols+noise_V_cols])
    axB = plt.subplot(121)
    axB.set_title('B filter')
    axB.set_xlim(xmin, xmax)
    axB.set_ylim(ymin, ymax)
    axB.plot([xmin,xmax], [0,0], c='k')
    axB.plot([0,0], [ymin,ymax], c='k')
    for name in noise_B_cols:
        plt.plot(x, table[name], 'o', c='g')
    axB.plot(table['x'], table['cleanB'], c='k', linewidth=3)
    axB.plot(x, [gausB(xi) for xi in x], c='r', linewidth=2)
    axV = plt.subplot(122)
    axV.set_title('V filter')
    axV.set_xlim(xmin, xmax)
    axV.set_ylim(ymin, ymax)
    axV.plot([xmin,xmax], [0,0], c='k')
    axV.plot([0,0], [ymin,ymax], c='k')
    for name in noise_V_cols:
        plt.plot(x, table[name], 'o', c='g')
    axV.plot(table['x'], table['cleanV'], c='k', linewidth=3)
    axV.plot(x, [gausV(xi) for xi in x], c='r', linewidth=2)

    plt.show()

# Read the data from the file
table = Table.read('data/two_filters.fits', format='fits')

# Print the header information
print ''
print 'Headers in file:'
for k,v in table.meta.items():
    print '   ',k,'=',v
    
# Get the names of the columns for each filter
noise_B_cols = [n for n in table.colnames if 'noiseB' in n]
noise_V_cols = [n for n in table.colnames if 'noiseV' in n]

# Plot the frames
fig = plt.figure(figsize=(15,5))
xmin = min(table['x'])
xmax = max(table['x'])
ymin = min([min(table[n]) for n in noise_B_cols+noise_V_cols])
ymax = max([max(table[n]) for n in noise_B_cols+noise_V_cols])
axB = plt.subplot(121)
axB.set_title('Noisy data - B filter')
axB.set_xlim(xmin, xmax)
axB.set_ylim(ymin, ymax)
axB.plot([xmin,xmax], [0,0], c='k')
axB.plot([0,0], [ymin,ymax], c='k')
lineB = axB.plot(table['x'], table[noise_B_cols[0]], 'o', c='g')
axB.plot(table['x'], table['cleanB'], c='k', linewidth=3)
def animateB(i):
    lineB[0].set_ydata(table[noise_B_cols[i]])
    return lineB
def initB():
    lineB[0].set_ydata(np.ma.array(table['x'], mask=True))
    return lineB
aniB = animation.FuncAnimation(fig, animateB, np.arange(0, len(noise_B_cols)), init_func=initB,
                               interval=300, blit=True)
axV = plt.subplot(122)
axV.set_title('Noisy data - V filter')
axV.set_xlim(xmin, xmax)
axV.set_ylim(ymin, ymax)
axV.plot([xmin,xmax], [0,0], c='k')
axV.plot([0,0], [ymin,ymax], c='k')
lineV = axV.plot(table['x'], table[noise_V_cols[0]], 'o', c='g')
axV.plot(table['x'], table['cleanV'], c='k', linewidth=3)
def animateV(i):
    lineV[0].set_ydata(table[noise_V_cols[i]])
    return lineV
def initV():
    lineV[0].set_ydata(np.ma.array(table['x'], mask=True))
    return lineV
aniV = animation.FuncAnimation(fig, animateV, np.arange(0, len(noise_V_cols)), init_func=initV,
                               interval=300, blit=True)
plt.show()

# We count the time of anything that is related with the fitting, including the
# creation of the parameters and the models
print '\nMinimization started...'
t1 = time.time()

# Setting up the B and V gaussian model components. They share the mean and the
# sigma parameters, but they have their own scales.
scaleB = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
scaleV = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
mean = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
sigma = p.EngineParameter(1., p.LogBoundConverter(.1, 2))

gausB = models.Gaussian1D(scaleB, mean, sigma)
gausV = models.Gaussian1D(scaleV, mean, sigma)

res_prov = []
# Go through the B frames and add the residual provider
for name in noise_B_cols:
    res_prov.append(m.Data1DResidualProvider(table['x'], table[name], gausB))
# Go through the V frames and add the residual provider
for name in noise_V_cols:
    res_prov.append(m.Data1DResidualProvider(table['x'], table[name], gausV))

# The engine will optimize all four parameters
manager = m.EngineParameterManager([scaleB, scaleV, mean, sigma])

# Setup the engine as solve the problem
estimator = m.ResidualEstimator(res_prov)
m.LeastSquareEngine(manager, estimator).solveProblem()

# End of the fitting
t2 = time.time()
showResults('No restrictions')
    
# Now add a restriction to the difference of the scaleB-scaleV. We give a big
# weight, so it will drive the minimization
print '\nMinimization started...'
t1 = time.time()
res_prov.append(m.ParameterDifferenceResidualProvider(scaleB, scaleV, 3., 100.))
estimator = m.ResidualEstimator(res_prov)
m.LeastSquareEngine(manager, estimator).solveProblem()
t2 = time.time()
showResults('B-V=3')

# Add another restriction to the value of the scaleB. Again we give a big weight
print '\nMinimization started...'
t1 = time.time()
res_prov.append(m.ParameterValueResidualProvider(scaleB, 5.6, 100.))
estimator = m.ResidualEstimator(res_prov)
m.LeastSquareEngine(manager, estimator).solveProblem()
t2 = time.time()
showResults('B-V=3 and B=5.6')