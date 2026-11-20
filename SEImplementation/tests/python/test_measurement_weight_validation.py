"""Regression tests for FITS weight validation in measurement configuration."""

from pathlib import Path
from tempfile import TemporaryDirectory
import unittest

import numpy as np
from astropy.io import fits

from sourcextractor.config.measurement_config import MeasurementConfig


class MeasurementWeightValidationTest(unittest.TestCase):
    def setUp(self):
        directory = TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.directory = Path(directory.name)
        self.plane = np.ones((5, 4), dtype=np.float32)

    def write_fits(self, name, *images):
        filename = str(self.directory / name)
        primary = fits.PrimaryHDU(images[0] if images else None)
        with fits.HDUList([primary] + [fits.ImageHDU(image) for image in images[1:]]) as hdus:
            hdus.writeto(filename)
        return filename

    def test_weight_hdu_counts(self):
        measurement_file = self.write_fits("measurement.fits", *([self.plane] * 3))
        measurement_cube = self.write_fits("measurement_cube.fits", np.stack([self.plane] * 3))

        for count in (0, 1, 3, 4):
            weight = self.write_fits("weight_{}.fits".format(count), *([self.plane] * count))
            for method, image in (("load_fits_image", measurement_file), ("load_fits_data_cube", measurement_cube)):
                with self.subTest(method=method, weight_count=count):
                    config = MeasurementConfig()
                    load = getattr(config, method)
                    if count == 3:
                        self.assertEqual(len(list(load(image, weight=weight))), 3)
                    else:
                        with self.assertRaises(ValueError) as raised:
                            load(image, weight=weight)
                        message = str(raised.exception)
                        self.assertIn(weight, message)
                        self.assertIn(image, message)
                        self.assertIn("contains {} image HDU".format(count), message)
                        self.assertIn("3 measurement image", message)
                        self.assertFalse(config.measurement_images)

        weight = self.write_fits("shared_weight.fits", self.plane)
        images = MeasurementConfig().load_fits_image(measurement_file, weight=weight, weight_hdu=0)
        self.assertEqual([image.weight_hdu for image in images], [0, 0, 0])

    def test_weight_cube_dimensions(self):
        expected = (4, 5, 3)
        measurement = self.write_fits("measurement_cube.fits", np.stack([self.plane] * 3))
        for actual in ((4, 5, 1), (4, 5, 4), (6, 5, 3), (4, 6, 3), expected):
            with self.subTest(weight_dimensions=actual):
                filename = "weight_{}_{}_{}.fits".format(*actual)
                weight = self.write_fits(filename, self.plane, np.ones(actual[::-1], dtype=np.float32))
                config = MeasurementConfig()
                if actual == expected:
                    images = config.load_fits_data_cube(measurement, weight=weight, weight_cube_hdu=1)
                    self.assertEqual([(image.weight_hdu, image.weight_layer) for image in images],
                                     [(1, 0), (1, 1), (1, 2)])
                else:
                    with self.assertRaises(ValueError) as raised:
                        config.load_fits_data_cube(measurement, weight=weight, weight_cube_hdu=1)
                    message = str(raised.exception)
                    for detail in (measurement, weight, "(HDU 1)", str(expected), str(actual)):
                        self.assertIn(detail, message)
                    self.assertFalse(config.measurement_images)


if __name__ == "__main__":
    unittest.main()
