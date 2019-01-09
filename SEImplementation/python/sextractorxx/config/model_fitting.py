from __future__ import division, print_function

from enum import Enum

import libSEImplementation as cpp
from psutil.tests import sh


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

    def __init__(self, init_value, range=None):
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

