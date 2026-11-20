import shutil

import numpy as np
import pytest
from astropy.io import fits
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

    def test_gain_from_the_keyword(self, fits_with_primary):
        assert MeasurementImage(fits_with_primary).gain == 42.0

    def test_gain_from_an_asdf_tree_path(self, asdf_metadata_file):
        image = MeasurementImage(asdf_metadata_file, image_name='/data',
                                 gain_keyword='/meta/gain')
        assert image.gain == 2.5

    def test_explicit_gain_wins(self, fits_with_primary):
        assert MeasurementImage(fits_with_primary, gain=7.).gain == 7.0

    def test_missing_gain_warns(self, tmp_path):
        path = str(tmp_path / 'no_gain.fits')
        fits.HDUList([fits.PrimaryHDU(np.zeros((2, 2), dtype=np.float32))]).writeto(path)

        with pytest.warns(UserWarning, match="has no 'GAIN'; falling back to 0"):
            image = MeasurementImage(path)
        assert image.gain == 0.

    def test_accepts_a_path(self, fits_with_primary):
        assert MeasurementImage(fits_with_primary).file == fits_with_primary

    def test_accepts_an_image_file(self, fits_file, fits_with_primary):
        assert MeasurementImage(fits_file).file == fits_with_primary

    def test_accepts_an_asdf_file(self, asdf_metadata_file):
        image = MeasurementImage(asdf_metadata_file, image_name='/data',
                                 gain_keyword='/meta/gain')
        assert image.file == asdf_metadata_file
        assert image.image_name == '/data'

    def test_requires_an_image_file(self):
        with raises(TypeError, match='requires an image file'):
            MeasurementImage()

    def test_rejects_negative_hdus(self, fits_with_primary):
        with raises(ValueError, match='HDU indices start at 0'):
            MeasurementImage(fits_with_primary, image_hdu=-1)

    def test_fits_meta_is_a_stringified_dict(self, fits_with_primary):
        meta = MeasurementImage(fits_with_primary).meta
        assert isinstance(meta, dict)
        assert meta['GAIN'] == '42'

    def test_fits_meta_keeps_the_synthetic_filename_entries(self, fits_with_primary):
        meta = MeasurementImage(fits_with_primary).meta
        assert meta['IMAGE_FILENAME'] == fits_with_primary
        assert meta['PSF_FILENAME'] == ''
        assert meta['WEIGHT_FILENAME'] == ''

    def test_asdf_meta_is_a_metadata_instance(self, asdf_metadata_file):
        meta = MeasurementImage(asdf_metadata_file, image_name='/data',
                                gain_keyword='/meta/gain').meta
        assert isinstance(meta, mi.Metadata)
        assert meta['/meta/gain'] == 2.5
        assert isinstance(meta['/meta/gain'], float)

    def test_asdf_meta_keeps_the_synthetic_filename_entries(self, asdf_metadata_file):
        meta = MeasurementImage(asdf_metadata_file).meta
        assert meta['IMAGE_FILENAME'] == asdf_metadata_file
        assert meta['PSF_FILENAME'] == ''
        assert meta['WEIGHT_FILENAME'] == ''

    def test_is_cached(self, fits_with_primary):
        image = MeasurementImage(fits_with_primary)
        assert image.meta is image.meta

    def test_an_unnamed_ambiguous_asdf_file_is_refused(self, asdf_multiple_hdu):
        """
        The refusal to guess which array is the image comes from the C++ layer,
        and normally arrives during construction: resolving saturation and
        flux_scale from their keywords is itself what first needs the metadata
        """
        with raises(RuntimeError, match='nothing in the file says which one'):
            MeasurementImage(asdf_multiple_hdu, gain=1.)


class TestByKeyword:
    def test_group_names_are_strings(self, asdf_metadata_file):
        """
        A non-FITS metadata value keeps its own type, but a group is addressed
        by name, so the name has to be a str
        """
        image = MeasurementImage(asdf_metadata_file, image_name='/data',
                                 gain_keyword='/meta/gain')
        assert image.meta['/meta/gain'] == 2.5
        assert not isinstance(image.meta['/meta/gain'], str)

        groups = mi.ByKeyword('/meta/gain')([image])
        assert [name for name, _ in groups] == ['2.5']

    def test_groups_by_a_fits_keyword(self, fits_with_primary):
        image = MeasurementImage(fits_with_primary)
        groups = mi.ByKeyword('GAIN')([image])
        assert [name for name, _ in groups] == ['42']

    def test_missing_keyword_raises(self, fits_with_primary):
        image = MeasurementImage(fits_with_primary)
        with raises(KeyError, match='does not contain the key'):
            mi.ByKeyword('NOSUCHKEY')([image])


class TestImageFileDimensions:
    def test_non_square_asdf(self, asdf_metadata_file):
        """
        metadata.asdf's /data has numpy shape (3, 4), so it is 4 wide and 3
        high; reporting those the other way round made every tile request out
        of bounds, so ASDF images only ever worked while square
        """
        dims = mi.cpp.ImageFile(asdf_metadata_file).get_dimensions('data')
        assert (dims.width, dims.height) == (4, 3)
        assert tuple(dims) == (3, 4)

    def test_depth_is_zero_without_a_cube(self, asdf_metadata_file,
                                          fits_with_primary):
        for path, name in ((asdf_metadata_file, 'data'), (fits_with_primary, None)):
            image_file = mi.cpp.ImageFile(path)
            dims = (image_file.get_dimensions(name) if name
                    else image_file.get_dimensions(0))
            assert dims.depth == 0
            assert not dims.is_cube

    def test_fits_cube_depth_reaches_the_generic_path(self, tmp_path):
        """ImageSource::getDepth() is what lets the generic path see a cube"""
        path = str(tmp_path / 'cube.fits')
        fits.HDUList([fits.PrimaryHDU(
            np.zeros((3, 7, 5), dtype=np.float32))]).writeto(path)

        dims = mi.cpp.ImageFile(path).get_dimensions(0)
        assert (dims.width, dims.height, dims.depth) == (5, 7, 3)
        assert dims.is_cube
        assert tuple(dims) == (3, 7, 5)
