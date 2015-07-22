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
    plt.plot([0,0], [ymin,ymax], c='k')
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
table = Table.read('data/three_moving_gaussians.fits', format='fits')

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
scale1 = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
mean01 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
step1 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
sigma1 = p.EngineParameter(1., p.LogBoundConverter(.1, 2))
scale2 = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
mean02 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
step2 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
sigma2 = p.EngineParameter(1., p.LogBoundConverter(.1, 2))
scale3 = p.EngineParameter(1., p.LogBoundConverter(.5, 10))
mean03 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
step3 = p.EngineParameter(0., p.LinBoundConverter(-5, 5))
sigma3 = p.EngineParameter(1., p.LogBoundConverter(.1, 2))

# This class is a ValueCalculator which computes the mean for a specific epoch.
# The first parameter is the mean0 and the second the step.
class EpochStep:
    def __init__(self, epoch):
        self._epoch = epoch
    def calculate(self, v):
        return v[0] + self._epoch * v[1]
    
class AddModels:
    def __init__(self, models):
        self._models = models
    def __call__(self, v):
        return sum([m(v) for m in self._models])

# We use a residual provider for each frame. In each frame the frame model is
# the sum of three gaussians. The gaussians share between the frames the same
# scale and sigma, but they have a mean which depends on the epoch
res_prov = []
frame_models = []
for noise_col, epoch in zip(noise_cols, epochs):
    # We create a new parameter for the mean of each gaussian for this frame. It
    # depends on the mean0 and step parameters via the EpochStep ValueCalculator.
    # Note that these parameters are not managed by the minimization engine
    mean1_i = p.DependantParameter([mean01, step1], EpochStep(epoch))
    mean2_i = p.DependantParameter([mean02, step2], EpochStep(epoch))
    mean3_i = p.DependantParameter([mean03, step3], EpochStep(epoch))
    # The three gaussian model for this frame
    gaus1 = models.Gaussian1D(scale1, mean1_i, sigma1)
    gaus2 = models.Gaussian1D(scale2, mean2_i, sigma2)
    gaus3 = models.Gaussian1D(scale3, mean3_i, sigma3)
    # The frame model is the sum of all three gaussians
    frame_model = AddModels([gaus1, gaus2, gaus3])
    res_prov.append(m.Data1DResidualProvider(table['x'], table[noise_col], frame_model))
    # We keep the model for presentation reasons
    frame_models.append(frame_model)

# The engine optimizes only the 12 main parameters
manager = m.EngineParameterManager([scale1, mean01, step1, sigma1,
                                    scale2, mean02, step2, sigma2,
                                    scale3, mean03, step3, sigma3])

# Setup the engine and solve the problem
estimator = m.ResidualEstimator(res_prov)
m.LeastSquareEngine(manager, estimator).solveProblem()

# End of the fitting
t2 = time.time()
print ''
print 'Time of fitting:', (t2-t1)
print ''
print 'From modeling:'
print '    Scale1 =', scale1.getValue()
print '    Mean01 =', mean01.getValue()
print '     Step1 =', step1.getValue()
print '    Sigma1 =', sigma1.getValue()
print '    Scale2 =', scale2.getValue()
print '    Mean02 =', mean02.getValue()
print '     Step2 =', step2.getValue()
print '    Sigma2 =', sigma2.getValue()
print '    Scale3 =', scale3.getValue()
print '    Mean03 =', mean03.getValue()
print '     Step3 =', step3.getValue()
print '    Sigma3 =', sigma3.getValue()
print ''

# Plot the results
plotFrames(show_noise=True, show_clean=True, show_model=True, title='Fitted gaussians')