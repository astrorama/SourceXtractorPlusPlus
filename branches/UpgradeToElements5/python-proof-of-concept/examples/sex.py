#!/usr/bin/env python

import sys
# Add in the pythonpath the dirctory with the modules
sys.path.append('..')

import overall.configuration as conf
import overall.maincomponents as mc
import overall.tasks as tsk
import overall.segmentation as segm
import overall.sourcegrouping as sgrp
import overall.pixelrefinement as pref
import overall.grouprefinement as gref
import overall.output as out

# We import all the tasks implementations. This will register them to the
# task manager
import overall.tasks_impl as tsk_impl

# If the user just asked for the available properties we print them
if '-p'in sys.argv:
    print ''
    print 'Available outputs:'
    print ''
    for c in out.output_column_manager.getColumnNames():
        print '    ', c
    print ''
    exit()

# This is the configuration manager to use everywhere
conf_mgr = conf.ConfigManager()

# We will need the detection image, so we reuqest the related configuration
conf_mgr.registerConfiguration(tsk_impl.DetectionImageConfig)

# We pass the config manager to the task registry to set it up for all the
# available tasks
tsk.task_registry.reportConfDependencies(conf_mgr)

# We get the configuration requirements of the SegmentationFactory
segm_factory = segm.SegmentationFactory()
segm_factory.reportConfDependencies(conf_mgr)

# We get the requirements of the PixelSourceRefinementFactory
pix_ref_factory = pref.PixelSourceRefinementFactory()
pix_ref_factory.reportConfDependencies(conf_mgr)

# Configuring the SourceGroupingFactory
source_grouping_factory = sgrp.SourceGroupingFactory(tsk.task_registry)
source_grouping_factory.reportConfDependencies(conf_mgr)

grp_ref_factory = gref.SourceGroupRefinementFactory()
grp_ref_factory.reportConfDependencies(conf_mgr)

# We setup the output handler
out_handler = out.OutputHandler()
out_handler.reportConfDependencies(conf_mgr)

# We end the ConfigManager registration phase. This returns all the options
# defined by all the registered Configurations
opt_desc = conf_mgr.closeRegistration()

# If the user just asked for the available configuration we print it
if '-d' in sys.argv:
    print ''
    print 'Available options:'
    print ''
    for d in opt_desc:
        print '    ', d.name, '('+d.type.__name__+') : ', d.help
    print ''
    exit()

# Here normally we pass the descriptions to whatever framework we use and it
# parses the user file. We simulate the result with a manual dictionary
user_values = {
    'OUT_COLUMNS' : 'PIX_CENTROID_X PIX_CENTROID_Y',
    'BACKGROUND_VALUE' : 9000,
    'DETECTION_IMAGE' : 'data/galaxies.fits',
    'DETECT_MINAREA' : 10,
#    'DEBLEND_ALGORITHM' : 'OFF',
    'DEBLEND_ALGORITHM' : 'ATTRACTORS',
    'SEGMENTATION_ALGORITHM' : 'LUTZ',
    'NEIGHBOR_MERGING' : 'ON'
}

# We initialize the config manager and we use it to configure all the Configurables
conf_mgr.initialize(user_values)
tsk.task_registry.configure(conf_mgr)
segm_factory.configure(conf_mgr)
pix_ref_factory.configure(conf_mgr)
source_grouping_factory.configure(conf_mgr)
grp_ref_factory.configure(conf_mgr)
out_handler.configure(conf_mgr)

# We get the main components which are constructed by factories
segm_algo = segm_factory.getSegmentation()
pix_ref_algo = pix_ref_factory.getPixelSourceRefinement()
source_grouping = source_grouping_factory.getSourceGrouping()
grp_ref_algo = grp_ref_factory.getSourceGroupRefinement()

sources = []
class listener:
    def handleSourceGroup(self, source_group):
        for s in source_group.getSources():
            out_handler.handleSource(s)
            sources.append(s)
        
# We connect the main components with each other
segm_algo.addPixelSourceListener(pix_ref_algo)
pix_ref_algo.addPixelSourceListener(source_grouping)
source_grouping.addPixelSourceListListener(grp_ref_algo)
grp_ref_algo.addSourceGroupListener(listener())

#grp_ref_algo.addSourceGroupListener(listener())

# We retrieve the detection image from the configuration
det_im = conf_mgr.getConfiguration(tsk_impl.DetectionImageConfig).getImage()

# If the user requested zeroqm messages add the listeners
if '-zmq' in sys.argv:
    import zmq
    context = zmq.Context()
    socket = context.socket(zmq.PUB)
    socket.bind("tcp://*:5556")
    class segm_listener:
        def handlePixelSource(self, source):
            socket.send('SEGM '+str(source.getProperty(tsk_impl.PixelCentroid.__name__).getCentroid()))
    segm_algo.addPixelSourceListener(segm_listener())
    class ref_listener:
        def handlePixelSource(self, source):
            socket.send('REF '+str(source.getProperty(tsk_impl.PixelCentroid.__name__).getCentroid()))
    pix_ref_algo.addPixelSourceListener(ref_listener())
    
if '-slow' in sys.argv:
    import time
    class slow_listener:
        def handlePixelSource(self,s):
            time.sleep(1)
    segm_algo.addPixelSourceListener(slow_listener())

# We scan the detection image, using the segmentation algorithm
segm_algo.scan(det_im)

source_grouping.processSources(sgrp.CriteriaAllSources())



# If the user requested for a visual output show it
if '-im' in sys.argv:
    import matplotlib.pyplot as plt
    import numpy as np
    from PIL import Image
    centroids = [s.getProperty(tsk_impl.PixelCentroid.__name__).getCentroid() for s in sources]
    xs = [x for x,y in centroids]
    ys = [y for x,y in centroids]
    im = det_im - det_im.min() + 1
    im = np.sqrt(im)
    im = im - im.min()
    im = im.clip(max=im.max()/3.) - im.max()/10
    im = im * 255 / im.max()
    img = Image.fromarray(im)
    plt.imshow(img, interpolation='nearest')
    xlim = plt.xlim()
    ylim = plt.ylim()
    plt.scatter(xs,ys,s=200,edgecolor='r', facecolor='none')
    plt.xlim(xlim)
    plt.ylim(ylim)
    plt.show()