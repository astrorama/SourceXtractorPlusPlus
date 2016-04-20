import tasks as tsk
import datamodel as dm
import configuration as conf

import sys
import os
import astropy.io.fits as fits

###############################################################################
# Configuration classes which are generic and can be used by more than one
# task factories
###############################################################################

class DetectionImageConfig(conf.Configuration):
    
    def __init__(self, manager):
        super(DetectionImageConfig, self).__init__(manager)
    
    def getProgramOptions(self):
        return [conf.OptionDescr('DETECTION_IMAGE', str, "The image to use for the detection")]
    
    def preInitialize(self, user_values):
        filename = user_values['DETECTION_IMAGE']
        if not os.path.isfile(filename):
            raise Exception('Detection image file '+filename+' does not exist')
    
    def initialize(self, user_values):
        self.image = fits.open(user_values['DETECTION_IMAGE'])[0].data
    
    def getImage(self):
        if self.state < self.State.INITIALIZED:
            raise Exception('getImage() call on uninitialized DetectionImageConfig')
        return self.image

###############################################################################
# Detection frame pixel values
###############################################################################

class DetectionFramePixelValues(dm.Property):
    
    def __init__(self, pixel_values):
        self.pixel_values = pixel_values
        
    def getPixelValues(self):
        return self.pixel_values
    
    

class DetectionFramePixelValuesTask(tsk.PixelSourceTask):
    
    def __init__(self, det_image):
        self.det_image = det_image
    
    def __call__(self, pixel_source):
        values = []
        for (x, y) in pixel_source.getPixelList():
            values.append(self.det_image[x][y])
        pixel_source.setProperty(DetectionFramePixelValues.__name__, DetectionFramePixelValues(values))
    
    
    
class DetectionFramePixelValuesFactory(tsk.TaskFactory):
    
    def __init__(self):
        self.task = None
        
    def getProducedProperties(self):
        return [DetectionFramePixelValues.__name__]
    
    def reportConfDependencies(self, config_manager):
        config_manager.registerConfiguration(DetectionImageConfig)
    
    def configure(self, config_manager):
        det_image = config_manager.getConfiguration(DetectionImageConfig).getImage()
        self.task = DetectionFramePixelValuesTask(det_image)
    
    def getTask(self):
        return self.task
    
    

tsk.task_registry.registerTaskFactory(DetectionFramePixelValuesFactory())


###############################################################################
# Pixel coordinate boundaries
###############################################################################

class PixelBoundaries(dm.Property):
    
    def __init__(self, min_x, max_x, min_y, max_y):
        self.min_coords = (min_x, min_y)
        self.max_coords = (max_x, max_y)
        
    def getMin(self):
        return self.min_coords
        
    def getMax(self):
        return self.max_coords
    
    

class PixelBoundariesTask(tsk.PixelSourceTask):
        
    def __call__(self, pixel_source):
        min_x = sys.maxint
        max_x = 0
        min_y = sys.maxint
        max_y = 0
        for p in pixel_source.getPixelList():
            if p[0] < min_x:
                min_x = p[0]
            if p[0] > max_x:
                max_x = p[0]
            if p[1] < min_y:
                min_y = p[1]
            if p[1] > max_y:
                max_y = p[1]
        pixel_source.setProperty(PixelBoundaries.__name__, PixelBoundaries(min_x, max_x, min_y, max_y))
    
    
    
class PixelBoundariesFactory(tsk.TaskFactory):
    
    def __init__(self):
        self.task = PixelBoundariesTask()
    
    def getProducedProperties(self):
        return [PixelBoundaries.__name__]
    
    def getTask(self):
        return self.task
    
    

tsk.task_registry.registerTaskFactory(PixelBoundariesFactory())


###############################################################################
# Pixel source stamp of the detection frame
###############################################################################

class DetectionFramePixelSourceStamp(dm.Property):
    
    def __init__(self, stamp):
        self.stamp = stamp
        
    def getStamp(self):
        return self.stamp
    
    

class DetectionFramePixelSourceStampTask(tsk.PixelSourceTask):
    
    def __init__(self, det_image):
        self.det_image = det_image
        
    def __call__(self, pixel_source):
        boundaries = pixel_source.getProperty('PixelBoundaries')
        min_x, min_y = boundaries.getMin()
        max_x, max_y = boundaries.getMax()
        stamp = self.det_image[min_y:max_y+1, min_x:max_x+1]
        pixel_source.setProperty(DetectionFramePixelSourceStamp.__name__, DetectionFramePixelSourceStamp(stamp))
    
    
    
class DetectionFramePixelSourceStampFactory(tsk.TaskFactory):
    
    def __init__(self):
        self.task = None
    
    def getProducedProperties(self):
        return [DetectionFramePixelSourceStamp.__name__]
    
    def reportConfDependencies(self, config_manager):
        config_manager.registerConfiguration(DetectionImageConfig)
        
    def configure(self, config_manager):
        det_image = config_manager.getConfiguration(DetectionImageConfig).getImage()
        self.task = DetectionFramePixelSourceStampTask(det_image)
    
    def getTask(self):
        return self.task
    
    

tsk.task_registry.registerTaskFactory(DetectionFramePixelSourceStampFactory())
