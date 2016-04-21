#!/usr/bin/env python

import sys
# Add in the pythonpath the dirctory with the modules
sys.path.append('..')

import overall.configuration as conf
import overall.maincomponents as mc
import overall.tasks as tsk
import overall.segmentation as segm
import overall.pixelrefinement as pref
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
#    'BACKGROUND_VALUE' : 10000,
    'DETECTION_IMAGE' : 'data/galaxies.fits',
    'DETECT_MINAREA' : 5,
    'DEBLEND_ALGORITHM' : 'OFF',
    'DEBLEND_ALGORITHM' : 'ATRACTORS',
    'SEGMENTATION_ALGORITHM' : 'LUTZ'
}

# We initialize the config manager and we use it to configure all the Configurables
conf_mgr.initialize(user_values)
tsk.task_registry.configure(conf_mgr)
segm_factory.configure(conf_mgr)
pix_ref_factory.configure(conf_mgr)
out_handler.configure(conf_mgr)

# We get the main components which are constructed by factories
segm_algo = segm_factory.getSegmentation()
pix_ref_algo = pix_ref_factory.getPixelSourceRefinement()

# We connect the main components with each other
segm_algo.addPixelSourceListener(pix_ref_algo)
class listener:
    def handlePixelSource(self, s):
        out_handler.handleSource(s)
pix_ref_algo.addPixelSourceListener(listener())

# We retrieve the detection image from the configuration
det_im = conf_mgr.getConfiguration(tsk_impl.DetectionImageConfig).getImage()

# We scan the detection image, using the segmentation algorithm
segm_algo.scan(det_im)