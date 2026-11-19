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

    def test_image_names_fits(self, fits_multiple_hdu):
        """A FITS image is named by its EXTNAME"""
        names = mi.cpp.ImageFile(fits_multiple_hdu).image_names
        assert names == ('COMPRESSED_IMAGE', 'IMAGE2')
        assert isinstance(names, tuple)

    def test_image_names_are_trimmed(self, fits_multiple_hdu):
        """The raw EXTNAME value is padded out to the width of the card"""
        for name in mi.cpp.ImageFile(fits_multiple_hdu).image_names:
            assert name == name.strip()

    def test_image_names_resolve(self, fits_multiple_hdu):
        """Every non-empty name is accepted by the by-name accessors"""
        image_file = mi.cpp.ImageFile(fits_multiple_hdu)
        for name in image_file.image_names:
            assert image_file.get_dimensions(name).width > 0

    def test_image_names_empty_for_an_unnamed_hdu(self, fits_with_primary):
        """A primary HDU has no EXTNAME, so it can only be reached by index"""
        assert mi.cpp.ImageFile(fits_with_primary).image_names == ('',)

    def test_image_names_line_up_with_image_hdus(self, fits_file,
                                                 fits_multiple_hdu):
        """On a FITS file the two lists describe the same images, in order"""
        multiple = mi.cpp.FitsFile(fits_multiple_hdu)
        assert len(multiple.image_names) == len(list(multiple.image_hdus))
        assert len(fits_file.image_names) == len(list(fits_file.image_hdus))

    def test_image_names_asdf(self, asdf_multiple_hdu):
        """For ASDF the names are tree paths of candidate ndarrays"""
        assert mi.cpp.ImageFile(asdf_multiple_hdu).image_names == ('/IMAGE', '/IMAGE2')

    def test_image_names_asdf_is_how_you_disambiguate(self, asdf_multiple_hdu):
        """
        An ASDF file with more than one candidate refuses to guess, so the
        names are the only way to pick one
        """
        image_file = mi.cpp.ImageFile(asdf_multiple_hdu)
        with raises(RuntimeError, match='nothing in the file says which one'):
            image_file.get_dimensions(0)

        for name in image_file.image_names:
            assert image_file.get_dimensions(name).width > 0


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
