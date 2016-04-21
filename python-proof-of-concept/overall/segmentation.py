
import maincomponents as mc
import configuration as conf




class Segmentation(mc.PixelSourceEventGenerator):
    """Superclass of classes which can be used for the segmentation"""
    
    def __init__(self):
        super(Segmentation, self).__init__()
    
    def scan(self, image):
        """All Segmentation implementations must implement the scan method to
        extract PixelSources from the given image. They should call the method
        _notifyListeners() with all the detected PixelSources"""
        raise NotImplementedError()
    
    
    

class SegmentationFactoryConfig(conf.Configuration):
    
    def __init__(self, manager):
        super(SegmentationFactoryConfig, self).__init__(manager)
    
    def getProgramOptions(self):
        return [conf.OptionDescr('SEGMENTATION_ALGORITHM', str, "One of LUTZ, DBSCAN, ...")]
    
    def preInitialize(self, user_values):
        if not 'SEGMENTATION_ALGORITHM' in user_values:
            raise Exception('Missing obligatory parameter SEGMENTATION_ALGORITHM')
        if user_values['SEGMENTATION_ALGORITHM'] != 'LUTZ':
            raise Exception("Only LUTZ algorithm is currently implemented for segmentation!")
    
    def initialize(self, user_values):
        self.algo = user_values['SEGMENTATION_ALGORITHM']
        
    def getAlgo(self):
        return self.algo
    
    
    
    
class SegmentationFactory(mc.Configurable):
    
    def reportConfDependencies(self, config_manager):
        config_manager.registerConfiguration(SegmentationFactoryConfig)
    
    def configure(self, config_manager):
        self.algo = config_manager.getConfiguration(SegmentationFactoryConfig).getAlgo()
    
    def getSegmentation(self):
        if self.algo == 'LUTZ':
            from lutz import Lutz
            return Lutz()