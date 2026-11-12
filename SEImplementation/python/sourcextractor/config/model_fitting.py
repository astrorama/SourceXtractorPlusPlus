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
from __future__ import annotations

import math
import sys
import warnings
from collections.abc import Mapping
from dataclasses import dataclass
from enum import Enum, IntEnum
from typing import (
    TYPE_CHECKING, Any, Callable, IO, Literal, NamedTuple, Protocol, TypeAlias, overload)

import _SourceXtractorPy as cpp

try:
    import pyston
except ImportError:
    warnings.warn('Could not import pyston: running outside sourcextractor?', ImportWarning)
from astropy import units as u
from astropy.coordinates import SkyCoord

from .measurement_images import MeasurementGroup


class SourceInfo(Protocol):
    """
    Seed properties of detected source, passed to model-fitting callables

    Mirrors the attributes of the C++ ObjectInfo (a Pyston AttributeSet) and
    must be kept in sync with its attributes.
    """

    centroid_x: float
    centroid_y: float
    isophotal_flux: float
    radius: float
    angle: float
    aspect_ratio: float
    assoc_match: bool
    assoc_size: float

    def __getattr__(self, name: str) -> float:
        """Covers config-specific assoc_value_<i> and assoc_<column_name>"""


class RangeType(Enum):
    LINEAR = 1
    EXPONENTIAL = 2


Limits: TypeAlias = tuple[float, float] | Callable[[float, SourceInfo], tuple[float, float]]


class Range:
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

    def __init__(self, limits: Limits, type: RangeType) -> None:
        """
        Constructor.
        """
        self.__limits = limits
        self.__callable = limits if callable(limits) else lambda v, o: limits
        self.__type = type

    def get_min(self, v: float, o: SourceInfo) -> float:
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

    def get_max(self, v: float, o: SourceInfo) -> float:
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

    def get_type(self) -> RangeType:
        """
        Returns
        -------
        RangeType
        """
        return self.__type

    def __str__(self) -> str:
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        res = '['
        if callable(self.__limits):
            res += 'func'
        else:
            res += f'{self.__limits[0]},{self.__limits[1]}'
        type_str = {
            RangeType.LINEAR: 'LIN',
            RangeType.EXPONENTIAL: 'EXP'
        }
        res += f',{type_str[self.__type]}]'
        return res


class Unbounded:
    """
    Unbounded, but normalize, value of a model fitting parameter

    Parameters
    ----------
    normalization_factor: float, or a callable that receives the initial value parameter value and a source,
        and returns a float
        The world value which will be normalized to 1 in engine coordinates
    """

    def __init__(self, normalization_factor: FloatOrFloatSourceCb = 1.) -> None:
        """
        Constructor.
        """
        self.__normalization_factor = normalization_factor
        if callable(normalization_factor):
            self.__callable = normalization_factor
        else:
            self.__callable = lambda v, o: normalization_factor

    def get_normalization_factor(self, v: float, o: SourceInfo) -> float:
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

    def __str__(self) -> str:
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        if callable(self.__normalization_factor):
            res = 'func'
        else:
            res = str(self.__normalization_factor)
        return f'[{res}]'


class ParameterBase(cpp.Id):
    """
    Base class for all model fitting parameter types.
    Can not be used directly.
    """

    def __str__(self) -> str:
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        return f'(ID:{self.id})'


class ConstantParameter(ParameterBase):
    """
    A parameter with a single value that remains constant. It will not be fitted.

    Parameters
    ----------
    value : float, or callable that receives a source and returns a float
        Value for this parameter
    """

    def __init__(self, value: FloatOrSourceCb) -> None:
        """
        Constructor.
        """
        super().__init__()
        self.__value = value
        self.__callable = value if callable(value) else lambda o: value

    def get_value(self, o: SourceInfo) -> float:
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

    def __str__(self) -> str:
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        pre = super().__str__()[:-1]
        if callable(self.__value):
            val = 'func'
        else:
            val = str(self.__value)
        return f'{pre}, value:{val})'


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

    def __init__(self, init_value: FloatOrSourceCb, range: Range | Unbounded = Unbounded()) -> None:
        """
        Constructor.
        """
        super().__init__()
        self.__init_value = init_value
        self.__init_call = init_value if callable(init_value) else lambda o: init_value
        self.__range = range

    def get_init_value(self, o: SourceInfo) -> float:
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

    def get_range(self) -> Unbounded | Range:
        """
        Returns
        -------
        Unbounded or Range
        """
        return self.__range

    def __str__(self) -> str:
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        pre = super().__str__()[:-1] + ', init:'
        if callable(self.__init_value):
            init = 'func'
        else:
            init = str(self.__init_value)
        if self.__range:
            rng = f', range:{self.__range}'
        else:
            rng = ''

        return f'{pre}, init: {init}{rng})'


