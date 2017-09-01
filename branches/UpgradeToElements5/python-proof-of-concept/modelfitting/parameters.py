import math


class Parameter(object):
    """Represents a model parameter
    
    This is the top level class of the inheritance tree that represents the
    model parameters.
    
    Each parameter has a value, which can be accessed with two different ways,
    directly via the getValue() method, or via an observer which is notified
    every time the value of the parameter is modified. This class implements the
    functionality of both these access ways.
    
    Note that this class provides the minimum interface required by the models,
    which includes only ways of getting the value of the parameter (models are
    not supposed to modify their parameter values). Setting the value of the
    parameter is delegated to the subclasses of the Parameter class, which
    implement the different update policies. This is supported via the
    _setValue() method, which has protected visibility.
    """
    
    def __init__(self, value):
        """Creates a new Parameter instance
        
        Args:
            value -- The initial value of the parameter
        """
        self._value = value
        self._observers = []
    
    
    def _setValue(self, new_value):
        """Sets the value of the parameter
        
        This is a protected method, meant to be used by the subclasses for
        setting the parameter value. If the result of the call is that the value
        of the parameter is modified, then the registered observers are notified
        by a call to their valueChanged() method.
        
        Args:
            new_value -- The new value of the parameter
        """
        if self._value == new_value:
            return
        self._value = new_value
        for obs in self._observers:
            obs.valueChanged(self._value)
    
    
    def getValue(self):
        """Returns the value of the parameter"""
        return self._value
    
    
    def addObserver(self, obs):
        """Adds an observer to the parameter
        
        This method adds an observer to the observers notified by this
        parameter. The given observer must provide the method
        valueChanged(value), which is called every time the value of the
        parameter is modified.
        
        Args:
            obs -- The observer to add
        """
        self._observers.append(obs)


###############################################################################

class PrintObserver:
    """An example of a parameter observer
    
    When the observed parameter value is changed, it is printed at the screen.
    """
    
    def __init__(self, message):
        self._message = message
    
    def valueChanged(self, x):
        print self._message, x


###############################################################################

class EngineParameter(Parameter):
    """Parameter which can be used by the minimization engine
    
    The EngineParameter class provides support for convertion between the world
    and minimization engine parameters. It provides the methods setEngineValue()
    and getEngineValue() for accessing the value in the engine coordinates and
    it delegates the conversion to an implementation of the CoordinateConverter
    interface, which provides the methods worldToEngine() and engineToWorld().
    
    This type of conversion is necessary, because the engine coordinate system
    must be unbounded, when some of the world coordinate values of the
    parameters can be bounded.
    """
    
    def __init__(self, world_value, converter):
        """Creates a new instance of EngineParameter
        
        Args:
            world_value -- The initial value of the parameter, expressed in
                    world coordinates
            converter -- The CoordinateConverter to use for transforming the
                    parameter values between the world and the engine coordinate
                    systems
        """
        super(EngineParameter, self).__init__(world_value)
        self._converter = converter
        self._engine_value = self._converter.worldToEngine(self.getValue())
        
    
    def getEngineValue(self):
        """Returns the value in the engine parameter space"""
        return self._engine_value
    
    
    def setEngineValue(self, new_engine_value):
        """Sets the parameter value in engine coordinates
        
        The world value of the parameter is also updated, according the
        converter of the EngineParameter. The observers listening for parameter
        modifications are notified for the world value.
        
        Args:
            new_engine_value -- The new parameter value, in engine coordinates
        """
        self._engine_value = new_engine_value
        super(EngineParameter, self)._setValue(self._converter.engineToWorld(new_engine_value))
        
        
    def _setValue(self, new_world_value):
        """The _setValue() method is overridden to also set the engine value"""
        self._engine_value = self._converter.worldToEngine(new_world_value)
        super(EngineParameter, self)._setValue(new_world_value)
    
    
###############################################################################
    
class UnboundConverter:
    """CoordinateConverter implementation which performs no conversion"""
    
    def worldToEngine(self, x_world):
        return x_world
    
    def engineToWorld(self, x_engine):
        return x_engine
    
    
###############################################################################
    
class LinBoundConverter:
    """CoordinateConverter implementation which uses the sigmoid function"""
    
    def __init__(self, min, max):
        """Creates a new instance of LinBoundConverter
        Args:
            min -- The minimum bound
            max -- The maximum bound
        """
        assert min < max
        self._min = float(min)
        self._max = float(max)
    
    def worldToEngine(self, x_world):
        assert self._min < x_world and x_world < self._max
        return math.log((x_world - self._min) / (self._max - x_world))
    
    def engineToWorld(self, x_engine):
        return self._min + (self._max - self._min) / (1 + math.exp(-x_engine))
    
    
###############################################################################

class LogBoundConverter:
    """CoordinateConverter implementation which uses the exponentiated sigmoid function"""
    
    def __init__(self, min, max):
        """Creates a new instance of LogBoundConverter
        Args:
            min -- The minimum bound
            max -- The maximum bound
        """
        assert min < max
        assert min > 0
        self._min = float(min)
        self._max = float(max)
    
    def worldToEngine(self, x_world):
        assert self._min < x_world and x_world < self._max
        return math.log(x_world / self._min) / math.log(self._max / x_world)
    
    def engineToWorld(self, x_engine):
        return self._min * math.exp(math.log(self._max / self._min) / (1 + math.exp(-x_engine)))
    
    
###############################################################################

class DependantParameter(Parameter):
    """Parameter implementation, the value of which is computed from other parameters
    
    This class can be used for parameters the value of which is derived from a
    set of other parameters. The computation of the parameter value is done by
    using an instance of the ValueCalculator interface, which is a class
    providing the calculate([p1,p2,...,pn]) method, which gets the values of the
    parameters this object depends on and returns its new value.
    """
    
    class _ListUpdater:
        """Internal class for updating the local parameter value list"""
        
        def __init__(self, outer, i):
            self._outer = outer
            self._i = i
            
        def valueChanged(self, value):
            self._outer._param_value_list[self._i] = value
            self._outer._recomputeValue()
    
    
    def __init__(self, parameter_list, calculator):
        """Creates a new instance of DependedParameter
        
        Args:
            parameter_list -- A list containing the parameters to depend on
            calculator -- A ValueCalculator implementation, for computing the
                    parameter value based on its dependencies
        """
        self._calculator = calculator
        self._param_value_list = []
        for p in parameter_list:
            self._param_value_list.append(p.getValue())
            p.addObserver(DependantParameter._ListUpdater(self, len(self._param_value_list)-1))
        super(DependantParameter, self).__init__(calculator.calculate(self._param_value_list))
    
            
    def _recomputeValue(self):
        """Helper method which recomputes the parameters value using the value calculator"""
        super(DependantParameter, self)._setValue(self._calculator.calculate(self._param_value_list))
        
        
###############################################################################

class ManualParameter(Parameter):
    """Parameter implementation which enables for manual value setting"""
    
    def __init__(self, value):
        super(ManualParameter, self).__init__(value)
    
    def setValue(self, value):
        self._setValue(value)