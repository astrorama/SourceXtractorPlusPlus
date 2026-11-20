"""
Test the image loader functions and `MeasurementConfig` class.
"""

import numpy as np
import pytest
from astropy.io import fits

from sourcextractor.config.measurement_config import MeasurementConfig


@pytest.fixture
def config():
    return MeasurementConfig()


def write_mef(path, n_images, width=4, height=4):
    """A FITS file with a dataless primary HDU and n_images image extensions"""
    hdus = [fits.PrimaryHDU()]
    for i in range(n_images):
        data = np.full((height, width), float(i + 1), dtype=np.float32)
        hdus.append(fits.ImageHDU(data, name=f'IMAGE{i}'))
    fits.HDUList(hdus).writeto(path)
    return str(path)


@pytest.fixture
def single(tmp_path):
    return write_mef(tmp_path / 'single.fits', 1)


@pytest.fixture
def other_single(tmp_path):
    return write_mef(tmp_path / 'other_single.fits', 1)


class TestMeasurementConfig:
    def test_list_of_images_without_psfs_or_weights(self, config, single,
                                                    other_single):
        """
        psfs and weights are documented as optional, including when images is
        a list
        """
        group = config.load_fits_images([single, other_single])
        assert len(group) == 2

    def test_list_of_images_with_weights_only(self, config, single, other_single):
        group = config.load_fits_images([single, other_single],
                                        weights=[single, other_single])
        assert len(group) == 2

    def test_mismatched_psfs_still_rejected(self, config, single, other_single):
        with pytest.raises(ValueError, match='psf'):
            config.load_fits_images([single, other_single], psfs=[single])

    def test_mismatched_weights_still_rejected(self, config, single, other_single):
        with pytest.raises(ValueError, match='weight'):
            config.load_fits_images([single, other_single], weights=[single])

    def test_a_single_image_is_not_a_list(self, config, single):
        group = config.load_fits_images(single)
        assert len(group) == 1


    def test_shorter_weight_mef_is_rejected(self, config, tmp_path):
        """
        A weight MEF with fewer image HDUs than the image file used to be
        zipped silently, dropping the images that had no weight
        """
        image = write_mef(tmp_path / 'image3.fits', 3)
        weight = write_mef(tmp_path / 'weight2.fits', 2)

        with pytest.raises(ValueError, match='weight'):
            config.load_fits_image(image, weight=weight)

    def test_matching_weight_mef_is_accepted(self, config, tmp_path):
        image = write_mef(tmp_path / 'image3b.fits', 3)
        weight = write_mef(tmp_path / 'weight3.fits', 3)

        group = config.load_fits_image(image, weight=weight)
        assert len(group) == 3

    def test_one_image(self, config, asdf_metadata_file):
        """An ASDF file yields exactly one image, named unless it is unambiguous"""
        group = config.load_asdf_image(asdf_metadata_file, image_name='/data',
                                       gain_keyword='/meta/gain')
        assert len(group) == 1
        image, = group.images()
        assert image.image_name == '/data'
        assert image.gain == 2.5

    def test_unambiguous_file_needs_no_name(self, asdf_metadata_file, config):
        """metadata.asdf holds a single candidate array"""
        group = config.load_asdf_image(asdf_metadata_file,
                                       gain_keyword='/meta/gain')
        assert len(group) == 1

    def test_ambiguous_file_is_refused(self, config, asdf_multiple_hdu):
        with pytest.raises(RuntimeError, match='nothing in the file says which one'):
            config.load_asdf_image(asdf_multiple_hdu, gain=1.)

    def test_registers_the_image(self, config, asdf_metadata_file):
        group = config.load_asdf_image(asdf_metadata_file, image_name='/data',
                                       gain=1.)
        image, = group.images()
        assert config.measurement_images[image.id] is image

    def test_one_image_per_file(self, config, asdf_metadata_file,
                                asdf_multiple_hdu):
        group = config.load_asdf_images([asdf_metadata_file, asdf_multiple_hdu],
                                        image_names=['/data', '/IMAGE'], gain=1.)
        assert len(group) == 2
        assert [im.image_name for im in group.images()] == ['/data', '/IMAGE']

    def test_singular_name_broadcasts(self, config, asdf_multiple_hdu):
        """One layout shared by every file is the common case"""
        group = config.load_asdf_images([asdf_multiple_hdu, asdf_multiple_hdu],
                                        image_name='/IMAGE2', gain=1.)
        assert [im.image_name for im in group.images()] == ['/IMAGE2', '/IMAGE2']

    def test_singular_and_plural_are_mutually_exclusive(self, config,
                                                        asdf_multiple_hdu):
        with pytest.raises(ValueError,
                           match='Pass either image_name or image_names'):
            config.load_asdf_images([asdf_multiple_hdu], image_name='/IMAGE',
                                    image_names=['/IMAGE'], gain=1.)

    def test_mismatched_plural_is_rejected(self, config, asdf_multiple_hdu):
        with pytest.raises(ValueError,
                           match='number of image files and image_names'):
            config.load_asdf_images([asdf_multiple_hdu, asdf_multiple_hdu],
                                    image_names=['/IMAGE'], gain=1.)

    def test_weight_names_follow_the_same_rule(self, config, asdf_multiple_hdu):
        group = config.load_asdf_images([asdf_multiple_hdu, asdf_multiple_hdu],
                                        image_name='/IMAGE',
                                        weight_names=['/IMAGE2', '/IMAGE'],
                                        gain=1.)
        assert [im.weight_name for im in group.images()] == ['/IMAGE2', '/IMAGE']

    def test_empty_list_is_rejected(self, config):
        with pytest.raises(ValueError, match='An empty list passed'):
            config.load_asdf_images([])


