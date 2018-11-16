from __future__ import division, print_function
from .model_fitting import ParameterBase

_used_names = set()
model_fitting_parameter_columns = []

_type_column_map = {
    ParameterBase : model_fitting_parameter_columns
}

def print_output_columns():
    if model_fitting_parameter_columns:
        print('Model fitting parameter outputs:')
        for n, ids in model_fitting_parameter_columns:
            print('    {} : {}'.format(n, ids))



def add_output_column(name, params):
    if name in _used_names:
        raise Exception('Column {} is already set'.format(name))
    _used_names.add(name)

    if not isinstance(params, list):
        params = [params]
    param_type = type(params[0])

    for base in _type_column_map:
        if issubclass(param_type, base):
            _type_column_map[base].append((name, params))
