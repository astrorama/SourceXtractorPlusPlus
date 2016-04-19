
class Property(object):
    """Superclass of all properties"""
    pass



class ObjectWithProperties(object):
    """Represents an object with properties. This class allows for accessing
    the properties and for adding new ones.
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
        """Returns the requested property if it exsists, None otherwise"""
        if self._isPropertySet(name):
            return self.properties[name]
        else:
            return None
        
    def setProperty(self, name, value):
        """Sets a property of the object. If the property already exists, it
        overrides its value"""
        self.properties[name] = value



class PixelGroup(ObjectWithProperties):
    """Represents a collection of pixels, which has some properties. Properties
    which are not yet set, are laizily initialized by using the pixel group task
    manager."""
    
    def __init__(self, pixel_list, task_mgr):
        """Creates a new PixelGroup.
        
        Args:
            pixel_list:
                A list containing the coordinates of the pixels of the source
            task_mgr:
                An object which provides tasks for computing properties of a
                PixelGroup. It should provide the method getTask(property), which
                should return a callable object, that if is called with PixelGroup
                as parameter, will compute and set this property.
        """
        super(PixelGroup, self).__init__()
        self.pixel_list = pixel_list
        self.task_mgr = task_mgr
        
    def getPixelList(self):
        """Gives access to the pixel coordinates"""
        return self.pixel_list
    
    def getProperty(self, name):
        """Returns the requested property of the PixelGroup.
        
        If the PixelGroup does not contain such property, this method will try
        to use the task manager to compute it.
        """
        # If the property is not yet set, try to compute it
        if not self._isPropertySet(name):
            task = self.task_mgr.getTask(name)
            if task != None:
                task(self)
            
        return super(PixelGroup, self).getProperty(name)




class Source(ObjectWithProperties):
    """Represents a source. A source encapsulates a PixelGroup and it delegates
    to it properties requests, if the properties are not locally available."""
    
    def __init__(self, pixel_group):
        super(Source, self).__init__()
        self.pixel_group = pixel_group
    
    def getProperty(self, name):
        """Returns the requested property of the Source. If the source does not
        contain such property, it delegates the request to the encapsulated
        pixel group."""
        if self._isPropertySet(name):
            return super(Source, self).getProperty(name)
        else:
            return self.pixel_group.getProperty(name)
        
        
        
        
class SourceGroup(ObjectWithProperties):
    """Represents a group of Sources, which should be handled together. It is
    itself an object with properties to acomodate the properties which belong
    to the group and not to a specific source"""
    
    class GroupedSource(Source):
        """Nested class of the SourceGroup, which introduce the idea that a
        source belongs to a group."""
        
        def __init__(self, pixel_group, group, task_mgr):
            """Constructs a new instance of a GroupedSource.
            
            Args:
                pixel_group:
                    The pixel group to encapsulate
                group:
                    The SourceGroup in which the GroupedSource belongs
                task_mgr:
                    An object which provides tasks for computing properties of a
                    SourceGroup. It should provide the method getTask(property), which
                    should return a callable object, that if is called with SourceGroup
                    as parameter, will compute and set this property on the group, or
                    on the sources it contains.
            """
            super(SourceGroup.GroupedSource, self).__init__(pixel_group)
            self.group = group
            self.task_mgr = task_mgr
    
        def getProperty(self, name):
            # If the property is alreday computed and attached to the group, we return it
            # We don't call directly the group.getProperty() to not initiate a computation
            # at the case that this is a property produced by a GroupTask, attached to the
            # sources.
            if self.group._isPropertySet(name):
                return self.group.getProperty(name)
            
            # We try to get the property from the local object. This will return somthing
            # in both cases that this is a property produced by a GroupTask that has
            # already been executed and the case that it is a property of the
            # encapsulated PixelGroup (which might be computed at this call).
            prop = super(SourceGroup.GroupedSource, self).getProperty(name)
            if prop != None:
                return prop

            # If we reached here, means that we have to compute the property using
            # a group task
            task = self.task_mgr.getTask(name)
            if task == None:
                return None
            task(self.group)

            # Now the property will be either at the group or at the local object
            if self.group._isPropertySet(name):
                return self.group.getProperty(name)
            else:
                return super(SourceGroup.GroupedSource, self).getProperty(name)
    
    
    def __init__(self, pixel_group_list, task_mgr):
        """Constructs a SourceGroup from the given pixel group list"""
        super(SourceGroup, self).__init__()
        self.source_list = [SourceGroup.GroupedSource(pg, self, task_mgr) for pg in pixel_group_list]
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

    
    