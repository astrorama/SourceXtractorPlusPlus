
class Configurable(object):
    """Interface of an object which can be configured"""
    
    def reportConfDependencies(self, config_manager):
        """Configurable implementations should implement this method to register
        to the given ConfigManager (using the registerConfiguration(type) method)
        all the Configurations it is going to use"""
        pass
    
    def configure(self, config_manager):
        """Method which should initialize the object. All the required Configurations
        can be accessed using the getConfiguration(type) method of the given manager"""
        pass
    
    
    
class PixelSourceListener(object):
    """Interface of classes which can listen of PixelSource events"""
    
    def handlePixelSource(self, pixel_source):
        """Implementations should override this method to implement their logic
        of handling a PixelSource"""
        raise NotImplementedError()
    


class SourceGroupListener(object):
    """Interface of classes which can listen of SourceGroup events"""
    
    def handleSourceGroup(self, pixel_source):
        """Implementations should override this method to implement their logic
        of handling a SourceGroup"""
        raise NotImplementedError()
        
    
    
class PixelSourceEventGenerator(object):
    
    def __init__(self):
        self.listeners = []
    
    def addPixelSourceListener(self, listener):
        """Adds a listener which will be notified for PixelSources"""
        self.listeners.append(listener)
        
    def _notifyListeners(self, pixel_source):
        """This is a protected method which the subclasses should use in their
        implementations to notify the listeners for the SourcePixels they
        generate"""
        for l in self.listeners:
            l.handlePixelSource(pixel_source)



class GroupSourceEventGenerator(object):
    
    def __init__(self):
        self.listeners = []
    
    def addPixelSourceListener(self, listener):
        """Adds a listener which will be notified for GroupSource"""
        self.listeners.append(listener)
        
    def _notifyListeners(self, source_group):
        """This is a protected method which the subclasses should use in their
        implementations to notify the listeners for the GroupSource they
        generate"""
        for l in self.listeners:
            l.handleSourceGroup(source_group)



class SourceListener(object):
    """Interface of classes which can listen of Source events"""
    
    def handleSource(self, source):
        """Implementations should override this method to implement their logic
        of handling a Source"""
        raise NotImplementedError()



class SelectionCriteria(object):
    
    def mustBeProcessed(self, source):
        raise NotImplementedError()



class ProcessSourceListener(object):
    
    def processSources(self, selection_criteria):
        raise NotImplementedError()



