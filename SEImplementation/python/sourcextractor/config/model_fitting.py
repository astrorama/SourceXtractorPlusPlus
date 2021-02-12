# -*- coding: utf-8 -*-

# Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
from __future__ import division, print_function

import sys
from enum import Enum

import _SourceXtractorPy as cpp
from .measurement_images import MeasurementGroup

from astropy import units as u
from astropy.coordinates import SkyCoord
from astropy.coordinates import Angle

import math


class RangeType(Enum):
    LINEAR = 1
    EXPONENTIAL = 2


class Range(object):
    r"""
    Limit, and normalize, the range of values for a model fitting parameter.


    Parameters
    ----------
    limits : a tuple (min, max), or a callable that receives a source, and returns a tuple (min, max)
    type : RangeType

    Notes
    -----
    RangeType.LINEAR
        Normalized to engine space using a sigmoid function

        .. math::

            engine = \ln \frac{world - min}{max-world} \\
            world = min + \frac{max - min}{1 + e^{engine}}

    RangeType.EXPONENTIAL
        Normalized to engine space using an exponential sigmoid function

        .. math::

            engine = \ln \left( \frac{\ln(world/min)}{\ln(max /world)} \right) \\
            world = min * e^\frac{ \ln(max / min) }{ (1 + e^{-engine}) }

    """

    def __init__(self, limits, type):
        """
        Constructor.
        """
        self.__limits = limits
        self.__callable = limits if hasattr(limits, '__call__') else lambda v, o: limits
        self.__type = type

    def get_min(self, v, o):
        """
        Parameters
        ----------
        v : initial value
        o : object being fitted

        Returns
        -------
        The minimum acceptable value for the range
        """
        return self.__callable(v, o)[0]

    def get_max(self, v, o):
        """
        Parameters
        ----------
        v : initial value
        o : object being fitted

        Returns
        -------
        The maximum acceptable value for the range
        """
        return self.__callable(v, o)[1]

    def get_type(self):
        """
        Returns
        -------
        RangeType
        """
        return self.__type

    def __str__(self):
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        res = '['
        if hasattr(self.__limits, '__call__'):
            res += 'func'
        else:
            res += '{},{}'.format(self.__limits[0], self.__limits[1])
        type_str = {
            RangeType.LINEAR : 'LIN',
            RangeType.EXPONENTIAL : 'EXP'
        }
        res += ',{}]'.format(type_str[self.__type])
        return res


class Unbounded(object):
    """
    Unbounded, but normalize, value of a model fitting parameter

    Parameters
    ----------
    normalization_factor: float, or a callable that receives the initial value parameter value and a source,
        and returns a float
        The world value which will be normalized to 1 in engine coordinates
    """
    
    def __init__(self, normalization_factor=1):
        """
        Constructor.
        """
        self.__normalization_factor = normalization_factor
        if hasattr(normalization_factor, '__call__'):
            self.__callable = normalization_factor
        else:
            self.__callable = lambda v, o: normalization_factor
    
    def get_normalization_factor(self, v, o):
        """
        Parameters
        ----------
        v : initial value
        o : object being fitted

        Returns
        -------
        float
            The world value which will be normalized to 1 in engine coordinates
        """
        return self.__callable(v, o)
    
    def __str__(self):
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        res = '['
        if hasattr(self.__normalization_factor, '__call__'):
            res += 'func'
        else:
            res += '{}'.format(self.__normalization_factor)
        res += ']'
        return res


constant_parameter_dict = {}
free_parameter_dict = {}
dependent_parameter_dict = {}


def print_parameters(file=sys.stderr):
    """
    Print a human-readable representation of the configured model fitting parameters.

    Parameters
    ----------
    file : file object
        Where to print the representation. Defaults to sys.stderr
    """
    if constant_parameter_dict:
        print('Constant parameters:', file=file)
        for n in constant_parameter_dict:
            print('  {}: {}'.format(n, constant_parameter_dict[n]), file=file)
    if free_parameter_dict:
        print('Free parameters:', file=file)
        for n in free_parameter_dict:
            print('  {}: {}'.format(n, free_parameter_dict[n]), file=file)
    if dependent_parameter_dict:
        print('Dependent parameters:', file=file)
        for n in dependent_parameter_dict:
            print('  {}: {}'.format(n, dependent_parameter_dict[n]), file=file)


