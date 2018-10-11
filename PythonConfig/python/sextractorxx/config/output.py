from __future__ import division, print_function

output_columns = []

def add_output_column(name, params):
    if name in [x for x,_ in output_columns]:
        raise Exception('Column {} is already set'.format(name))
    if isinstance(params, list):
        output_columns.append((name, [x.id for x in params]))
    else:
        output_columns.append((name, params.id))
