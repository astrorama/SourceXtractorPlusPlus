"""Regression tests for reusable compute-graph model configuration."""

import unittest

from sourcextractor.config.model_fitting import ComputeGraphModel, ConstantParameter


class ComputeGraphModelTest(unittest.TestCase):
    def make_model(self, params):
        return ComputeGraphModel("model.onnx", 0, 0, 1, params)

    def test_constructor_preserves_input_and_reserved_parameters(self):
        scale = ConstantParameter(2)
        aspect_ratio = ConstantParameter(0.5)
        angle = ConstantParameter(0.3)
        params = {"_scale": scale, "_aspect_ratio": aspect_ratio, "_angle": angle, "value": 7}
        original = dict(params)
        first = self.make_model(params)
        second = self.make_model(params)
        self.assertEqual(params, original)
        for model in (first, second):
            self.assertIs(model.scale, scale)
            self.assertIs(model.aspect_ratio, aspect_ratio)
            self.assertIs(model.angle, angle)
            self.assertEqual(set(model.params), {"value"})
            self.assertIsInstance(model.params["value"], ConstantParameter)
        params["value"] = 99
        second.params["extra"] = ConstantParameter(3)
        self.assertNotIn("extra", first.params)
        self.assertIsNot(first.params, second.params)
        self.assertIsNot(first.params, params)

    def test_default_parameter_dictionaries_are_independent(self):
        first = ComputeGraphModel("model.onnx", 0, 0, 1)
        second = ComputeGraphModel("model.onnx", 0, 0, 1)
        first.params["value"] = ConstantParameter(4)
        self.assertEqual(second.params, {})


if __name__ == "__main__":
    unittest.main()