class ParameterBase(cpp.Id):
    """
    Base class for all model fitting parameter types.
    Can not be used directly.
    """

    def __str__(self):
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        return '(ID:{})'.format(self.id)


class ConstantParameter(ParameterBase):
    """
    A parameter with a single value that remains constant. It will not be fitted.

    Parameters
    ----------
    value : float, or callable that receives a source and returns a float
        Value for this parameter
    """

    def __init__(self, value):
        """
        Constructor.
        """
        ParameterBase.__init__(self)
        self.__value = value
        self.__callable = value if hasattr(value, '__call__') else lambda o: value
        constant_parameter_dict[self.id] = self

    def get_value(self, o):
        """
        Parameters
        ----------
        o : object being fitted

        Returns
        -------
        float
            Value of the constant parameter
        """
        return self.__callable(o)

    def __str__(self):
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        res = ParameterBase.__str__(self)[:-1] + ', value:'
        if hasattr(self.__value, '__call__'):
            res += 'func'
        else:
            res += str(self.__value)
        return res + ')'


class FreeParameter(ParameterBase):
    """
    A parameter that will be fitted by the model fitting engine.

    Parameters
    ----------
    init_value : float or callable that receives a source, and returns a float
        Initial value for the parameter.
    range : instance of Range or Unbounded
        Defines if this parameter is unbounded or bounded, and how.

    See Also
    --------
    Unbounded
    Range

    Examples
    --------
    >>> sersic = FreeParameter(2.0, Range((1.0, 7.0), RangeType.LINEAR))
    """

    def __init__(self, init_value, range=Unbounded()):
        """
        Constructor.
        """
        ParameterBase.__init__(self)
        self.__init_value = init_value
        self.__init_call = init_value if hasattr(init_value, '__call__') else lambda o: init_value
        self.__range = range
        free_parameter_dict[self.id] = self

    def get_init_value(self, o):
        """
        Parameters
        ----------
        o : object being fitted

        Returns
        -------
        float
            Initial value for the free parameter
        """
        return self.__init_call(o)

    def get_range(self):
        """
        Returns
        -------
        Unbounded or Range
        """
        return self.__range

    def __str__(self):
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        res = ParameterBase.__str__(self)[:-1] + ', init:'
        if hasattr(self.__init_value, '__call__'):
            res += 'func'
        else:
            res += str(self.__init_value)
        if self.__range:
            res += ', range:' + str(self.__range)
        return res + ')'


class DependentParameter(ParameterBase):
    """
    A DependentParameter is not fitted by itself, but its value is derived from another Parameters, whatever their type:
    FreeParameter, ConstantParameter, or other DependentParameter

    Parameters
    ----------
    func : callable
        A callable that will be called with all the parameters specified in this constructor each time a new
        evaluation is needed.
    params : list of ParameterBase
        List of parameters on which this DependentParameter depends.

    Examples
    --------
    >>> flux = get_flux_parameter()
    >>> mag = DependentParameter(lambda f: -2.5 * np.log10(f) + args.mag_zeropoint, flux)
    >>> add_output_column('mf_mag_' + band, mag)
    """

    def __init__(self, func, *params):
        """
        Constructor.
        """
        ParameterBase.__init__(self)
        self.func = func
        self.params = [p.id for p in params]
        dependent_parameter_dict[self.id] = self


