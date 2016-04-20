#!/usr/bin/env python

import sys
# Add in the pythonpath the dirctory with the modules
sys.path.append('..')

import overall.tasks as tsk
import overall.datamodel as dm
import overall.tasks_impl as tsk_impl

import detection.image as image
import detection.segmentation as segmentation
import detection.deblend as deblend

import astropy.io.fits as fits
import numpy as np
import warnings

det_im_name = 'data/galaxies.fits'

# Load the detection image. The suppress of the warnings is done to not have some
# annoying messages about some invalid header keywords.
warnings.simplefilter('ignore', UserWarning)
im = fits.open(det_im_name)[0].data

# Create the background subtracted image by using the SubtractImage decorator.
# The background is represented by a lambda which always returns the double of
# the median of the detection image.
bg_value = 2 * np.median(im)
bg = image.SubtractImage(im, lambda x,y : bg_value)

# Create the segmentation algorithm object. We use the existing implementation of
# the Lutz algorithm from the detection prototype, but we do not use its properties
# functionality
algo = segmentation.SegmentationAlgo(pixel_properties={}, group_properties={})



options = {
    'DetectionImage' : det_im_name
}
tsk.task_registry.configure(options)


# The pixel source list, as they are computed by the Lutz
pixel_source_list = []

# A listener which implements the overview design in demonstration
def listener(lutz_group):
    # first we need to convert the detection prototype data model to the overview one
    pixel_list = []
    for p in lutz_group.pixel_list:
        pixel_list.append((p.x, p.y))
    pixel_source_list.append(dm.PixelSource(pixel_list, tsk.task_registry))


# Execute the segmentation
print 'Starting segmentation process...'
algo.scan(bg, listener)
print 'Segmentation finished. Detected', len(pixel_source_list), 'pixel sources'


# The pixel source list, as they are exiting the refinement process
refined_pixel_source_list = []


# This loop simulates the pixel source refinement box, calling all the refinement steps
print 'Starting the pixel source refinement process...'
for pixel_source in pixel_source_list:
    
    #############################################################
    # We simulate a single refinement step, by using attractors
    #############################################################
    stamp = pixel_source.getProperty('DetectionFramePixelSourceStamp').getStamp()
    bounds = pixel_source.getProperty('PixelBoundaries')
    minxy = bounds.getMin()
    maxxy = bounds.getMax()
    def value(x,y):
        if (x < minxy[0] or x > maxxy[0] or y < minxy[1] or y > maxxy[1]):
            return -1
        return im[x - minxy[0]][y - minxy[1]]
    
    # Regroup the pixels based on attractors
    pix_loc = [(p, p) for p in pixel_source.getPixelList()]
    attractors = {}
    deblend._attractPixels(pix_loc, attractors, value)
    merged = deblend._mergeAttractors(attractors)
    
    # If we end up with a single group use the original group
    if len(merged) == 1:
        refined_pixel_source_list.append(pixel_source)
    else:
        # else we need to construct the new groups
        for minxy,maxxy,pixels in merged:
            refined_pixel_source_list.append(dm.PixelSource(pixels, tsk.task_registry))
    #############################################################
    
print 'pixel source refinement finished with ', len(refined_pixel_source_list), 'pixel sources'