# Type aliass to make the below DependentParamenter.__init__ overloads terser
_F: TypeAlias = float
_P: TypeAlias = 'FittableParameter'


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

    # NOTE: The interface of this constructor is not possible to express exactly with current
    # static typing features, as the signature of the callback function depends on the number
    # of positional parameters given--this is not even possible with a TypeVarTuple.  Would
    # require a plugin, or refactoring.  IMO this is not a bad interface though.
    # The overloads here cover the cases of a few different parameters (up to 5 for now).

    @overload
    def __init__(self, func: Callable[[_F], _F], p0: _P, /) -> None: ...
    @overload
    def __init__(self, func: Callable[[_F, _F], _F], p0: _P, p1: _P, /) -> None: ...
    @overload
    def __init__(self, func: Callable[[_F, _F, _F], _F], p0: _P, p1: _P, p2: _P, /) -> None: ...
    @overload
    def __init__(self, func: Callable[[_F, _F, _F, _F], _F], p0: _P, p1: _P, p2: _P, p3: _P, /) -> None: ...
    @overload
    def __init__(self, func: Callable[[_F, _F, _F, _F, _F], _F], p0: _P, p1: _P, p2: _P, p3: _P, p4: _P, /) -> None: ...
    @overload
    def __init__(self, func: Callable[..., float], *params: FittableParameter) -> None: ...
    def __init__(self, func: Any, *params: Any) -> None:  # noqa: E301
        """
        Constructor.
        """
        super().__init__()
        self.func = func
        self.params = list(params)


def get_pos_parameters() -> tuple[FreeParameter, FreeParameter]:
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
    param_range = Range(lambda v, o: (v - o.radius, v + o.radius), RangeType.LINEAR)
    return (
        FreeParameter(lambda o: o.centroid_x, param_range),
        FreeParameter(lambda o: o.centroid_y, param_range)
    )


class FluxParameterType(IntEnum):
    """
    Possible flux types to use as initial value for the flux parameter.
    Right now, only isophotal is supported.
    """
    ISO = 1


def get_flux_parameter(type: FluxParameterType = FluxParameterType.ISO,
                       scale: float = 1.) -> FreeParameter:
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
        FluxParameterType.ISO: 'isophotal_flux'
    }
    return FreeParameter(lambda o: getattr(o, attr_map[type]) * scale,
                         Range(lambda v, o: (v * 1E-3, v * 1E3), RangeType.EXPONENTIAL))


class ModelBase(cpp.Id):
    """
    Base class for all models.
    """

    def get_params(self) -> list[FittableParameter]:
        return []


FloatOrFloatCb: TypeAlias = float | Callable[[float], float]
FloatOrSourceCb: TypeAlias = float | Callable[[SourceInfo], float]
FloatOrFloatSourceCb: TypeAlias = float | Callable[[float, SourceInfo], float]

if TYPE_CHECKING:
    # This one can't be assigned yet at runtime so has to be guarded
    FloatOrParam: TypeAlias = float | FittableParameter