def get_pos_parameters():
    """
    Convenience function for the position parameter X and Y.

    Returns
    -------
    x : FreeParameter
        X coordinate, starting at the X coordinate of the centroid and linearly limited to X +/- the object radius.
    y : FreeParameter
        Y coordinate, starting at the Y coordinate of the centroid and linearly limited to Y +/- the object radius.
    Notes
    -----
    X and Y are fitted on the detection image X and Y coordinates. Internally, these are translated to measurement
    images using the WCS headers.
    """
    param_range = Range(lambda v,o: (v-o.radius, v+o.radius), RangeType.LINEAR)
    return (
        FreeParameter(lambda o: o.centroid_x, param_range),
        FreeParameter(lambda o: o.centroid_y, param_range)
    )


class FluxParameterType():
    """
    Possible flux types to use as initial value for the flux parameter.
    Right now, only isophotal is supported.
    """
    ISO = 1


def get_flux_parameter(type=FluxParameterType.ISO, scale=1):
    """
    Convenience function for the flux parameter.

    Parameters
    ----------
    type : int
        One of the values defined in FluxParameterType
    scale : float
        Scaling of the initial flux. Defaults to 1.

    Returns
    -------
    flux : FreeParameter
        Flux parameter, starting at the flux defined by `type`, and limited to +/- 1e3 times the initial value.
    """
    attr_map = {
        FluxParameterType.ISO : 'isophotal_flux'
    }
    return FreeParameter(lambda o: getattr(o, attr_map[type]) * scale, Range(lambda v,o: (v * 1E-3, v * 1E3), RangeType.EXPONENTIAL))


prior_dict = {}


class Prior(cpp.Id):
    """
    Model a Gaussian prior on a given parameter.

    Parameters
    ----------
    param : ParameterBase
        Model fitting parameter
    value : float or callable that receives a source and returns a float
        Mean of the Gaussian
    sigma : float or callable that receives a source and returns a float
        Standard deviation of the Gaussian
    """

    def __init__(self, param, value, sigma):
        """
        Constructor.
        """
        cpp.Id.__init__(self)
        self.param = param.id
        self.value = value if hasattr(value, '__call__') else lambda o: value
        self.sigma = sigma if hasattr(sigma, '__call__') else lambda o: sigma


def add_prior(param, value, sigma):
    """
    Add a prior to the given parameter.

    Parameters
    ----------
    param : ParameterBase
    value : float or callable that receives a source and returns a float
        Mean of the Gaussian
    sigma : float or callable that receives a source and returns a float
        Standard deviation of the Gaussian
    """
    prior = Prior(param, value, sigma)
    prior_dict[prior.id] = prior


frame_models_dict = {}


def _set_model_to_frames(group, model):
    for x in group:
        if isinstance(x, tuple):
            _set_model_to_frames(x[1], model)
        else:
            if not x.id in frame_models_dict:
                frame_models_dict[x.id] = []
            frame_models_dict[x.id].append(model.id)


def add_model(group, model):
    """
    Add a model to be fitted to the given group.

    Parameters
    ----------
    group : MeasurementGroup
    model : ModelBase
    """
    if not isinstance(group, MeasurementGroup):
        raise TypeError('Models can only be added on MeasurementGroup, got {}'.format(type(group)))
    if not hasattr(group, 'models'):
        group.models = []
    group.models.append(model)
    _set_model_to_frames(group, model)


def print_model_fitting_info(group, show_params=False, prefix='', file=sys.stderr):
    """
    Print a human-readable representation of the configured models.

    Parameters
    ----------
    group : MeasurementGroup
        Print the models for this group.
    show_params : bool
        If True, print also the parameters that belong to the model
    prefix : str
        Prefix each line with this string. Used internally for indentation.
    file : file object
        Where to print the representation. Defaults to sys.stderr
    """
    if hasattr(group, 'models') and group.models:
        print('{}Models:'.format(prefix), file=file)
        for m in group.models:
            print('{}  {}'.format(prefix, m.to_string(show_params)), file=file)
    for x in group:
        if isinstance(x, tuple):
            print('{}{}:'.format(prefix, x[0]), file=file)
            print_model_fitting_info(x[1], show_params, prefix + '    ', file=file)


