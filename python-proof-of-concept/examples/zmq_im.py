#!/usr/bin/env python

import zmq
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
import astropy.io.fits as fits
import warnings
import sys
from ast import literal_eval

if len(sys.argv) != 2 or not sys.argv[1] in ('SEGM', 'REF'):
    print 'Usage: zmq_im.py [SEGM|REF]'
    exit()

warnings.simplefilter('ignore', UserWarning)
det_im = fits.open('data/galaxies.fits')[0].data
im = det_im - det_im.min() + 1
im = np.sqrt(im)
im = im - im.min()
im = im.clip(max=im.max()/3.) - im.max()/10
im = im * 255 / im.max()
img = Image.fromarray(im)


fig, ax = plt.subplots()
ax.imshow(img, interpolation='nearest')

plt.show(block=False)

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("tcp://localhost:5556")
socket.setsockopt(zmq.SUBSCRIBE, sys.argv[1])

i = 0
xs = []
ys = []
#line, = ax.plot(xs, ys, 'o')
while True:
    i += 1
    msg = socket.recv()
    data = msg[msg.find(' ')+1:]
    coords = literal_eval(data)
    xs.append(coords[0])
    xs.append(coords[1])
    xlim = plt.xlim()
    ylim = plt.ylim()
    ax.scatter(coords[0], coords[1],s=200,edgecolor='r', facecolor='none')
#    line.set_xdata(xs)
#    line.set_ydata(ys)
    plt.xlim(xlim)
    plt.ylim(ylim)
#    plt.show(block=False)
    fig.canvas.draw()

while True:
    pass