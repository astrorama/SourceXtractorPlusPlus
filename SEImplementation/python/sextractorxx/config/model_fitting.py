from __future__ import division, print_function

from enum import Enum

import _SExtractorPy as cpp
from .measurement_images import MeasurementGroup

from astropy import units as u
from astropy.coordinates import SkyCoord
from astropy.coordinates import Angle

import math

class RangeType(Enum):
    LINEAR = 1
    EXPONENTIAL = 2


class Range(object):

    def __init__(self, limits, type):
        self.__limits = limits
        self.__type = type

    def get_limits(self):
        return self.__limits if hasattr(self.__limits, '__call__') else lambda v,o: self.__limits

    def get_type(self):
        return self.__type

    def __str__(self):
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
    
    def __init__(self, normalization_factor=1):
        self.__normalization_factor = normalization_factor
    
    def get_normalization_factor(self):
        return self.__normalization_factor if hasattr(self.__normalization_factor, '__call__') else lambda v,o: self.__normalization_factor
    
    def __str__(self):
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

def print_parameters():
    if constant_parameter_dict:
        print('Constant parameters:')
        for n in constant_parameter_dict:
            print('  {}: {}'.format(n, constant_parameter_dict[n]))
    if free_parameter_dict:
        print('Free parameters:')
        for n in free_parameter_dict:
            print('  {}: {}'.format(n, free_parameter_dict[n]))
    if dependent_parameter_dict:
        print('Dependent parameters:')
        for n in dependent_parameter_dict:
            print('  {}: {}'.format(n, dependent_parameter_dict[n]))


class ParameterBase(cpp.Id):
    def __str__(self):
        return '(ID:{})'.format(self.id)


class ConstantParameter(ParameterBase):

    def __init__(self, value):
        ParameterBase.__init__(self)
        self.__value = value
        constant_parameter_dict[self.id] = self

    def get_value(self):
        return self.__value if hasattr(self.__value, '__call__') else lambda o: self.__value

    def __str__(self):
        res = ParameterBase.__str__(self)[:-1] + ', value:'
        if hasattr(self.__value, '__call__'):
            res += 'func'
        else:
            res += str(self.__value)
        return res + ')'


class FreeParameter(ParameterBase):

    def __init__(self, init_value, range=Unbounded()):
        ParameterBase.__init__(self)
        self.__init_value = init_value
        self.__range = range
        free_parameter_dict[self.id] = self

    def get_init_value(self):
        return self.__init_value if hasattr(self.__init_value, '__call__') else lambda o: self.__init_value

    def get_range(self):
        return self.__range

    def __str__(self):
        res = ParameterBase.__str__(self)[:-1] + ', init:'
        if hasattr(self.__init_value, '__call__'):
            res += 'func'
        else:
            res += str(self.__init_value)
        if self.__range:
            res += ', range:' + str(self.__range)
        return res + ')'


class DependentParameter(ParameterBase):

    def __init__(self, func, *params):
        ParameterBase.__init__(self)
        self.func = func
        self.params = [p.id for p in params]
        dependent_parameter_dict[self.id] = self


def get_pos_parameters():
    return (
        FreeParameter(lambda o: o.get_centroid_x(), Range(lambda v,o: (v-o.get_radius(), v+o.get_radius()), RangeType.LINEAR)),
        FreeParameter(lambda o: o.get_centroid_y(), Range(lambda v,o: (v-o.get_radius(), v+o.get_radius()), RangeType.LINEAR))
    )


class FluxParameterType():
    ISO = 1


def get_flux_parameter(type=FluxParameterType.ISO, scale=1):
    func_map = {
        FluxParameterType.ISO : 'get_iso_flux'
    }
    return FreeParameter(lambda o: getattr(o, func_map[type])() * scale, Range(lambda v,o: (v * 1E-3, v * 1E3), RangeType.EXPONENTIAL))


prior_dict = {}

class Prior(cpp.Id):

    def __init__(self, param, value, sigma):
        cpp.Id.__init__(self)
        self.param = param.id
        self.value = value if hasattr(value, '__call__') else lambda o: value
        self.sigma = sigma if hasattr(sigma, '__call__') else lambda o: sigma


def add_prior(param, value, sigma):
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
    if not isinstance(group, MeasurementGroup):
        raise TypeError('Models can only be added on MeasurementGroup, got {}'.format(type(group)))
    if not hasattr(group, 'models'):
        group.models = []
    group.models.append(model)
    _set_model_to_frames(group, model)


def print_model_fitting_info(group, show_params=False, prefix=''):
    if hasattr(group, 'models') and group.models:
        print('{}Models:'.format(prefix))
        for m in group.models:
            print('{}  {}'.format(prefix, m.to_string(show_params)))
    for x in group:
        if isinstance(x, tuple):
            print('{}{}:'.format(prefix, x[0]))
            print_model_fitting_info(x[1], show_params, prefix + '    ')


point_source_model_dict = {}
sersic_model_dict = {}
exponential_model_dict = {}
de_vaucouleurs_model_dict = {}
params_dict = { "max_iterations" : 100, "modified_chi_squared_scale" : 10 }

def set_max_iterations(iterations):
    params_dict["max_iterations"] = iterations
    
