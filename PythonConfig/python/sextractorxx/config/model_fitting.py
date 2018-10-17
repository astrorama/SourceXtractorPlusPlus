from __future__ import division, print_function

from enum import Enum


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


_parameter_counter = 0


parameter_dict = {}

def print_parameters():
    for n in parameter_dict:
        print('{}: {}'.format(n, parameter_dict[n]))


class ParameterBase(object):

    def __init__(self):
        global _parameter_counter
        _parameter_counter += 1
        self.id = _parameter_counter
        parameter_dict[self.id] = self

    def __str__(self):
        return '(ID:{})'.format(self.id)


class ConstantParameter(ParameterBase):

    def __init__(self, init_value):
        ParameterBase.__init__(self)
        self.__init_value = init_value

    def get_init_value(self):
        return self.__init_value if hasattr(self.__init_value, '__call__') else lambda o: self.__init_value

    def __str__(self):
        res = ParameterBase.__str__(self)[:-1] + ', value:'
        if hasattr(self.__init_value, '__call__'):
            res += 'func'
        else:
            res += str(self.__init_value)
        return res + ')'


class FreeParameter(ConstantParameter):

    def __init__(self, init_value, range=None):
        ConstantParameter.__init__(self, init_value)
        self.__range = range

    def get_range(self):
        return self.__range

    def __str__(self):
        res = ConstantParameter.__str__(self)[:-1]
        res = res.replace('value', 'init')
        if self.__range:
            res += ', range:' + str(self.__range)
        return res + ')'


class DependentParameter(ParameterBase):

    def __init__(self, func, *params):
        self.func = func
        self.params = [p.id for p in params]


def get_pos_parameters():
    return (
        FreeParameter(lambda o: o.get_alpha_coord(), Range(lambda v,o: (v-o.get_radius_world(), v+o.get_radius_world()), RangeType.LINEAR)),
        FreeParameter(lambda o: o.get_delta_coord(), Range(lambda v,o: (v-o.get_radius_world(), v+o.get_radius_world()), RangeType.LINEAR))
    )


class FluxParameterType():
    ISO = 1


def get_flux_parameter(type=FluxParameterType.ISO):
    func_map = {
        FluxParameterType.ISO : 'get_iso_flux'
    }
    return FreeParameter(lambda o: getattr(o, func_map[type]), Range(lambda v,o: (v * 1E-3, v * 1E3), RangeType.EXPONENTIAL))


class ModelFittingGroup(object):

    def __init__(self, meas_group):
        self.__images = None
        self.__subgroups = None
        if meas_group.is_leaf():
            self.__images = [im for im in meas_group]
        else:
            self.__subgroups = [(n, ModelFittingGroup(g)) for n,g in meas_group]
        self.__models = []

    def __iter__(self):
        if self.__subgroups:
            return self.__subgroups.__iter__()
        else:
            return self.__images.__iter__()

    def __getitem__(self, name):
        if self.__subgroups is None:
            raise Exception('Does not contain subgroups')
        return (x for x in self.__subgroups if x[0] == name).next()[1]

    def add_model(self, model):
        self.__models.append(model)

    def get_models(self):
        return self.__models

    def printToScreen(self, prefix='', show_images=False, show_params=False):
        if self.__images:
            print('{}Image List ({})'.format(prefix, len(self.__images)))
            if show_images:
                for im in self.__images:
                    print('{}{}'.format(prefix, im))
        if self.__subgroups:
            print('{}Sub-groups: {}'.format(prefix, ','.join(
                x for x, _ in self.__subgroups)))
            for name, group in self.__subgroups:
                print('{}  {}:'.format(prefix, name))
                group.printToScreen(prefix + '    ', show_images, show_params)
        if self.__models:
            print('{}Models:'.format(prefix))
            for m in self.__models:
                print ('{}  {}'.format(prefix, m.to_string(show_params)))


class ModelBase(object):
    pass


class PointSourceModel(ModelBase):

    def __init__(self, alpha, delta, flux):
        self.alpha = alpha if isinstance(alpha, ParameterBase) else ConstantParameter(alpha)
        self.delta = delta if isinstance(delta, ParameterBase) else ConstantParameter(delta)
        self.flux = flux if isinstance(flux, ParameterBase) else ConstantParameter(flux)

    def to_string(self, show_params=False):
        if show_params:
            return 'PointSource[alpha={}, delta={}, flux={}]'.format(
                self.alpha, self.delta, self.flux)
        else:
            return 'PointSource[alpha={}, delta={}, flux={}]'.format(
                self.alpha.id, self.delta.id, self.flux.id)


class SersicModel(PointSourceModel):

    def __init__(self, alpha, delta, flux, i0, n, k):
        PointSourceModel.__init__(self, alpha, delta, flux)
        self.i0 = i0 if isinstance(i0, ParameterBase) else ConstantParameter(i0)
        self.n = n if isinstance(n, ParameterBase) else ConstantParameter(n)
        self.k = k if isinstance(k, ParameterBase) else ConstantParameter(k)

    def to_string(self, show_params=False):
        if show_params:
            return 'Sersic[alpha={}, delta={}, flux={}, i0={}, n={}, k={}]'.format(
                self.alpha, self.delta, self.flux, self.i0, self.n, self.k)
        else:
            return 'Sersic[alpha={}, delta={}, flux={}, i0={}, n={}, k={}]'.format(
                self.alpha.id, self.delta.id, self.flux.id, self.i0.id, self.n.id, self.k.id)


class ExponentialModel(SersicModel):

    def __init__(self, alpha, delta, flux, i0, k):
        SersicModel.__init__(self, alpha, delta, flux, i0, 1, k)

    def to_string(self, show_params=False):
        if show_params:
            return 'Exponential[alpha={}, delta={}, flux={}, i0={}, k={}]'.format(
                self.alpha, self.delta, self.flux, self.i0, self.k)
        else:
            return 'Exponential[alpha={}, delta={}, flux={}, i0={}, k={}]'.format(
                self.alpha.id, self.delta.id, self.flux.id, self.i0.id, self.k.id)


class DeVaucouleursModel(SersicModel):

    def __init__(self, alpha, delta, flux, i0, k):
        SersicModel.__init__(self, alpha, delta, flux, i0, 4, k)

    def to_string(self, show_params=False):
        if show_params:
            return 'DeVaucouleurs[alpha={}, delta={}, flux={}, i0={}, k={}]'.format(
                self.alpha, self.delta, self.flux, self.i0, self.k)
        else:
            return 'DeVaucouleurs[alpha={}, delta={}, flux={}, i0={}, k={}]'.format(
                self.alpha.id, self.delta.id, self.flux.id, self.i0.id, self.k.id)


model_fitting_groups = []

def set_model_fitting_groups(*groups):
    model_fitting_groups = groups