
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
    manager."""
    
    def __init__(self, pixel_list, task_mgr):
        """Creates a new PixelSource.
        
        Args:
            pixel_list:
                A list containing the coordinates of the pixels of the source as
                (x,y) pairs
            task_mgr:
                An object which provides tasks for computing properties of a
                PixelSource. It should provide the method getTask(property), which
                should return a callable object, that if is called with PixelSource
                as parameter, will compute and set this property.
        """
        super(PixelSource, self).__init__()
        self.pixel_list = pixel_list
        self.task_mgr = task_mgr
        
    def getPixelList(self):
        """Gives access to the pixel coordinates"""
        return self.pixel_list
    
    def getProperty(self, name):
        """Returns the requested property of the PixelSource.
        
        If the PixelSource does not contain such property, this method will try
        to use the task manager to compute it.
        """
        # If the property is not yet set, try to compute it
        if not self._isPropertySet(name):
            task = self.task_mgr.getTask(name)
            if task != None:
                task(self)
            
        return super(PixelSource, self).getProperty(name)
                
        
        
class SourceGroup(ObjectWithProperties):
    """Represents a group of Sources, which should be handled together. It is
    itself an object with properties to accommodate the properties which belong
    to the group and not to a specific source"""
    
    class GroupedSource(PixelSource, Source):
        """Nested class of the SourceGroup, which introduce the idea that a
        source belongs to a group. It encapsulates a PixelSource and it delegates
        properties requests to it, if the properties are not locally available."""
        
        def __init__(self, pixel_source, group, task_mgr):
            """Constructs a new instance of a GroupedSource.
            
            Args:
                pixel_source:
                    The pixel source to encapsulate
                group:
                    The SourceGroup in which the GroupedSource belongs
                task_mgr:
                    An object which provides tasks for computing properties of a
                    SourceGroup. It should provide the method getTask(property), which
                    should return a callable object, that is called with SourceGroup
                    as parameter, will compute and set this property on the group, or
                    on the sources it contains.
            """
            super(SourceGroup.GroupedSource, self).__init__()
            self.pixel_source = pixel_source
            self.group = group
            self.task_mgr = task_mgr
    
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
            task = self.task_mgr.getTask(name)
            if task == None:
                return None
            
            # Computes the property by executing the task
            task(self.group)

            # Now the property will be either at the group or at the local object
            if self.group._isPropertySet(name):
                return self.group.getProperty(name)
            else:
                return super(SourceGroup.GroupedSource, self).getProperty(name)
    
    def __init__(self, pixel_source_list, task_mgr):
        """Constructs a SourceGroup from the given pixel source list"""
        super(SourceGroup, self).__init__()
        self.source_list = [SourceGroup.GroupedSource(s, self, task_mgr) for s in pixel_source_list]
        self.task_mgr = task_mgr
        
    def getSources(self):
        """This returns a list of the contained sources. In C++ it should be
        exposing them as the Source superclass"""
        return self.source_list
    
    def getProperty(self, name):
        """Returns the requested property of the group.
        
        If the group does not contain such property, this method will try
        to use the task manager to compute it. If this is not possible, an
        UnknownProperty exception will be raised.
        """
        # The property is not yet set, so try to compute it
        if not self._isPropertySet(name):
            task = self.task_mgr.getTask(name)
            if task != None:
                task(self)
            
        return super(SourceGroup, self).getProperty(name)

    
    