def set_modified_chi_squared_scale(scale):
    params_dict["modified_chi_squared_scale"] = scale


class ModelBase(cpp.Id):
    pass


class CoordinateModelBase(ModelBase):

    def __init__(self, x_coord, y_coord, flux):
        ModelBase.__init__(self)
        self.x_coord = x_coord if isinstance(x_coord, ParameterBase) else ConstantParameter(x_coord)
        self.y_coord = y_coord if isinstance(y_coord, ParameterBase) else ConstantParameter(y_coord)
        self.flux = flux if isinstance(flux, ParameterBase) else ConstantParameter(flux)


class PointSourceModel(CoordinateModelBase):

    def __init__(self, x_coord, y_coord, flux):
        CoordinateModelBase.__init__(self, x_coord, y_coord, flux)
        global point_source_model_dict
        point_source_model_dict[self.id] = self

    def to_string(self, show_params=False):
        if show_params:
            return 'PointSource[x_coord={}, y_coord={}, flux={}]'.format(
                self.x_coord, self.y_coord, self.flux)
        else:
            return 'PointSource[x_coord={}, y_coord={}, flux={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id)


class SersicModelBase(CoordinateModelBase):

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle):
        CoordinateModelBase.__init__(self, x_coord, y_coord, flux)
        self.effective_radius = effective_radius if isinstance(effective_radius, ParameterBase) else ConstantParameter(effective_radius)
        self.aspect_ratio = aspect_ratio if isinstance(aspect_ratio, ParameterBase) else ConstantParameter(aspect_ratio)
        self.angle = angle if isinstance(angle, ParameterBase) else ConstantParameter(angle)


class SersicModel(SersicModelBase):

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle, n):
        SersicModelBase.__init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle)
        self.n = n if isinstance(n, ParameterBase) else ConstantParameter(n)
        global sersic_model_dict
        sersic_model_dict[self.id] = self

    def to_string(self, show_params=False):
        if show_params:
            return 'Sersic[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}, n={}]'.format(
                self.x_coord, self.y_coord, self.flux, self.effective_radius, self.aspect_ratio, self.angle, self.n)
        else:
            return 'Sersic[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}, n={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id, self.effective_radius.id, self.aspect_ratio.id, self.angle.id, self.n.id)


class ExponentialModel(SersicModelBase):

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle):
        SersicModelBase.__init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle)
        global exponential_model_dict
        exponential_model_dict[self.id] = self

    def to_string(self, show_params=False):
        if show_params:
            return 'Exponential[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord, self.y_coord, self.flux, self.effective_radius, self.aspect_ratio, self.angle)
        else:
            return 'Exponential[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id, self.effective_radius.id, self.aspect_ratio.id, self.angle.id)


class DeVaucouleursModel(SersicModelBase):

    def __init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle):
        SersicModelBase.__init__(self, x_coord, y_coord, flux, effective_radius, aspect_ratio, angle)
        global de_vaucouleurs_model_dict
        de_vaucouleurs_model_dict[self.id] = self

    def to_string(self, show_params=False):
        if show_params:
            return 'DeVaucouleurs[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord, self.y_coord, self.flux, self.effective_radius, self.aspect_ratio, self.angle)
        else:
            return 'DeVaucouleurs[x_coord={}, y_coord={}, flux={}, effective_radius={}, aspect_ratio={}, angle={}]'.format(
                self.x_coord.id, self.y_coord.id, self.flux.id, self.effective_radius.id, self.aspect_ratio.id, self.angle.id)
            
class WorldCoordinate:
    def __init__(self, ra, dec):
        self.ra = ra
        self.dec = dec
            
def pixel_to_world_coordinate(x, y):
    global coordinate_system
    wc = coordinate_system.image_to_world(cpp.ImageCoordinate(x-1, y-1)) # -1 as we go FITS -> internal 
    return WorldCoordinate(wc.alpha, wc.delta)

def get_sky_coord(x,y):
    coord = pixel_to_world_coordinate(x, y)
    sky_coord = SkyCoord(ra=coord.ra*u.degree, dec=coord.dec*u.degree)
    return sky_coord

def radius_to_wc_angle(x, y, rad):
    return (separation_angle(x, y, x+rad, y) + separation_angle(x, y, x, y+rad)) / 2.0 

def get_separation_angle(x1, y1, x2, y2):
    c1 = get_sky_coord(x1, y1)
    c2 = get_sky_coord(x2, y2)
    return c1.separation(c2).degree

def get_position_angle(x1, y1, x2, y2):
    c1 = get_sky_coord(x1, y1)
    c2 = get_sky_coord(x2, y2)
    angle = c1.position_angle(c2).degree
    
    # return angle normalized to range: -90 <= angle < 90
    return (angle + 90.0) % 180.0 - 90.0

def set_coordinate_system(cs):
    global coordinate_system
    coordinate_system = cs

def get_world_position_parameters(x, y):
    ra = DependentParameter(lambda x,y: pixel_to_world_coordinate(x, y).ra, x, y)
    dec = DependentParameter(lambda x,y: pixel_to_world_coordinate(x, y).dec, x, y)
    return (ra, dec)

def get_world_parameters(x, y, radius, angle, ratio):
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
    
    return (ra, dec, wc_rad, wc_angle, wc_ratio)

