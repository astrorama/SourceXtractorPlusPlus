import math


class Gaussian1D:
    """Implements the univariate Gaussian model"""
    
    def __init__(self, scale, mean, sigma):
        """Creates a new instance of Gaussian1D model, with the given parameters
        
        Args:
            scale -- The scale parameter of the model
            mean -- The mean parameter of the model
            sigma -- The sigma parameter of the model
        """
        self._scale = scale
        self._mean = mean
        self._sigma = sigma
    
    def __call__(self, x):
        """Returns the value of the model for the given x value"""
        temp = (x - self._mean.getValue()) / self._sigma.getValue()
        return self._scale.getValue() * math.exp(-.5 * temp * temp)