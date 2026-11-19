import shutil

from pytest import raises

from sourcextractor.config import measurement_images as mi
from sourcextractor.config.measurement_images import MeasurementImage


class TestImageFile:
    def test_fits(self, fits_with_primary):
        assert mi.cpp.ImageFile(fits_with_primary).format == 'fits'

    def test_asdf(self, asdf_metadata_file):
        assert mi.cpp.ImageFile(asdf_metadata_file).format == 'asdf'

    def test_fits_file_inherits_it(self, fits_file):
        assert fits_file.format == 'fits'

    def test_detected_from_content_not_the_extension(self, fits_with_primary,
                                                     asdf_metadata_file, tmp_path):
        """Either file is recognized however it is named"""
        misnamed_fits = tmp_path / 'actually_fits.asdf'
        shutil.copy(fits_with_primary, misnamed_fits)
        assert mi.cpp.ImageFile(str(misnamed_fits)).format == 'fits'

        misnamed_asdf = tmp_path / 'actually_asdf.fits'
        shutil.copy(asdf_metadata_file, misnamed_asdf)
        assert mi.cpp.ImageFile(str(misnamed_asdf)).format == 'asdf'

    def test_short_asdf_file(self, tmp_path):
        """The #ASDF header comment is enough, however short the file is"""
        tiny = tmp_path / 'tiny.asdf'
        tiny.write_bytes(b'#ASDF 1.0.0\n')
        assert mi.cpp.ImageFile(str(tiny)).format == 'asdf'


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
