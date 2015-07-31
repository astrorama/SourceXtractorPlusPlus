import segmentation
import sys


class MinXY(segmentation.GroupProperty):
    
    def initValue(self):
        return (sys.maxint, sys.maxint)
    
    def nextValue(self, current_value, pixel):
        x = pixel.x if pixel.x < current_value[0] else current_value[0]
        y = pixel.y if pixel.y < current_value[1] else current_value[1]
        return (x, y)


class MaxXY(segmentation.GroupProperty):
    
    def initValue(self):
        return (0, 0)
    
    def nextValue(self, current_value, pixel):
        x = pixel.x if pixel.x > current_value[0] else current_value[0]
        y = pixel.y if pixel.y > current_value[1] else current_value[1]
        return (x, y)
    
    
class BoundaryFlag(segmentation.GroupProperty):
    
    def __init__(self, xmin, ymin, xmax, ymax):
        self._xmin = xmin
        self._xmax = xmax
        self._ymin = ymin
        self._ymax = ymax
    
    def initValue(self):
        return False
    
    def nextValue(self, current_value, pixel):
        edge = pixel.x <= self._xmin or pixel.y <= self._ymin or pixel.x >= self._xmax or pixel.y >= self._ymax
        return current_value or edge
    
    
class ExternalFlag(segmentation.GroupProperty):
    
    def __init__(self, pix_prop_name, type):
        self._name = pix_prop_name
        if type == 'OR' or type == 'or':
            self._action = lambda x,y: x or y
            self._type = 'OR'
        else:
            self._action = lambda x,y: x and y
            self._type = 'AND'
    
    def initValue(self):
        return False if self._type == 'OR' else True
    
    def nextValue(self, current_value, pixel):
        return self._action(current_value, pixel.properties[self._name])
    
    
class SumProperty(segmentation.GroupProperty):
    
    def __init__(self, pix_prop_name):
        self._name = pix_prop_name
    
    def initValue(selfself):
        return 0
    
    def nextValue(self, current_value, pixel):
        return current_value + pixel.properties[self._name]


class WeightedCentroid(segmentation.GroupProperty):
    
    def __init__(self, value_prop_name):
        self._val_name = value_prop_name
    
    def initValue(self):
        return (0., 0., 0.)
    
    def nextValue(self, current_value, pixel):
        pix_val = pixel.properties[self._val_name]
        total = current_value[2] + pix_val
        x = current_value[0] + (pixel.x - current_value[0]) * pix_val / total
        y = current_value[1] + (pixel.y - current_value[1]) * pix_val / total
        return (x, y, total)
    

class PeakPixel(segmentation.GroupProperty):
    
    def __init__(self, value_prop_name):
        self._val_name = value_prop_name
    
    def initValue(self):
        return (0, 0, 0.)
    
    def nextValue(self, current_value, pixel):
        if pixel.properties[self._val_name] < current_value[2]:
            return current_value
        else:
            return (pixel.x, pixel.y, pixel.properties[self._val_name])