from __future__ import division, print_function

import sys

from .model_fitting import ParameterBase
from .aperture import Aperture

_used_names = set()
model_fitting_parameter_columns = []
aperture_columns = []

_type_column_map = {
    ParameterBase : model_fitting_parameter_columns,
    Aperture: aperture_columns
}


def print_output_columns(file=sys.stderr):
    if model_fitting_parameter_columns:
        print('Model fitting parameter outputs:', file=file)
        for n, ids in model_fitting_parameter_columns:
            print('    {} : {}'.format(n, ids), file=file)
    if aperture_columns:
        print('Aperture outputs:', file=file)
        for n, ids in aperture_columns:
            print('    {} : {}'.format(n, ids), file=file)


def add_output_column(name, params):
    if name in _used_names:
        raise Exception('Column {} is already set'.format(name))
    _used_names.add(name)

    if not isinstance(params, list):
        params = [params]
    param_type = type(params[0])

    known_subclass = False
    for base in _type_column_map:
        if issubclass(param_type, base):
            _type_column_map[base].append((name, params))
            known_subclass = True

    if not known_subclass:
        raise Exception('{} is not a known column type'.format(str(param_type)))
