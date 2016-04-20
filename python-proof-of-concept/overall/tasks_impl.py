import tasks as tsk
import datamodel as dm

import sys
import astropy.io.fits as fits


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
    
    def getOptions(self):
        return ['DetectionImage']
    
    def configure(self, options):
        self.task = DetectionFramePixelValuesTask(fits.open(options['DetectionImage'])[0].data)
    
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
    
    def getOptions(self):
        return ['DetectionImage']
        
    def configure(self, options):
        self.task = DetectionFramePixelSourceStampTask(fits.open(options['DetectionImage'])[0].data)
    
    def getTask(self):
        return self.task
    
    

tsk.task_registry.registerTaskFactory(DetectionFramePixelSourceStampFactory())
    
    
###############################################################################

# In the rest of the file as some dummy classes for testing

task_count = 0

class DummyPixelSourceTask(object):
    
    def __init__(self, prop_list):
        global task_count
        task_count += 1
        self.task_id = task_count
        self.prop_list = prop_list
        
    def __call__(self, pixel_source):
        for p in self.prop_list:
            pixel_source.setProperty(p, "Set by pixel source task "+str(self.task_id))
    
    

class DummyPixelSourceTaskManager(object):
    
    def __init__(self):
        self.available_tasks = [
            ['pg_1', 'pg_2'],
            ['pg_3', 'pg_4'],
        ]
    
    def getTask(self, prop_name):
        for prop_list in self.available_tasks:
            if prop_name in prop_list:
                return DummyPixelSourceTask(prop_list)
        return None
    

class DummySourceGroupTask(object):
    
    def __init__(self, source_prop_list, group_prop_list):
        global task_count
        task_count += 1
        self.task_id = task_count
        self.source_prop_list = source_prop_list
        self.group_prop_list = group_prop_list
    
    def __call__(self, source_group):
        for p in self.group_prop_list:
            source_group.setProperty(p, "Set by source group task "+str(self.task_id))
        for s in source_group.getSources():
            for p in self.source_prop_list:
                s.setProperty(p, "Set by source group task "+str(self.task_id))
    
    

class DummySourceGroupTaskManager(object):
    
    def __init__(self):
        self.available_tasks = [
            [['s_1', 's_2'], ['sg_1', 'sg_2']],
            [['s_3', 's_4'], ['sg_3', 'sg_4']]
        ]
    
    def getTask(self, prop_name):
        for task in self.available_tasks:
            if prop_name in task[0] or prop_name in task[1]:
                return DummySourceGroupTask(task[0], task[1])
        return None