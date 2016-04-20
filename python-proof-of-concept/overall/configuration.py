
class OptionDescr(object):
    """Represents a program input option"""
    
    def __init__(self, name, type, help):
        self.name = name
        self.type = type
        self.help = help
        
        

class Configuration(object):
    """Parent class of all configurations
    
    Subclasses should override the methods getProgramOptions(), preInitialize(),
    initialize() and postInitialize(), as well as providing specific methods to
    access the objects they provide.
    """
    
    class State(object):
        """Defines the different states the configuration object can be in"""
        CONSTRUCTED = 1
        PRE_INITIALIZED = 2
        INITIALIZED = 3
        FINAL = 4
        
    def __init__(self, manager):
        self.manager = manager
        self.dependencies = set()
        self.state = Configuration.State.CONSTRUCTED
    
    def declareDependency(self, dep):
        """Configuration implementations can use this method in their constructor
        to declare their dependencies."""
        self.manager.registerConfiguration(dep)
        self.dependencies.add(dep)
        
    def getDependency(self, dep):
        """Configuration implementations can use this method to get their dependencies
        during the initialization phase"""
        return manager.getConfiguration(dep)
    
    def getProgramOptions(self):
        """Returns the program options defined by a specific configuration.
        Configuration implementations can override this method to return a
        list of the OptionDescr they require"""
        return []
    
    def preInitialize(self, user_values):
        """The Configuration implementations can override this method to implement
        actions which are fast and do not require their dependencies (like textual
        validation of the inputs). There is no guarantee on the order this method
        is called between the dependent configurations."""
        pass
    
    def initialize(self, user_values):
        """The Configuration implementations should override this method to implement
        their logic. When this method is called, all the configurations which are
        defined as dependencies by using the declareDependency() method are
        guaranteed to already be initialized."""
        pass
    
    def postInitialize(self, user_values):
        """This method is called after all configurations have been initialized. It
        can be overridden by Configurations which want to perform an action with
        the guarantee that all other Configurations are initialized. There is no
        guarantee on the order this method is called between the dependent
        configurations."""
        pass
    
    
    
class ConfigManager(object):
    """Manages a set of configuration classes
    
    A manager can be under the following states, which describe how it can be used:
    - Registration phase:
       This is the state a ConfigManager instance is in right after creation.
       During this phase configuration types can be added by using the
       registerConfiguration() method.
    - Waiting for initialization phase:
       A manager goes in this state after a call to the closeRegistration() method.
       From this state on, the registerConfiguration() method cannot be used any
       more. The closeRegistration() returns a boost options_description which
       describes the options for all the managed configurations (it can be called
       more than once).
    - Initialization phase:
       The initialization phase starts by calling the initialize() method. This
       method gets as arguments the user parameters to initialize the configurations
       with. During this phase, the manager performs three actions. First, it
       calls the preInitialize() method of all its managed configurations, in
       arbitrary order. Second, it calls the initialize() method of the managed
       configurations, in such order so all dependencies of a configuration have
       already been initialized before its method is called. Finally, it calls
       the postInitialize() methods, again in arbitrary order.
    - Initialized phase:
       When all the managed configurations have been initialized (as described
       above) the manager enters the initialized state. At this phase, the user
       can use the getConfiguration() method for retrieving the information he
       needs."""
    
    class State(object):
        """Defines the different states the manager object can be in"""
        REGISTRATION = 1
        WAITING_INITIALIZATION = 2
        INITIALIZED = 3

    def __init__(self):
        self.state = ConfigManager.State.REGISTRATION
        self.config_dict = {}
        self.dependency_map = {}
        
    def registerConfiguration(self, conf_type):
        """Registers a Configuration to the manager
        
        After this method is executed all the dependencies of the Configuration will
        also be registered (recursively). This method can be executed more than one
        time for the same configuration. Consecutive calls have no extra effect.
        """
        if self.state != ConfigManager.State.REGISTRATION:
            raise Exception("Manager is closed for configuration registration")
        print conf_type, conf_type in self.config_dict
        if not conf_type in self.config_dict:
            self.config_dict[conf_type] = None
            self.config_dict[conf_type] = conf_type(self)
    
    def closeRegistration(self):
        """Terminates the manager registration phase
    
        This call will make a test that there are no circular dependencies between
        the registered configurations. The returned options_description can be
        used for parsing the user input.
        """
        self.state = ConfigManager.State.WAITING_INITIALIZATION
        # Populate the dependencies map
        for c in self.config_dict:
            self.dependency_map[c] = self.config_dict[c].dependencies
        # Check for circular dependencies
        def hasCircularDependencies(dep_map, root, dep_set):
            if root in dep_set:
                return [root]
            for c in dep_set:
                found = hasCircularDependencies(dep_map, root, dep_map[c])
                if found:
                    return [c] + found
            return []
        for c in self.config_dict:
            found = hasCircularDependencies(self.dependency_map, c, self.dependency_map[c])
            if found:
                raise Exception("Circular dependency between configurations: "+str(found))
        # Return all the options
        all_options = []
        for c in self.config_dict.viewvalues():
            all_options.extend(c.getProgramOptions())
        return all_options
    
    def initialize(self, user_values):
        """nitialize the manager

        @details
        This method gets as arguments the user parameters to initialize the configurations
        with. During the initialization phase, the manager performs three actions. First, it
        calls the preInitialize() method of all its managed configurations, in
        arbitrary order. Second, it calls the initialize() method of the managed
        configurations, in such order so all dependencies of a configuration have
        already been initialized before its method is called. Finally, it calls
        the postInitialize() methods, again in arbitrary order.

        When this method returns, all the configurations are in FINAL state and the
        manager in INITIALIZED state."""
        self.state = ConfigManager.State.INITIALIZED
        
        for c in self.config_dict.viewvalues():
            c.preInitialize(user_values)
            c.state = Configuration.State.PRE_INITIALIZED
            
        def recursiveInitialization(conf_dict, dep_map, user_values, c):
            if conf_dict[c].state >= Configuration.State.INITIALIZED:
                return
            for dep in dep_map[c]:
                recursiveInitialization(conf_dict, dep_map, user_values, dep)
            conf_dict[c].initialize(user_values)
            conf_dict[c].state = Configuration.State.INITIALIZED
        for c in self.config_dict:
            recursiveInitialization(self.config_dict, self.dependency_map, user_values, c)
            
        for c in self.config_dict.viewvalues():
            c.postInitialize(user_values)
            c.state = Configuration.State.FINAL
    
    def getConfiguration(self, conf_type):
        """Returns the requested configuration"""
        if conf_type in self.config_dict:
            return self.config_dict[conf_type]
        else:
            return None