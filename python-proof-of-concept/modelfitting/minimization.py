import parameters


class EngineParameterManager:
    """Class responsible for updating the parameters minimized by the engine"""
    
    def __init__(self, parameter_list):
        """Creates a new EngineParameterManager, which manages the given parameters
        
        Args:
            parameter_list -- A list with the parameters the engine will optimize
        """
        assert len(parameter_list) > 0
        for p in parameter_list:
            assert issubclass(type(p), parameters.EngineParameter)
        self._parameter_list = parameter_list
    
    def getEngineValues(self):
        """Returns the engine values of the managed parameters
        
        The values are returned in the format the engine can use them. For this
        example it is a list, which is what scipy.optimize.leastsq expects.
        """
        result = []
        for p in self._parameter_list:
            result.append(p.getEngineValue())
        return result
    
    def updateEngineValues(self, values_list):
        """Updates the managed parameter values with the given engine values
        
        Note that the world values and any dependant parameters are updated
        automatically.
        
        Args:
             values_list -- The values in engine coordinantes to update the
                    managed parameters with
        """
        for v,p in zip(values_list, self._parameter_list):
            p.setEngineValue(v)


###############################################################################

class ResidualEstimator:
    """Class responsible for providing to the minimization engine with the residuals

    The residuals buffer is splitted in blocks. This class contains a list with
    ResidualBlockProviders, one responsible for each block.
    
    The residuals are provided to the engine in the format it can use them. For
    this example it is a list, which is what scipy.optimize.leastsq expects.
    """
    
    def __init__(self, provider_list):
        """Creates a new ResidualEstimator instance
        
        Args:
            provider_list -- A list with the ResidualBlockProviders for computing
                    the blocks of the residuals buffer
        """
        self._provider_list = provider_list
    
    def getResiduals(self):
        """Returns the residuals buffer in the format the engine can use it"""
        residuals = []
        for p in self._provider_list:
            residuals.extend(p.getResiduals())
        return residuals
    

###############################################################################

class LeastSquareEngine:
    """Wrapper arround the scipy.optimize.leastsq minimization engine
    
    This class uses an EngineParameterManager for getting the initial engine
    parameters values and for updating them, a ResidualEstimator for computing
    the residuals and the scipy leastsq minimization engine.
    """
    
    def __init__(self, param_manager, residual_estimator):
        self._param_manager = param_manager
        self._residual_estimator = residual_estimator
    
    def solveProblem(self):
        from scipy.optimize import leastsq
        def costFunction(param_values):
            self._param_manager.updateEngineValues(param_values)
            return self._residual_estimator.getResiduals()
        result = leastsq(costFunction, self._param_manager.getEngineValues())
        self._param_manager.updateEngineValues(result[0])
        return result
    
    
###############################################################################

class Data1DResidualProvider:
    """RedisualBlockProvider implementation which computes the differences between
       real data and 1D model values."""
    
    def __init__(self, x_data, y_data, model):
        self._x_data = x_data
        self._y_data = y_data
        self._model = model
    
    def getResiduals(self):
        res = []
        for xi, yi in zip(self._x_data, self._y_data):
            res.append(yi - self._model(xi))
        return res


###############################################################################

class ParameterValueResidualProvider:
    """ResidualBlockProvider implementation which applies a prior to a Parameter value"""
    
    def __init__(self, param, value, weight):
        self._param = param
        self._value = value
        self._weight = weight
    
    def getResiduals(self):
        return [self._weight * (self._param.getValue() - self._value)]
    

###############################################################################

class ParameterDifferenceResidualProvider:
    """ResidualBlockProvider implementation which applies a prior to the difference
       of two parameters values"""
    
    def __init__(self, param1, param2, value, weight):
        self._param1 = param1
        self._param2 = param2
        self._value = value
        self._weight = weight
    
    def getResiduals(self):
        return [self._weight * (self._param1.getValue() - self._param2.getValue() - self._value)]