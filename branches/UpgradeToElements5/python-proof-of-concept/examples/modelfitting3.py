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
import time


# This is a helper method for ploting the frames
def plotFrames(show_noise=False, show_clean=False, show_model=False, title=''):
    fig = plt.figure()
    xmin = min(table['x'])
    xmax = max(table['x'])
    ymin = min([min(table[n]) for n in noise_cols])
    ymax = max([max(table[n]) for n in noise_cols])
    ax = plt.axes(xlim=(xmin,xmax), ylim=(ymin,ymax))
    ax.set_title(title)
    ax.plot([xmin,xmax], [0,0], c='k')
    ax.plot([0,0], [ymin,ymax], c='k')
    if show_noise:
        line_noise = ax.plot(table['x'], table[noise_cols[0]], 'o', c='g')[0]
    if show_clean:
        line_clean = ax.plot(table['x'], table[clean_cols[0]], c='k', linewidth=3)[0]
    if show_model:
        line_model = ax.plot(table['x'], table[clean_cols[0]], c='r', linewidth=2)[0]
    epoch_text = ax.text(0.01, 0.95, '', transform=ax.transAxes)
    def init():
        epoch_text.set_text('')
        result = [epoch_text]
        if show_noise:
            line_noise.set_data([],[])
            result.append(line_noise)
        if show_clean:
            line_clean.set_data([],[])
            result.append(line_clean)
        if show_model:
            line_model.set_data([],[])
            result.append(line_model)
        return result
    def animate(i):
        epoch_text.set_text('Epoch = '+str(epochs[i]))
        result = [epoch_text]
        if show_noise:
            line_noise.set_data(table['x'], table[noise_cols[i]])
            result.append(line_noise)
        if show_clean:
            line_clean.set_data(table['x'], table[clean_cols[i]])
            result.append(line_clean)
        if show_model:
            line_model.set_data(table['x'], [frame_models[i](xi) for xi in table['x']])
            result.append(line_model)
        return result
    anim = animation.FuncAnimation(fig, animate, init_func=init,
                                   frames=len(noise_cols), interval=300, blit=True)
    plt.show()


# Read the data from the file
table = Table.read('data/moving_gaussian.fits', format='fits')

# Print the header information
print ''
print 'Headers in file:'
for k,v in table.meta.items():
    print '   ',k,'=',v

# Get the names of the columns containing the frames
noise_cols = [n for n in table.colnames if 'noise' in n]
clean_cols = [n for n in table.colnames if 'clean' in n]
epochs = sorted([table.meta[n] for n in table.meta.keys() if 'EPOCH' in n])

# Plot the frames
plotFrames(show_noise=True, title='Noisy data')

# Plot the frames with the clean observed object
plotFrames(show_noise=True, show_clean=True, title='Noisy data - Moving object')

# We count the time of anything that is related with the fitting, including the
# creation of the parameters and the models
print '\nMinimization started...'
t1 = time.time()

# Setting the parameters that will be optimized by the engine
scale = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
mean0 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
step = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
sigma = p.EngineParameter(1., p.LogBoundConverter(.1, 2))

# This class is a ValueCalculator which computes the mean for a specific epoch.
# The first parameter is the mean0 and the second the step.
class EpochStep:
    def __init__(self, epoch):
        self._epoch = epoch
    def calculate(self, v):
        return v[0] + self._epoch * v[1]

# We use a residual provider for each frame. In each frame the frame model is
# a gaussian. All the frames share the same scale and sigma, but they have a
# mean which depends on the epoch
res_prov = []
frame_models = []
for noise_col, epoch in zip(noise_cols, epochs):
    # We create a new parameter for the frame mean. It depends on the mean0 and
    # step parameters via the EpochStep ValueCalculator. Note that this parameter
    # is not managed by the minimization engine
    mean_i = p.DependantParameter([mean0, step], EpochStep(epoch))
    # The gaussian model for this frame
    gaus = models.Gaussian1D(scale, mean_i, sigma)
    res_prov.append(m.Data1DResidualProvider(table['x'], table[noise_col], gaus))
    # We keep the model for presentation reasons
    frame_models.append(gaus)

# The engine optimizes only the four parameters
manager = m.EngineParameterManager([scale, mean0, step, sigma])

# Setup the engine and solve the problem
estimator = m.ResidualEstimator(res_prov)
m.LeastSquareEngine(manager, estimator).solveProblem()

# End of the fitting
t2 = time.time()
print ''
print 'Time of fitting:', (t2-t1)
print ''
print 'From modeling:'
print '    Scale =', scale.getValue()
print '    Mean0 =', mean0.getValue()
print '     Step =', step.getValue()
print '    Sigma =', sigma.getValue()
print ''

# Plot the results
plotFrames(show_noise=True, show_clean=True, show_model=True, title='Fitted gaussians')