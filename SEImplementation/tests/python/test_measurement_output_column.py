"""Regression tests for measurement catalog output configuration."""

import unittest

from _SourceXtractorPy import Aperture
from sourcextractor.config.measurement_config import MeasurementConfig
from sourcextractor.config.model_fitting import ConstantParameter, DependentParameter


class MeasurementOutputColumnTest(unittest.TestCase):
    def test_registers_all_derived_parameters_and_dependencies(self):
        config = MeasurementConfig()
        dependencies = [ConstantParameter(2), ConstantParameter(3)]
        params = [DependentParameter(lambda value: value * 2, param) for param in dependencies]

        config.add_output_column("derived", params)

        self.assertEqual(config.model_fitting_parameter_columns, [("derived", params)])
        self.assertEqual(set(config.model_fitting.dependent_parameter_dict), {param.id for param in params})
        self.assertEqual(set(config.model_fitting.constant_parameter_dict), {param.id for param in dependencies})

    def test_invalid_lists_leave_the_column_name_available(self):
        config = MeasurementConfig()
        param = ConstantParameter(2)
        aperture = Aperture([5])

        with self.assertRaises(ValueError):
            config.add_output_column("measurement", [])
        with self.assertRaises(TypeError):
            config.add_output_column("measurement", [param, aperture])
        self.assertFalse(config.model_fitting_parameter_columns)
        self.assertFalse(config.aperture_columns)
        self.assertFalse(config.model_fitting.constant_parameter_dict)

        config.add_output_column("measurement", aperture)
        self.assertEqual(config.aperture_columns, [("measurement", [aperture])])


if __name__ == "__main__":
    unittest.main()