else:
    FloatOrParam = Any


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

    def __init__(self, param: ParameterBase, value: FloatOrSourceCb, sigma: FloatOrSourceCb) -> None:
        """
        Constructor.
        """
        super().__init__()
        self.param = param.id
        self.value = value if callable(value) else lambda o: value
        self.sigma = sigma if callable(sigma) else lambda o: sigma


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

    def __init__(self, x_coord: float | FittableParameter, y_coord: float | FittableParameter,
                 flux: float | FittableParameter) -> None:
        """
        Constructor.
        """
        super().__init__()
        self.x_coord: FittableParameter = x_coord if isinstance(x_coord, ParameterBase) else ConstantParameter(x_coord)
        self.y_coord: FittableParameter = y_coord if isinstance(y_coord, ParameterBase) else ConstantParameter(y_coord)
        self.flux: FittableParameter = flux if isinstance(flux, ParameterBase) else ConstantParameter(flux)

    def get_params(self) -> list[FittableParameter]:
        return [self.x_coord, self.y_coord, self.flux]



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

    def __init__(self, x_coord: FloatOrParam, y_coord: FloatOrParam, flux: FloatOrParam) -> None:
        """
        Constructor.
        """
        super().__init__(x_coord, y_coord, flux)

    def to_string(self, show_params: bool = False) -> str:
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
            return f'PointSource[x_coord={self.x_coord}, y_coord={self.y_coord}, flux={self.flux}]'
        else:
            return f'PointSource[x_coord={self.x_coord.id}, y_coord={self.y_coord.id}, flux={self.flux.id}]'


class ConstantModel(ModelBase):
    """
    A model that is constant through all the image.

    Parameters
    ----------
    value: ParameterBase or float
        Value to add to the value of all pixels from the model.
    """

    def __init__(self, value: FloatOrParam) -> None:
        """
        Constructor.
        """
        super().__init__()
        self.value = value if isinstance(value, ParameterBase) else ConstantParameter(value)

    def to_string(self, show_params: bool = False) -> str:
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
            return f'ConstantModel[value={self.value}]'
        else:
            return f'ConstantModel[value={self.value.id}]'

        def get_params(self):
            return [self.value]


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

    def __init__(self, x_coord: float | FittableParameter, y_coord: float | FittableParameter,
                 flux: float | FittableParameter, effective_radius: float | FittableParameter,
                 aspect_ratio: float | FittableParameter, angle: float | FittableParameter):
        """
        Constructor.
        """
        super().__init__(x_coord, y_coord, flux)
        self.effective_radius = effective_radius if isinstance(effective_radius,
                                                               ParameterBase) else ConstantParameter(
            effective_radius)
        self.aspect_ratio = aspect_ratio if isinstance(aspect_ratio,
                                                       ParameterBase) else ConstantParameter(
            aspect_ratio)
        self.angle = angle if isinstance(angle, ParameterBase) else ConstantParameter(angle)

    def get_params(self) -> list[FittableParameter]:
        return super().get_params() + [self.effective_radius, self.aspect_ratio, self.angle]


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

    def __init__(self, x_coord: FloatOrParam, y_coord: FloatOrParam, flux: FloatOrParam,
                 effective_radius: FloatOrParam, aspect_ratio: FloatOrParam, angle: FloatOrParam,
                 n: FloatOrParam) -> None:
        """
        Constructor.
        """
        super().__init__(x_coord, y_coord, flux, effective_radius, aspect_ratio, angle)
        self.n = n if isinstance(n, ParameterBase) else ConstantParameter(n)

    def to_string(self, show_params: bool = False) -> str:
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
        # TODO: These reprs could be generalized so as to avoid writing these
        # strings out by hand in every subclass
        if show_params:
            return (f'Sersic[x_coord={self.x_coord}, y_coord={self.y_coord}, flux={self.flux}, '
                    f'effective_radius={self.effective_radius}, aspect_ratio={self.aspect_ratio}, '
                    f'angle={self.angle}, n={self.n}]')
        else:
            return (f'Sersic[x_coord={self.x_coord.id}, y_coord={self.y_coord.id}, flux={self.flux.id}, '
                    f'effective_radius={self.effective_radius.id}, aspect_ratio={self.aspect_ratio.id}, '
                    f'angle={self.angle.id}, n={self.n.id}]')

    def get_params(self) -> list[FittableParameter]:
        return super().get_params() + [self.n]


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

    def to_string(self, show_params: bool = False) -> str:
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
            return (f'Exponential[x_coord={self.x_coord}, y_coord={self.y_coord}, flux={self.flux}, '
                    f'effective_radius={self.effective_radius}, aspect_ratio={self.aspect_ratio}, '
                    f'angle={self.angle}]')
        else:
            return (f'Exponential[x_coord={self.x_coord.id}, y_coord={self.y_coord.id}, flux={self.flux.id}, '
                    f'effective_radius={self.effective_radius.id}, aspect_ratio={self.aspect_ratio.id}, '
                    f'angle={self.angle.id}]')


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

    def to_string(self, show_params: bool = False) -> str:
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
            return (f'DeVaucouleurs[x_coord={self.x_coord}, y_coord={self.y_coord}, flux={self.flux}, '
                    f'effective_radius={self.effective_radius}, aspect_ratio={self.aspect_ratio}, '
                    f'angle={self.angle}]')
        else:
            return (f'DeVaucouleurs[x_coord={self.x_coord.id}, y_coord={self.y_coord.id}, flux={self.flux.id}, '
                    f'effective_radius={self.effective_radius.id}, aspect_ratio={self.aspect_ratio.id}, '
                    f'angle={self.angle.id}]')