class TestMeasurementConfigLoadAsdfDataCube:
    """cube.asdf's /data is (depth 3, height 4, width 5)"""

    def test_one_image_per_layer(self, config, cube_asdf_file):
        group = config.load_asdf_data_cube(cube_asdf_file, image_name='/data',
                                           gain=1.)
        assert len(group) == 3
        assert [im.image_layer for im in group.images()] == [0, 1, 2]

    def test_every_slice_names_the_same_array(self, config, cube_asdf_file):
        group = config.load_asdf_data_cube(cube_asdf_file, image_name='/data',
                                           gain=1.)
        for im in group.images():
            assert im.image_name == '/data'
            assert im.is_data_cube

    def test_a_2d_array_is_not_a_cube(self, config, asdf_metadata_file):
        with pytest.raises(ValueError, match='Not a data cube'):
            config.load_asdf_data_cube(asdf_metadata_file, image_name='/data',
                                       gain=1.)

    def test_psf_list_must_match_the_layers(self, config, cube_asdf_file):
        with pytest.raises(ValueError, match='number of psf files'):
            config.load_asdf_data_cube(cube_asdf_file, psf=['a.psf', 'b.psf'],
                                       image_name='/data', gain=1.)

    def test_weight_list_must_match_the_layers(self, config, cube_asdf_file):
        with pytest.raises(ValueError, match='number of weight files'):
            config.load_asdf_data_cube(cube_asdf_file, weight=['a', 'b'],
                                       image_name='/data', gain=1.)

    def test_a_weight_cube_is_matched_layer_by_layer(self, config,
                                                     cube_asdf_file):
        group = config.load_asdf_data_cube(cube_asdf_file,
                                           weight=cube_asdf_file,
                                           image_name='/data',
                                           weight_name='/data',
                                           weight_type='weight', gain=1.)
        assert [im.weight_layer for im in group.images()] == [0, 1, 2]

    def test_a_flat_weight_image_is_shared(self, config, cube_asdf_file,
                                           asdf_metadata_file):
        """A two-dimensional weight array applies to every layer"""
        group = config.load_asdf_data_cube(cube_asdf_file,
                                           weight=asdf_metadata_file,
                                           image_name='/data',
                                           weight_name='/data',
                                           weight_type='weight', gain=1.)
        assert [im.weight_layer for im in group.images()] == [0, 0, 0]

    def test_a_mismatched_weight_cube_is_rejected(self, config, cube_asdf_file,
                                                  tmp_path):
        import asdf
        import numpy as np
        other = str(tmp_path / 'short_cube.asdf')
        asdf.AsdfFile({
            'data': np.zeros((2, 4, 5), dtype=np.float32)
        }).write_to(other)

        with pytest.raises(ValueError,
                           match='has 2 layers but the image cube has 3'):
            config.load_asdf_data_cube(cube_asdf_file, weight=other,
                                       image_name='/data', weight_name='/data',
                                       weight_type='weight', gain=1.)
