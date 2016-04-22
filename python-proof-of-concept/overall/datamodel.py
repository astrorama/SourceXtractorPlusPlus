import sys

class Property(object):
    """Superclass of all properties"""
    pass



class Source(object):
    """Interface representing a source, as it is viewed from all the parts of
    the system."""
    
    def getProperty(self, name):
        """Returns the requested property of the source"""
        raise NotImplementedError()    



class ObjectWithProperties(object):
    """Represents an object with properties. This class allows for accessing
    the properties and for adding new ones. Its reason of existance is to
    simplify the implementation of all the objects which have properties.
    """
    
    def __init__(self):
        """Create a new object with no properties"""
        self.properties = {}
        
    def _isPropertySet(self, name):
        """This is an method that can be used by subclasses (protected scope in 
        other languages) to check if the setProperty() has already been called
        for a specific property. This method should not be overridden by the
        subclasses."""
        return name in self.properties
        
    def getProperty(self, name):
        """Returns the requested property if it exists, None otherwise"""
        if self._isPropertySet(name):
            return self.properties[name]
        else:
            return None
        
    def setProperty(self, name, value):
        """Sets a property of the object. If the property already exists, it
        overrides its value"""
        self.properties[name] = value



class PixelSource(ObjectWithProperties, Source):
    """Represents a collection of pixels, which has some properties. Properties
    which are not yet set, are laizily initialized by using the pixel source task
    registry."""
    
    def __init__(self, pixel_list, task_registry):
        """Creates a new PixelSource.
        
        Args:
            pixel_list:
                A list containing the coordinates of the pixels of the source as
                (x,y) pairs
            task_registry:
                An object which provides tasks for computing properties of a
                PixelSource. It should provide the method getTask(property), which
                should return a callable object, that if is called with PixelSource
                as parameter, will compute and set this property.
        """
        super(PixelSource, self).__init__()
        self.pixel_list = pixel_list
        self.task_registry = task_registry
        
    def getPixelList(self):
        """Gives access to the pixel coordinates"""
        return self.pixel_list
    
    def getProperty(self, name):
        """Returns the requested property of the PixelSource.
        
        If the PixelSource does not contain such property, this method will try
        to use the task registry to compute it.
        """
        # If the property is not yet set, try to compute it
        if not self._isPropertySet(name):
            task = self.task_registry.getTask(name)
            if task != None:
                task(self)
            
        return super(PixelSource, self).getProperty(name)
    
class PixelSourceList(object):
    """ A list of PixelSource that have been put together for potential grouping. It contains a few
        hardcoded attribute that we know how to recompute efficiently when we add new sources to the list
        or when two lists are merged together.
        """
    
    def __init__(self, pixel_sources = []):
        self.pixel_sources = []
        self.min_coords = (sys.maxint, sys.maxint)
        self.max_coords = (-sys.maxint - 1, -sys.maxint - 1)

        for s in pixel_sources:
            self.append(s)
            
    def append(self, pixel_source):
        """ Appends one source to the list and recomputes what attributes are needed """
        
        self.pixel_sources.append(pixel_source)
        
        boundaries = pixel_source.getProperty('PixelBoundaries')
        
        self.min_coords = (min(self.min_coords[0], boundaries.getMin()[0]), min(self.min_coords[1], boundaries.getMin()[1]))
        self.max_coords = (max(self.max_coords[0], boundaries.getMax()[0]), max(self.max_coords[1], boundaries.getMax()[1]))
    
    def remove(self, pixel_source):
        """Removes a source from the list and recomputes what attributes are needed"""
        if not pixel_source in self.pixel_sources:
            return
        self.pixel_sources.remove(pixel_source)
        if len(self.pixel_sources) == 0:
            return
        s_bounds = pixel_source.getProperty('PixelBoundaries')
        s_min = s_bounds.getMin()
        s_max = s_bounds.getMax()
        if self.min_coords[0] == s_min[0] or self.min_coords[1] == s_min[1] or self.max_coords[0] == s_max[0] or self.max_coords[1] == s_max[1]:
            min_x = min(s.getProperty('PixelBoundaries').getMin()[0] for s in self.pixel_sources)
            min_y = min(s.getProperty('PixelBoundaries').getMin()[1] for s in self.pixel_sources)
            max_x = max(s.getProperty('PixelBoundaries').getMax()[0] for s in self.pixel_sources)
            max_y = max(s.getProperty('PixelBoundaries').getMax()[1] for s in self.pixel_sources)
            self.min_coords = (min_x, min_y)
            self.max_coords = (max_x, max_y)
        
        
    def merge(self, pixel_source_list):
        """ Merges another PixelSourceList to this list, it can be smart in recomputing attributes
         without having to reiterate on the whole list"""
        
        self.pixel_sources.extend(pixel_source_list.pixel_sources)
        
        self.min_coords = (min(self.min_coords[0], pixel_source_list.min_coords[0]), min(self.min_coords[1], pixel_source_list.min_coords[1]))
        self.max_coords = (max(self.max_coords[0], pixel_source_list.max_coords[0]), max(self.max_coords[1], pixel_source_list.max_coords[1]))
        
    def getPixelSources(self):
        """Gets a copy of the current list of PixelSources"""
        
        return self.pixel_sources[:]
    
    # minimum coordinates of the bounding box of the entire list
    def getMin(self):
        return self.min_coords
        
    # maximum coordinates of the bounding box of the entire list
    def getMax(self):
        return self.max_coords

    # Allows accessing PixelSourceList directly like a Python container
        
    def __iter__(self):
        return self.pixel_sources.__iter__()
    
    def __getitem__(self, index):
        return self.pixel_sources[index]

    def __len__(self):
        return len(self.pixel_sources)
       
        