constant_model_dict = {}
point_source_model_dict = {}
sersic_model_dict = {}
exponential_model_dict = {}
de_vaucouleurs_model_dict = {}
params_dict = {"max_iterations": 100, "modified_chi_squared_scale": 10, "engine": ""}


def set_max_iterations(iterations):
    """
    Parameters
    ----------
    iterations : int
        Max number of iterations for the model fitting.
    """
    params_dict["max_iterations"] = iterations


def set_modified_chi_squared_scale(scale):
    """
    Parameters
    ----------
    scale : float
        Sets u0, as used by the modified chi squared residual comparator, a function that reduces the effect of large
        deviations.
        Refer to the SourceXtractor++ documentation for a better explanation of how residuals are computed and how
        this value affects the model fitting.
    """
    params_dict["modified_chi_squared_scale"] = scale


def set_engine(engine):
    """
    Parameters
    ----------
    engine : str
        Minimization engine for the model fitting : levmar or gsl
    """
    params_dict["engine"] = engine


class ModelBase(cpp.Id):
    """
    Base class for all models.
    """
    pass


class CoordinateModelBase(ModelBase):
    """
    Base class for positioned models with a flux. It can not be used directly.

    Parameters
    ----------
    x_coord : ParameterBase or float
        X coordinate (in the detection image)
    y_coord : ParameterBase or float
        Y coordinate (in the detection image)
    flux : ParameterBase or float
        Total flux
    """

    def __init__(self, x_coord, y_coord, flux):
        """
        Constructor.
        """
        ModelBase.__init__(self)
        self.x_coord = x_coord if isinstance(x_coord, ParameterBase) else ConstantParameter(x_coord)
        self.y_coord = y_coord if isinstance(y_coord, ParameterBase) else ConstantParameter(y_coord)
        self.flux = flux if isinstance(flux, ParameterBase) else ConstantParameter(flux)


class PointSourceModel(CoordinateModelBase):
    """
    Models a source as a point, spread by the PSF.

    Parameters
    ----------
    x_coord : ParameterBase or float
        X coordinate (in the detection image)
    y_coord : ParameterBase or float
        Y coordinate (in the detection image)
    flux : ParameterBase or float
        Total flux
    """

    def __init__(self, x_coord, y_coord, flux):
        """
        Constructor.
        """
        CoordinateModelBase.__init__(self, x_coord, y_coord, flux)
        global point_source_model_dict
        point_source_model_dict[self.id] = self

    def to_string(self, show_params=False):
        """
        Return a human readable representation of the model.

        Parameters
        ----------
        show_params: bool
            If True, include information about the parameters.

        Returns
        -------
        str
        """
        if show_params:
            return 'PointSource[x_coord={}, y_coord={}, flux={}]'.format(
                self.x_coord, self.y_coord, self.flux)
        else:
            return 'PointSource[x_coord={}, y_coord={}, flux={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id)


class ConstantModel(ModelBase):
    """
    A model that is constant through all the image.

    Parameters
    ----------
    value: ParameterBase or float
        Value to add to the value of all pixels from the model.
    """

    def __init__(self, value):
        """
        Constructor.
        """
        ModelBase.__init__(self)
        self.value = value if isinstance(value, ParameterBase) else ConstantParameter(value)
        global constant_model_dict
        constant_model_dict[self.id] = self

    def to_string(self, show_params=False):
        """
        Return a human readable representation of the model.

        Parameters
        ----------
        show_params: bool
            If True, include information about the parameters.

        Returns
        -------
        str
        """
        if show_params:
            return 'ConstantModel[value={}]'.format(self.value)
        else:
            return 'ConstantModel[value={}]'.format(self.value.id)