class ComputeGraphModel(CoordinateModelBase):
    """
    ComputeGraphModel model

    Parameters
    ----------
    models: string or list of strings, corresponding to path to Onnx format models,
     (specifying more than one allows using the most efficient model based on render size.)
    x_coord : ParameterBase or float
        X coordinate (in the detection image)
    y_coord : ParameterBase or float
        Y coordinate (in the detection image)
    flux : ParameterBase or float
        Total flux
    params : Dictionary of String and ParameterBase or float
        Dictionary of custom parameters for the ONNX model
    """

    def __init__(self, models: str | list[str], x_coord: FloatOrParam, y_coord: FloatOrParam,
                 flux: FloatOrParam, params: dict[str, FloatOrParam] = {}):
        """
        Constructor.
        """
        super().__init__(x_coord, y_coord, flux)

        ratio_name = "_aspect_ratio"
        angle_name = "_angle"
        scale_name = "_scale"

        params_new: dict[str, FittableParameter] = dict()  # Don't reuse mutable argument default

        for key, param in params.items():
            if isinstance(param, float):
                params_new[key] = ConstantParameter(param)
            else:
                params_new[key] = param

        del params  # To ensure it's not reused by this function after here

        aspect_ratio = params_new[ratio_name] if ratio_name in params_new else 1.0
        angle = params_new[angle_name] if angle_name in params_new else 0.0
        scale = params_new[scale_name] if scale_name in params_new else 1.0

        self.aspect_ratio = aspect_ratio if isinstance(aspect_ratio,
                                                       ParameterBase) else ConstantParameter(
            aspect_ratio)
        self.angle = angle if isinstance(angle, ParameterBase) else ConstantParameter(angle)
        self.scale = scale if isinstance(scale, ParameterBase) else ConstantParameter(scale)

        params_new.pop(ratio_name, None)
        params_new.pop(angle_name, None)
        params_new.pop(scale_name, None)

        self.params = params_new
        self.models = models if isinstance(models, list) else [models]

    def to_string(self, show_params: bool = False) -> str:
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
        # NOTE: This code used to list an `effective_radius` as well, but this is not
        # set in the constructor unlike the models based on SersicModelBase; as such
        # this code was previously broken.  Rather than guess as to whether or not that
        # should be a supported parameter (I don't know) leave it out for now. Do make
        # a note of this when submitting a PR...
        if show_params:
            return (f'ComputeGraph[x_coord={self.x_coord}, y_coord={self.y_coord}, flux={self.flux}, '
                    f'aspect_ratio={self.aspect_ratio}, angle={self.angle}]')
        else:
            return (f'ComputeGraph[x_coord={self.x_coord.id}, y_coord={self.y_coord.id}, flux={self.flux.id}, '
                    f'aspect_ratio={self.aspect_ratio.id}, angle={self.angle.id}]')

    def get_params(self) -> list[FittableParameter]:
        return (super().get_params() + [self.scale, self.aspect_ratio, self.angle] +
            list(self.params.values()))


@dataclass
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

    ra: float
    dec: float


def pixel_to_world_coordinate(x: float, y: float) -> WorldCoordinate:
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
    # -1 as we go FITS -> internal
    wc_alpha = pyston.image_to_world_alpha(x - 1, y - 1)
    wc_delta = pyston.image_to_world_delta(x - 1, y - 1)
    return WorldCoordinate(wc_alpha, wc_delta)


