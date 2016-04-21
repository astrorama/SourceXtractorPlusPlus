import maincomponents as mc

class PixelSourceTask():
    """Interface for classes which compute pixel source properties"""
    
    def __call__(self, pixel_group):
        """This should populate the given pixel source with the result properties
        of the task. It accesses the pixels of the source using the getPixelList()"""
        raise NotImplementedError()
    

    
    
class SourceGroupTask():
    """Interface for classes which compute source group properties"""
    
    def __call__(self, source_group):
        """This should populate the given group and its sources with the result
        properties of the task. It accesses the sources using the getSources()"""
        raise NotImplementedError()
    
    


class TaskFactory(mc.Configurable):
    """For each type of tasks a factory should be provided, which will be responsible
    for instansiating the specific task, based on the user configuration"""
    
    def getProducedProperties(self):
        """This should return which properties are produced by this factory tasks"""
        raise NotImplementedError()
    
    
    
    
class SourceTaskFactory(TaskFactory):
    """Factory which produces SourceTasks"""
    
    def getTask(self):
        """Returns an instance of a SourceTask"""
        raise NotImplementedError()
    
   
    
    
class GroupTaskFactory(TaskFactory):
    """Factory which produces GroupTasks"""
    
    def getTask(self):
        """Returns an instance of a GroupTask"""
        raise NotImplementedError()
        
      
    
class TaskRegistry(mc.Configurable):
    
    def __init__(self):
        self.factories = []
        self.prop_factory_map = {}
        
    def registerTaskFactory(self, task_factory):
        for prop in task_factory.getProducedProperties():
            if prop in self.prop_factory_map:
                raise Exception("Multiple task factories for property "+prop)
            else:
                self.factories.append(task_factory)
                self.prop_factory_map[prop] = task_factory
                
    def reportConfDependencies(self, config_manager):
        for f in self.factories:
            f.reportConfDependencies(config_manager)
    
    def configure(self, config_manager):
        for f in self.factories:
            f.configure(config_manager)
            
    def getAvailableProperties(self):
        return self.prop_factory_map.keys()
    
    def getTask(self, prop):
        if prop in self.prop_factory_map:
            return self.prop_factory_map[prop].getTask()
        else:
            return None



task_registry = TaskRegistry()
