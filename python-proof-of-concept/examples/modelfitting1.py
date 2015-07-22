#!/usr/bin/env python

import sys
# Add in the pythonpath the dirctory with the modules
sys.path.append('..')

import modelfitting.parameters as p
import modelfitting.minimization as m
import modelfitting.models as models

from astropy.table import Table
import matplotlib.pyplot as plt
import time

# Read the data from the file and plot the noisy ones
table = Table.read('data/two_gaussians.fits', format='fits')

print ''
print 'Headers in file:'
for k,v in table.meta.items():
    print '   ',k,'=',v
    
plt.title('Noisy data')
xmin = min(table['x'])
xmax = max(table['x'])
ymin = min(table['noise'])
ymax = max(table['noise'])
plt.xlim(xmin, xmax)
plt.ylim(ymin, ymax)
plt.plot([xmin,xmax], [0,0], c='k')
plt.plot([0,0], [ymin,ymax], c='k')
plt.plot(table['x'], table['noise'], 'o', c='g')
plt.plot(table['x'], table['clean'], c='k', linewidth=2)
plt.show()

# We count the time of anything that is related with the fitting, including the
# creation of the parameters and the models
print '\nMinimization started...'
t1 = time.time()

# Setting up the first gaussian model component to use for the fitting
scale1 = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
mean1 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
sigma1 = p.EngineParameter(1., p.LogBoundConverter(.1, 2))
g1 = models.Gaussian1D(scale1, mean1, sigma1)

# Setting up the second gaussian model component to use for the fitting
scale2 = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
mean2 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
sigma2 = p.EngineParameter(1., p.LogBoundConverter(.1, 2))
g2 = models.Gaussian1D(scale2, mean2, sigma2)

# We use a single residual provider, because we have a single frame. The frame
# model is the addition of the two gaussian model components
res_prov = m.Data1DResidualProvider(table['x'], table['noise'], lambda xi: g1(xi)+g2(xi))

# The engine will optimize all six parameters of the two gaussian models
manager = m.EngineParameterManager([scale1, mean1, sigma1, scale2, mean2, sigma2])

# Setup the engine as solve the problem
estimator = m.ResidualEstimator([res_prov])
m.LeastSquareEngine(manager, estimator).solveProblem()

# End of the fitting
t2 = time.time()
print ''
print 'Time of fitting:', (t2-t1)
print ''
print 'From modeling:'
print '    Scale1 =', scale1.getValue()
print '    Mean1 =', mean1.getValue()
print '    Sigma1 =', sigma1.getValue()
print '    Scale2 =', scale2.getValue()
print '    Mean2 =', mean2.getValue()
print '    Sigma2 =', sigma2.getValue()
print ''

x = table['x']
plt.title('Fitted gaussians')
plt.xlim(xmin, xmax)
plt.ylim(ymin, ymax)
plt.plot([xmin,xmax], [0,0], c='k')
plt.plot([0,0], [ymin,ymax], c='k')
plt.plot(x, table['noise'], 'o', c='g')
plt.plot(x, table['clean'], c='k', linewidth=3)
plt.plot(x, [g1(xi)+g2(xi) for xi in x], c='r', linewidth=3)
plt.show()