def get_sky_coord(x: float, y: float) -> SkyCoord:
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
    sky_coord = SkyCoord(ra=coord.ra * u.degree, dec=coord.dec * u.degree)
    return sky_coord


def radius_to_wc_angle(x: float, y: float, rad: float) -> float:
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
    return (get_separation_angle(x, y, x + rad, y) + get_separation_angle(x, y, x, y + rad)) / 2.0


def get_separation_angle(x1: float, y1: float, x2: float, y2: float) -> float:
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
    # in case astropy does not provide type information, wrap the type
    # here just in case, but it should already be float
    return float(c1.separation(c2).degree)


def get_position_angle(x1: float, y1: float, x2: float, y2: float) -> float:
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
    # in case astropy does not provide type information, wrap the type
    # here just in case, but it should already be float
    angle = float(c1.position_angle(c2).degree)

    # return angle normalized to range: -90 <= angle < 90
    return (angle + 90.0) % 180.0 - 90.0


class WorldPositionParameters(NamedTuple):
    ra: DependentParameter
    dec: DependentParameter


def get_world_position_parameters(x: FittableParameter, y: FittableParameter) -> WorldPositionParameters:
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
    ra = DependentParameter(lambda x, y: pixel_to_world_coordinate(x, y).ra, x, y)
    dec = DependentParameter(lambda x, y: pixel_to_world_coordinate(x, y).dec, x, y)
    return WorldPositionParameters(ra, dec)


class WorldParameters(NamedTuple):
    ra: DependentParameter
    dec: DependentParameter
    rad: DependentParameter
    angle: DependentParameter
    ratio: DependentParameter


def get_world_parameters(x: FittableParameter, y: FittableParameter, radius: FittableParameter,
                         angle: FittableParameter, ratio: FittableParameter) -> WorldParameters:
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
    ra = DependentParameter(lambda x, y: pixel_to_world_coordinate(x, y).ra, x, y)
    dec = DependentParameter(lambda x, y: pixel_to_world_coordinate(x, y).dec, x, y)

    def get_major_axis(x: float, y: float, radius: float, angle: float, ratio: float) -> tuple[float, float]:
        if ratio <= 1:
            x2 = x + math.cos(angle) * radius
            y2 = y + math.sin(angle) * radius
        else:
            x2 = x + math.sin(angle) * radius * ratio
            y2 = y + math.cos(angle) * radius * ratio

        return x2, y2

    def get_minor_axis(x: float, y: float, radius: float, angle: float, ratio: float) -> tuple[float, float]:
        if ratio <= 1:
            x2 = x + math.sin(angle) * radius * ratio
            y2 = y + math.cos(angle) * radius * ratio
        else:
            x2 = x + math.cos(angle) * radius
            y2 = y + math.sin(angle) * radius

        return x2, y2

    def wc_rad_func(x: float, y: float, radius: float, angle: float, ratio: float) -> float:
        x2, y2 = get_major_axis(x, y, radius, angle, ratio)
        return get_separation_angle(x, y, x2, y2)

    def wc_angle_func(x: float, y: float, radius: float, angle: float, ratio: float) -> float:
        x2, y2 = get_major_axis(x, y, radius, angle, ratio)
        return get_position_angle(x, y, x2, y2)

    def wc_ratio_func(x: float, y: float, radius: float, angle: float, ratio: float) -> float:
        minor_x, minor_y = get_minor_axis(x, y, radius, angle, ratio)
        minor_angle = get_separation_angle(x, y, minor_x, minor_y)

        major_x, major_y = get_major_axis(x, y, radius, angle, ratio)
        major_angle = get_separation_angle(x, y, major_x, major_y)

        return minor_angle / major_angle

    wc_rad = DependentParameter(wc_rad_func, x, y, radius, angle, ratio)
    wc_angle = DependentParameter(wc_angle_func, x, y, radius, angle, ratio)
    wc_ratio = DependentParameter(wc_ratio_func, x, y, radius, angle, ratio)

    return WorldParameters(ra, dec, wc_rad, wc_angle, wc_ratio)