class SersicModelBase(CoordinateModelBase):
    """
    Base class for the Sersic, Exponential and de Vaucouleurs models. It can not be used directly.

    Parameters
    ----------
    x_coord : ParameterBase or float
        X coordinate (in the detection image)
    y_coord : ParameterBase or float
        Y coordinate (in the detection image)
    flux : ParameterBase or float
        Total flux
    effective_radius : ParameterBase or float
        Ellipse semi-major axis, in pixels on the detection image.
    aspect_ratio : ParameterBase or float
        Ellipse ratio.
    angle : ParameterBase or float
        Ellipse rotation, in radians
    """

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle):
        """
        Constructor.
        """
        CoordinateModelBase.__init__(self, x_coord, y_coord, flux)
        self.effective_radius = effective_radius if isinstance(effective_radius, ParameterBase) else ConstantParameter(effective_radius)
        self.aspect_ratio = aspect_ratio if isinstance(aspect_ratio, ParameterBase) else ConstantParameter(aspect_ratio)
        self.angle = angle if isinstance(angle, ParameterBase) else ConstantParameter(angle)


class SersicModel(SersicModelBase):
    """
    Model a source with a Sersic profile.

    Parameters
    ----------
    x_coord : ParameterBase or float
        X coordinate (in the detection image)
    y_coord : ParameterBase or float
        Y coordinate (in the detection image)
    flux : ParameterBase or float
        Total flux
    effective_radius : ParameterBase or float
        Ellipse semi-major axis, in pixels on the detection image.
    aspect_ratio : ParameterBase or float
        Ellipse ratio.
    angle : ParameterBase or float
        Ellipse rotation, in radians
    n : ParameterBase or float
        Sersic index
    """

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle, n):
        """
        Constructor.
        """
        SersicModelBase.__init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle)
        self.n = n if isinstance(n, ParameterBase) else ConstantParameter(n)
        global sersic_model_dict
        sersic_model_dict[self.id] = self

    def to_string(self, show_params=False):
        """
        Return a human readable representation of the model.

        Parameters
        ----------
        show_params: bool
            If True, include information about the parameters.

        Returns
        -------
        str
        """
        if show_params:
            return 'Sersic[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}, n={}]'.format(
                self.x_coord, self.y_coord, self.flux, self.effective_radius, self.aspect_ratio, self.angle, self.n)
        else:
            return 'Sersic[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}, n={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id, self.effective_radius.id, self.aspect_ratio.id, self.angle.id, self.n.id)


class ExponentialModel(SersicModelBase):
    """
    Model a source with an exponential profile (Sersic model with an index of 1)

    Parameters
    ----------
    x_coord : ParameterBase or float
        X coordinate (in the detection image)
    y_coord : ParameterBase or float
        Y coordinate (in the detection image)
    flux : ParameterBase or float
        Total flux
    effective_radius : ParameterBase or float
        Ellipse semi-major axis, in pixels on the detection image.
    aspect_ratio : ParameterBase or float
        Ellipse ratio.
    angle : ParameterBase or float
        Ellipse rotation, in radians
    """

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle):
        """
        Constructor.
        """
        SersicModelBase.__init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle)
        global exponential_model_dict
        exponential_model_dict[self.id] = self

    def to_string(self, show_params=False):
        """
        Return a human readable representation of the model.

        Parameters
        ----------
        show_params: bool
            If True, include information about the parameters.

        Returns
        -------
        str
        """
        if show_params:
            return 'Exponential[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord, self.y_coord, self.flux, self.effective_radius, self.aspect_ratio, self.angle)
        else:
            return 'Exponential[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id, self.effective_radius.id, self.aspect_ratio.id, self.angle.id)


