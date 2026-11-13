from pytest import raises

from sourcextractor.config.measurement_images import MeasurementImage


class TestMeasurementImage:
    def test_property_setters(self, fits_with_primary) -> None:
        """Test functionality of the CheckBoostProperties mixin"""

        im = MeasurementImage(fits_with_primary)
        # weight_absolute expects a bool, strictly
        with raises(TypeError, match='Expecting bool for weight_absolute, got int'):
            im.weight_absolute = 3
        # image_hdu expects an int, strict
        with raises(TypeError, match='Expecting int for image_hdu, got bool'):
            im.image_hdu = False
        with raises(TypeError, match='Expecting int for image_hdu, got float'):
            im.image_hdu = 42.0
        # gain expects a float, but accepts an int
        im.gain = 42
        assert isinstance(im.gain, float)
        assert im.gain == 42.0
