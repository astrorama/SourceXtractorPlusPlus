import maincomponents as mc
import configuration as conf
import datamodel as dm
import tasks as tsk

import sys


class PixelSourceRefineAction(object):
    """Interface of classes than can be used for refining a PixelSource by the
    PixelSourceRefinement"""
    
    def refine(self, pixel_source):
        """This method should be implemented to return a list of zero or more
        pixel_sources, based on the input pixel_source. If no refinement is
        required, the input should just be forwared"""
        raise NotImplementedError()
        
        
        
        
class MinAreaRefineAction(PixelSourceRefineAction):
    """Refinement which rejects all sources which do not have enough pixels"""
    
    
    def __init__(self, min_pix_count):
        self.min_pix_count = min_pix_count
    
    def refine(self, pixel_source):
        if len(pixel_source.getPixelList()) < self.min_pix_count:
            return []
        else:
            return [pixel_source]
        
        


class AtractorsRefineAction(PixelSourceRefineAction):
    """Refnement using atractors (similar to DBSCAN)"""
    
    def refine(self, pixel_source):
        stamp = pixel_source.getProperty('DetectionFramePixelSourceStamp').getStamp()
        bounds = pixel_source.getProperty('PixelBoundaries')
        minxy = bounds.getMin()
        maxxy = bounds.getMax()
        def value(x,y):
            if (x < minxy[0] or x > maxxy[0] or y < minxy[1] or y > maxxy[1]):
                return -sys.float_info.max
            return stamp[x - minxy[0], y - minxy[1]]

        # Regroup the pixels based on attractors
        pix_loc = [(p, p) for p in pixel_source.getPixelList()]
        attractors = {}
        import detection.deblend as deblend
        deblend._attractPixels(pix_loc, attractors, value)
        merged = deblend._mergeAttractors(attractors)

        # If we end up with a single group use the original group
        out_list = []
        if len(merged) == 1:
            out_list.append(pixel_source)
        else:
            # else we need to construct the new groups
            for minxy,maxxy,pixels in merged:
                out_list.append(dm.PixelSource(pixels, tsk.task_registry))
        return out_list
    
    
    
        
        
class PixelSourceRefinement(mc.PixelSourceEventGenerator, mc.PixelSourceListener):
    """Class responsible for the pixel refinement. It is a PixelSourceListener
    so it can be connected with the source of the PixelSources. For each pixel
    source it performs a set of refinement actions and it notifies its own
    listeners for the PixelSources at the end of the chain."""
    
    def __init__(self, action_list):
        super(PixelSourceRefinement, self).__init__()
        self.action_list = action_list
    
    def handlePixelSource(self, pixel_source):
        step_input_list = [pixel_source]
        for action in self.action_list:
            step_output_list = []
            for source in step_input_list:
                step_output_list.extend(action.refine(source))
            step_input_list = step_output_list
        # Here all actions are done, so notify the listeners
        for source in step_input_list:
            self._notifyListeners(source)
        
        
        
        
        
class PixelSourceRefinementConfig(conf.Configuration):
    """Configuration class which is responsible to handle the user input to decide
    which PixelSourceRefineActions will be used"""
    
    def __init__(self, manager):
        super(PixelSourceRefinementConfig, self).__init__(manager)
        self.action_list = []
    
    def getProgramOptions(self):
        return [
            conf.OptionDescr('DEBLEND_ALGORITHM', str, "One of OFF, LUTZ, DBSCAN, ATRACTORS, etc..."),
            conf.OptionDescr('DETECT_MINAREA', int, "min. # of pixels to be considered a detection")
        ]
    
    def preInitialize(self, user_values):
        if not 'DEBLEND_ALGORITHM' in user_values:
            raise Exception('Missing obligatory parameter DEBLEND_ALGORITHM')
        if not user_values['DEBLEND_ALGORITHM'] in ('OFF', 'ATRACTORS'):
            raise Exception("Only ATRACTORS algorithm is currently implemented for deblending!")
    
    def initialize(self, user_values):
        # First chose which deblending algorithm we use
        algo = user_values['DEBLEND_ALGORITHM']
        if (algo == 'ATRACTORS'):
            self.action_list.append(AtractorsRefineAction())
        # As the last action we filter out any source with less than the specified
        # number of pixels
        if 'DETECT_MINAREA' in user_values:
            minarea = user_values['DETECT_MINAREA']
            if minarea > 1:
                self.action_list.append(MinAreaRefineAction(minarea))
                
    def getRefinementActions(self):
        return self.action_list
    
    
    
    
class PixelSourceRefinementFactory(mc.Configurable):
    
    def reportConfDependencies(self, config_manager):
        config_manager.registerConfiguration(PixelSourceRefinementConfig)
    
    def configure(self, config_manager):
        self.action_list = config_manager.getConfiguration(PixelSourceRefinementConfig).getRefinementActions()
        
    def getPixelSourceRefinement(self):
        return PixelSourceRefinement(self.action_list)