class DeVaucouleursModel(SersicModelBase):
    """
    Model a source with a De Vaucouleurs profile (Sersic model with an index of 4)

    Parameters
    ----------
    x_coord : ParameterBase or float
        X coordinate (in the detection image)
    y_coord : ParameterBase or float
        Y coordinate (in the detection image)
    flux : ParameterBase or float
        Total flux
    effective_radius : ParameterBase or float
        Ellipse semi-major axis, in pixels on the detection image.
    aspect_ratio : ParameterBase or float
        Ellipse ratio.
    angle : ParameterBase or float
        Ellipse rotation, in radians
    """

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle):
        """
        Constructor.
        """
        SersicModelBase.__init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle)
        global de_vaucouleurs_model_dict
        de_vaucouleurs_model_dict[self.id] = self

    def to_string(self, show_params=False):
        """
        Return a human readable representation of the model.

        Parameters
        ----------
        show_params: bool
            If True, include information about the parameters.

        Returns
        -------
        str
        """
        if show_params:
            return 'DeVaucouleurs[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord, self.y_coord, self.flux, self.effective_radius, self.aspect_ratio, self.angle)
        else:
            return 'DeVaucouleurs[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id, self.effective_radius.id, self.aspect_ratio.id, self.angle.id)


class WorldCoordinate:
    """
    Coordinates in right ascension and declination

    Parameters
    ----------
    ra : float
        Right ascension
    dec : float
        Declination
    """

    def __init__(self, ra, dec):
        """
        Constructor.
        """
        self.ra = ra
        self.dec = dec


def pixel_to_world_coordinate(x, y):
    """
    Transform an (X, Y) in pixel coordinates on the detection image to (RA, DEC) in world coordinates.
    Parameters
    ----------
    x : float
    y : float

    Returns
    -------
    WorldCoordinate
    """
    global coordinate_system
    wc = coordinate_system.image_to_world(cpp.ImageCoordinate(x-1, y-1)) # -1 as we go FITS -> internal 
    return WorldCoordinate(wc.alpha, wc.delta)


def get_sky_coord(x, y):
    """
    Transform an (X, Y) in pixel coordinates on the detection image to astropy SkyCoord.

    Parameters
    ----------
    x : float
    y : float

    Returns
    -------
    SkyCoord
    """
    coord = pixel_to_world_coordinate(x, y)
    sky_coord = SkyCoord(ra=coord.ra*u.degree, dec=coord.dec*u.degree)
    return sky_coord


def radius_to_wc_angle(x, y, rad):
    """
    Transform a radius in pixels on the detection image to a radius in sky coordinates.

    Parameters
    ----------
    x : float
    y : float
    rad : float

    Returns
    -------
    Radius in degrees
    """
    return (get_separation_angle(x, y, x+rad, y) + get_separation_angle(x, y, x, y+rad)) / 2.0


def get_separation_angle(x1, y1, x2, y2):
    """
    Get the separation angle in sky coordinates for two points defined in pixels on the detection image.

    Parameters
    ----------
    x1 : float
    y1 : float
    x2 : float
    y2 : float

    Returns
    -------
    Separation in degrees
    """
    c1 = get_sky_coord(x1, y1)
    c2 = get_sky_coord(x2, y2)
    return c1.separation(c2).degree


def get_position_angle(x1, y1, x2, y2):
    """
    Get the position angle in sky coordinates for two points defined in pixels on the detection image.

    Parameters
    ----------
    x1
    y1
    x2
    y2

    Returns
    -------
    Position angle in degrees, normalized to -/+ 90
    """
    c1 = get_sky_coord(x1, y1)
    c2 = get_sky_coord(x2, y2)
    angle = c1.position_angle(c2).degree
    
    # return angle normalized to range: -90 <= angle < 90
    return (angle + 90.0) % 180.0 - 90.0


def set_coordinate_system(cs):
    """
    Set the global coordinate system. This function is used internally by SourceXtractor++.
    """
    global coordinate_system
    coordinate_system = cs


def get_world_position_parameters(x, y):
    """
    Convenience function for generating two dependent parameter with world (alpha, delta) coordinates
    from image (X, Y) coordinates.

    Parameters
    ----------
    x : ParameterBase
    y : ParameterBase

    Returns
    -------
    ra : DependentParameter
    dec : DependentParameter

    See Also
    --------
    get_pos_parameters

    Examples
    --------
    >>> x, y = get_pos_parameters()
    >>> ra, dec = get_world_position_parameters(x, y)
    >>> add_output_column('mf_ra', ra)
    >>> add_output_column('mf_dec', dec)
    """
    ra = DependentParameter(lambda x,y: pixel_to_world_coordinate(x, y).ra, x, y)
    dec = DependentParameter(lambda x,y: pixel_to_world_coordinate(x, y).dec, x, y)
    return (ra, dec)