class SourceGroup(ObjectWithProperties):
    """Represents a group of Sources, which should be handled together. It is
    itself an object with properties to accommodate the properties which belong
    to the group and not to a specific source"""
    
    class GroupedSource(ObjectWithProperties, Source):
        """Nested class of the SourceGroup, which introduce the idea that a
        source belongs to a group. It encapsulates a PixelSource and it delegates
        properties requests to it, if the properties are not locally available."""
        
        def __init__(self, pixel_source, group, task_registry):
            """Constructs a new instance of a GroupedSource.
            
            Args:
                pixel_source:
                    The pixel source to encapsulate
                group:
                    The SourceGroup in which the GroupedSource belongs
                task_registry:
                    An object which provides tasks for computing properties of a
                    SourceGroup. It should provide the method getTask(property), which
                    should return a callable object, that is called with SourceGroup
                    as parameter, will compute and set this property on the group, or
                    on the sources it contains.
            """
            super(SourceGroup.GroupedSource, self).__init__()
            self.pixel_source = pixel_source
            self.group = group
            self.task_registry = task_registry
    
        def getProperty(self, name):
            # If the property is already computed and attached to ourself, we return it  
            if self._isPropertySet(name):
                return super(SourceGroup.GroupedSource, self).getProperty(name)

            # If the property is already computed and attached to the group, we return it            
            if self.group._isPropertySet(name):
                return self.group.getProperty(name)
            
            # Try to get the property from the encapsulated object, to be computed if needed and possible            
            prop = self.pixel_source.getProperty(name)
            if prop != None:
                return prop
            
            # If we reached here, it means that we have to compute the property using a group task
            task = self.task_registry.getTask(name)
            if task == None:
                return None
            
            # Computes the property by executing the task
            task(self.group)

            # Now the property will be either at the group or at the local object
            if self.group._isPropertySet(name):
                return self.group.getProperty(name)
            else:
                return super(SourceGroup.GroupedSource, self).getProperty(name)
            
    def __init__(self, pixel_source_list, task_registry):
        """Constructs a SourceGroup from the given pixel source list"""
        super(SourceGroup, self).__init__()
        self.source_list = [SourceGroup.GroupedSource(s, self, task_registry) for s in pixel_source_list.getPixelSources()]
        self.task_registry = task_registry
        
    def getSources(self):
        """This returns a list of the contained sources. In C++ it should be
        exposing them as the Source superclass"""
        return self.source_list
    
    def getProperty(self, name):
        """Returns the requested property of the group.
        
        If the group does not contain such property, this method will try
        to use the task registry to compute it. If this is not possible, an
        UnknownProperty exception will be raised.
        """
        # The property is not yet set, so try to compute it
        if not self._isPropertySet(name):
            task = self.task_registry.getTask(name)
            if task != None:
                task(self)
            
        return super(SourceGroup, self).getProperty(name)

    
    