#!/usr/bin/env python

import sys
# Add in the pythonpath the dirctory with the modules
sys.path.append('..')

import detection.image as image
import detection.pixelproperties as pixprop
import detection.groupproperties as grprop
import detection.segmentation as segmentation
import detection.deblend as deblend

import astropy.io.fits as fits
from astropy.table import Table
import numpy as np
import warnings

# Load the detection image. The suppress of the warnings is done to not have some
# annoying messages about some invalid header keywords.
warnings.simplefilter('ignore', UserWarning)
im = fits.open('data/galaxies.fits')[0].data

# Create the background subtracted image by using the SubtractImage decorator.
# The background is represented by a lambda which always returns the double of
# the median of the detection image.
bg_value = 2 * np.median(im)
bg = image.SubtractImage(im, lambda x,y : bg_value)

# Set the pixel properties the framework will attach to each pixel. These properties
# are the value of the original image and the background subtracted value.
pixel_properties = {
    'Original' : pixprop.getImageValue(im),
    'BG' : pixprop.getImageValue(bg)
}

# Set the group properties the framework will compute for each pixel group. We
# calculate the min and max coordinates, the summary of all the pixels in the
# original and background subtracted images, the weighted centroid and the
# peak pixel
group_properties = {
    'MinXY' : grprop.MinXY(),
    'MaxXY' : grprop.MaxXY(),
    'Sum BG' : grprop.SumProperty('BG'),
    'Sum Original' : grprop.SumProperty('Original'),
    'Centroid' : grprop.WeightedCentroid('BG'),
    'Peak' : grprop.PeakPixel('BG')
}

# Create the segmentation algorithm object
algo = segmentation.SegmentationAlgo(pixel_properties=pixel_properties,
                                     group_properties=group_properties)
                                     
# A group listener which adds the group information to a table
t = Table(names=('ID','X', 'Y', 'X_PEAK','Y_PEAK','PIXELS','TOTAL_COUNTS','PEAK_COUNTS'),
          dtype=('i4','f4','f4','i4',    'i4',    'i4',    'f4',          'f4'))
def listener(group):
    t.add_row((group.id,
               group.properties['Centroid'][0],
               group.properties['Centroid'][1],
               group.properties['Peak'][0],
               group.properties['Peak'][1],
               len(group.pixel_list),
               group.properties['Sum BG'],
               group.properties['Peak'][2]))
               
# A group listener which gets the lutz pixel groups, performs deblending on them
# and adds them to a list
grouplist = []
lutz_count = 0
def listener(group):
    global lutz_count
    lutz_count += 1
    for g in deblend.deblend(group, group_properties, 'BG'):
        grouplist.append(g)

# A progress listener which prints the number of lines already processed
def progress(n):
    print >> sys.stderr, 'Done', n, '/', len(im),'\r',

# Call the algorithm with input the background subtracted image and group listener
# and progress listener the ones created above
algo.scan(bg, listener, progress)

# Print the results
print >> sys.stderr, '\n'
print >> sys.stderr, 'Groups before deblend:', lutz_count
print >> sys.stderr, 'Groups after deblend:', len(grouplist)
print >> sys.stderr, ''

t = Table(names=('ID','X', 'Y', 'X_PEAK','Y_PEAK','PIXELS','TOTAL_COUNTS','PEAK_COUNTS'),
          dtype=('i4','f4','f4','i4',    'i4',    'i4',    'f4',          'f4'))
id = 0
for group in grouplist:
    id += 1
    t.add_row((id,
               group.properties['Centroid'][0],
               group.properties['Centroid'][1],
               group.properties['Peak'][0],
               group.properties['Peak'][1],
               len(group.pixel_list),
               group.properties['Sum BG'],
               group.properties['Peak'][2]))
t.write(sys.stdout, format='ascii.fixed_width')