def get_world_parameters(x, y, radius, angle, ratio):
    """
    Convenience function for generating five dependent parameters, in world coordinates, for the position
    and shape of a model.

    Parameters
    ----------
    x : ParameterBase
    y : ParameterBase
    radius : ParameterBase
    angle : ParameterBase
    ratio : ParameterBase

    Returns
    -------
    ra : DependentParameter
        Right ascension
    dec : DependentParameter
        Declination
    rad : DependentParameter
        Radius as degrees
    angle : DependentParameter
        Angle in degrees
    ratio : DependentParameter
        Aspect ratio. It has to be recomputed as the axis of the ellipse may have different ratios
        in image coordinates than in world coordinates

    Examples
    --------
    >>> flux = get_flux_parameter()
    >>> x, y = get_pos_parameters()
    >>> radius = FreeParameter(lambda o: o.radius, Range(lambda v, o: (.01 * v, 100 * v), RangeType.EXPONENTIAL))
    >>> angle = FreeParameter(lambda o: o.angle, Range((-np.pi, np.pi), RangeType.LINEAR))
    >>> ratio = FreeParameter(1, Range((0, 10), RangeType.LINEAR))
    >>> add_model(group, ExponentialModel(x, y, flux, radius, ratio, angle))
    >>> ra, dec, wc_rad, wc_angle, wc_ratio = get_world_parameters(x, y, radius, angle, ratio)
    >>> add_output_column('mf_world_angle', wc_angle)
    """
    ra = DependentParameter(lambda x,y: pixel_to_world_coordinate(x, y).ra, x, y)
    dec = DependentParameter(lambda x,y: pixel_to_world_coordinate(x, y).dec, x, y)
    
    def get_major_axis(x, y, radius, angle, ratio):
        if ratio <= 1:
            x2 = x + math.cos(angle) * radius
            y2 = y + math.sin(angle) * radius
        else:
            x2 = x + math.sin(angle) * radius * ratio
            y2 = y + math.cos(angle) * radius * ratio
            
        return x2, y2
        
    def get_minor_axis(x, y, radius, angle, ratio):
        if ratio <= 1:
            x2 = x + math.sin(angle) * radius * ratio
            y2 = y + math.cos(angle) * radius * ratio
        else:
            x2 = x + math.cos(angle) * radius
            y2 = y + math.sin(angle) * radius
            
        return x2, y2
        
    def wc_rad_func(x, y, radius, angle, ratio):
        x2, y2 = get_major_axis(x, y, radius, angle, ratio)
        return get_separation_angle(x, y, x2, y2)
        
    def wc_angle_func(x, y, radius, angle, ratio):
        x2, y2 = get_major_axis(x, y, radius, angle, ratio)
        return get_position_angle(x, y, x2, y2)
    
    def wc_ratio_func(x, y, radius, angle, ratio):
        minor_x, minor_y = get_minor_axis(x, y, radius, angle, ratio)
        minor_angle = get_separation_angle(x,y, minor_x, minor_y) 
        
        major_x, major_y = get_major_axis(x, y, radius, angle, ratio)
        major_angle = get_separation_angle(x,y, major_x, major_y)
        
        return minor_angle / major_angle
    
    wc_rad = DependentParameter(wc_rad_func, x, y, radius, angle, ratio)
    wc_angle = DependentParameter(wc_angle_func, x, y, radius, angle, ratio)
    wc_ratio = DependentParameter(wc_ratio_func, x, y, radius, angle, ratio)
    
    return ra, dec, wc_rad, wc_angle, wc_ratio

