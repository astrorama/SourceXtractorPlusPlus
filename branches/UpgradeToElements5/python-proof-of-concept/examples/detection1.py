#!/usr/bin/env python

import sys
# Add in the pythonpath the dirctory with the modules
sys.path.append('..')

import detection.image as image
import detection.pixelproperties as pixprop
import detection.groupproperties as grprop
import detection.segmentation as segmentation

import astropy.io.fits as fits

# Load the detection image
im = fits.open('data/lutz_detection.fits')[1].data

# Create the background subtracted image by using the SubtractImage decorator.
# The background is represented by a lambda which always returns 1.
bg = image.SubtractImage(im, lambda x,y : 1.)

# Read the images that contain the three flags
flag1 = fits.open('data/flag_1.fits')[1].data
flag2 = fits.open('data/flag_2.fits')[1].data
flag3 = fits.open('data/flag_3.fits')[1].data

# Set the pixel properties the framework will attach to each pixel. These properties
# are the three external flags for the given pixel, the value of the original
# image and the background subtracted value.
pixel_properties = {
    'Flag1' : pixprop.getImageFlag(flag1),
    'Flag2' : pixprop.getImageFlag(flag2),
    'Flag3' : pixprop.getImageFlag(flag3),
    'Original' : pixprop.getImageValue(im),
    'BG' : pixprop.getImageValue(bg)
}

# Set the group properties the framework will compute for each pixel group. We
# calculate the min and max coordinates, a flag showing if it touches the boundaries
# of the image, if any of the pixels of the group have set the first or the second
# flag and if all the pixels of the group have set the third flag.
group_properties = {
    'MinXY' : grprop.MinXY(),
    'MaxXY' : grprop.MaxXY(),
    'Boundary Flag' : grprop.BoundaryFlag(0,0,11,len(im)-1),
    'External Flag1 (OR)' : grprop.ExternalFlag('Flag1','OR'),
    'External Flag2 (OR)' : grprop.ExternalFlag('Flag2','OR'),
    'External Flag3 (AND)' : grprop.ExternalFlag('Flag3','AND')
}

# Create the segmentation algorithm object
algo = segmentation.SegmentationAlgo(pixel_properties=pixel_properties,
                                     group_properties=group_properties)
                                     
# A listener which appends the pixel groups in a list
grouplist = []
def listener(group):
    grouplist.append(group)

# Call the algorithm with input the background subtracted image and group listener
# the one created above
algo.scan(bg, listener)

# Show the information of the groups and their pixels
print ''
print 'Total number of groups detected:', len(grouplist)
for group in grouplist:
    print ''
    print 'Pixel Group:', group.id
    for p,v in group.properties.items():
        print ' ',p,':',v
    print '  Number of pixels:', len(group.pixel_list)
    for pix in group.pixel_list:
        print '    Pixel ('+str(pix.x)+','+str(pix.y)+')'+' = '+str(pix.value)
        for k,v in pix.properties.items():
            print '      ',k,':',v