class WindowType(Enum):
    RECTANGLE = 1
    SQUARE_MIN = 2
    SQUARE_MAX = 3
    SQUARE_AREA = 4
    DISK_MIN = 5
    DISK_MAX = 6
    DISK_AREA = 7
    ALIGNED_ELLIPSE = 8
    ROTATED_ELLIPSE = 9


FittableParameter: TypeAlias = ConstantParameter | DependentParameter | FreeParameter
"""Parameter types that can be added to a model fitter"""


FittableModel: TypeAlias = (
    ConstantModel | PointSourceModel | SersicModel | ExponentialModel | DeVaucouleursModel |
    ComputeGraphModel
)
"""Concrete model types that can be fitted"""


Engine = Literal['gsl', 'levmar']
"""Allowed engine types; currently the above two"""


class ModelFitting:
    def __init__(self) -> None:
        self.constant_parameter_dict: dict[int, ConstantParameter] = {}
        self.free_parameter_dict: dict[int, FreeParameter] = {}
        self.dependent_parameter_dict: dict[int, DependentParameter] = {}
        self.frame_models_dict: dict[int, list[int]] = {}
        self.prior_dict: dict[int, Prior] = {}
        self.constant_model_dict: dict[int, ConstantModel] = {}
        self.point_source_model_dict: dict[int, PointSourceModel] = {}
        self.sersic_model_dict: dict[int, SersicModel] = {}
        self.exponential_model_dict: dict[int, ExponentialModel] = {}
        self.de_vaucouleurs_model_dict: dict[int, DeVaucouleursModel] = {}
        self.onnx_model_dict: dict[int, ComputeGraphModel] = {}
        self.params_dict = {"max_iterations": 200, "modified_chi_squared_scale": 10, "engine": "",
                            "use_iterative_fitting": True, "meta_iterations": 5,
                            "deblend_factor": 0.95, "meta_iteration_stop": 0.0001,
                            "window_type": WindowType.RECTANGLE, "ellipse_scale": 3.0,
                            "show_fitting_areas": False
                            }

    def _set_model_to_frames(self, group: MeasurementGroup, model: FittableModel) -> None:
        for x in group:
            if isinstance(x, tuple):
                self._set_model_to_frames(x[1], model)
            else:
                if x.id not in self.frame_models_dict:
                    self.frame_models_dict[x.id] = []
                self.frame_models_dict[x.id].append(model.id)

    def _is_param_known(self, param: FittableParameter) -> bool:
        return param.id in self.constant_parameter_dict or \
               param.id in self.free_parameter_dict or \
               param.id in self.dependent_parameter_dict

    def _register_parameter(self, attr: FittableParameter) -> None:
        if isinstance(attr, ConstantParameter):
            self.constant_parameter_dict[attr.id] = attr
        elif isinstance(attr, FreeParameter):
            self.free_parameter_dict[attr.id] = attr
        elif isinstance(attr, DependentParameter):
            self.dependent_parameter_dict[attr.id] = attr
            for param in attr.params:
                self._register_parameter(param)

    def _populate_parameters(self, model: FittableModel) -> None:
        for param in model.get_params():
            self._register_parameter(param)

    def _register_model(self, model: FittableModel) -> None:
        if isinstance(model, ConstantModel):
            self.constant_model_dict[model.id] = model
        elif isinstance(model, PointSourceModel):
            self.point_source_model_dict[model.id] = model
        elif isinstance(model, SersicModel):
            self.sersic_model_dict[model.id] = model
        elif isinstance(model, ExponentialModel):
            self.exponential_model_dict[model.id] = model
        elif isinstance(model, DeVaucouleursModel):
            self.de_vaucouleurs_model_dict[model.id] = model
        elif isinstance(model, ComputeGraphModel):
            self.onnx_model_dict[model.id] = model
        else:
            raise TypeError(f'Unknown model type {type(model)}')

    def add_model(self, group: MeasurementGroup, model: FittableModel) -> None:
        """
        Add a model to be fitted to the given group.

        Parameters
        ----------
        group : MeasurementGroup
        model : ModelBase
        """
        if not isinstance(group, MeasurementGroup):
            raise TypeError(
                f'Models can only be added on MeasurementGroup, got {type(group)}')
        group.models.append(model)
        self._set_model_to_frames(group, model)
        self._populate_parameters(model)
        self._register_model(model)

    def add_prior(self, param: FittableParameter, value: FloatOrSourceCb, sigma: FloatOrSourceCb) -> None:
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
        self.prior_dict[prior.id] = prior
        if not self._is_param_known(param):
            self._register_parameter(param)

    def print_parameters(self, file: IO[str] | None = None) -> None:
        """
        Print a human-readable representation of the configured model fitting parameters.

        Parameters
        ----------
        file : file object
            Where to print the representation. Defaults to sys.stderr
        """
        file = file or sys.stderr

        def list_params(type_name: str, params: Mapping[int, ParameterBase]) -> None:
            print(type_name + ':', file=file)
            for n, param in params.items():
                print(f'  {n}: {param}', file=file)

        list_params('Constant parameters', self.constant_parameter_dict)
        list_params('Free parameters', self.free_parameter_dict)
        list_params('Dependent parameters', self.dependent_parameter_dict)

    def set_max_iterations(self, iterations: int) -> None:
        """
        Parameters
        ----------
        iterations : int
            Max number of iterations for the model fitting.
        """
        self.params_dict["max_iterations"] = iterations

    def set_modified_chi_squared_scale(self, scale: float) -> None:
        """
        Parameters
        ----------
        scale : float
            Sets u0, as used by the modified chi squared residual comparator, a function that reduces the effect of large
            deviations.
            Refer to the SourceXtractor++ documentation for a better explanation of how residuals are computed and how
            this value affects the model fitting.
        """
        self.params_dict["modified_chi_squared_scale"] = scale

    def set_engine(self, engine: Engine) -> None:
        """
        Parameters
        ----------
        engine : str
            Minimization engine for the model fitting : levmar or gsl
        """
        self.params_dict["engine"] = engine

    def use_iterative_fitting(self, use_iterative_fitting: bool) -> None:
        """
        Parameters
        ----------
        use_iterative_fitting : boolean
            use iterative model fitting or legacy
        """
        self.params_dict["use_iterative_fitting"] = use_iterative_fitting

    def set_meta_iterations(self, meta_iterations: int) -> None:
        """
        Parameters
        ----------
        meta_iterations : int
            number of meta iterations on the whole group (when using iterative model fitting)
        """
        self.params_dict["meta_iterations"] = meta_iterations

    def set_deblend_factor(self, deblend_factor: float) -> None:
        """
        Parameters
        ----------

        """
        self.params_dict["deblend_factor"] = deblend_factor

    def set_meta_iteration_stop(self, meta_iteration_stop: float) -> None:
        """
        Parameters
        ----------

        """
        self.params_dict["meta_iteration_stop"] = meta_iteration_stop

    def set_window_type(self, window_type: WindowType) -> None:
        """
        Parameters
        ----------

        window_type : WindowType
            specify the type of model fitting window

        """
        if type(window_type) is not WindowType:
            raise TypeError("Window type must be a WindowType enum value!")

        self.params_dict["window_type"] = window_type

    def set_ellipse_scale(self, ellipse_scale: float) -> None:
        """
        Parameters
        ----------

        ellipse_scale : double
            specify scaling of elliptic footprints

        """
        self.params_dict["ellipse_scale"] = ellipse_scale

    def set_show_fitting_areas(self, show_fitting_areas: bool) -> None:
        """
        Parameters
        ----------

        show_fitting_areas : bool
            Specify whether to output the model fitting windows.

        """
        self.params_dict["show_fitting_areas"] = show_fitting_areas


def print_model_fitting_info(group: MeasurementGroup, show_params: bool = False, prefix: str = '',
                             file: IO[str] | None = None) -> None:
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
    file = file or sys.stderr

    if group.models:
        print(f'{prefix}Models:', file=file)
        for m in group.models:
            print(f'{prefix}  {m.to_string(show_params)}', file=file)
    for x in group:
        if isinstance(x, tuple):
            print(f'{prefix}{x[0]}:', file=file)
            print_model_fitting_info(x[1], show_params, prefix + '    ', file=file)
