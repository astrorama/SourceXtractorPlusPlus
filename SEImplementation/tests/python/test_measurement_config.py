"""
Test the image loader functions and `MeasurementConfig` class.
"""

import numpy as np
import pytest
from astropy.io import fits

from sourcextractor.config.measurement_config import (MeasurementConfig,
                                                      data_cube_slices)
from sourcextractor.config.measurement_images import MeasurementImage


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

    def test_load_image_takes_a_measurement_image(self, config, asdf_metadata_file):
        """The point of the interface: the image already describes itself"""
        image = MeasurementImage(asdf_metadata_file, image='/data',
                                 gain_keyword='/meta/gain', fits_compat=False)
        group = config.load_image(image)

        assert list(group.images()) == [image]
        assert config.measurement_images[image.id] is image

    def test_load_image_builds_one_from_arguments(self, config, asdf_metadata_file):
        group = config.load_image(asdf_metadata_file, image='/data',
                                  gain_keyword='/meta/gain')
        image, = group.images()
        assert image.image == '/data'
        assert image.gain == 2.5

    def test_load_image_refuses_an_image_and_arguments_together(
            self, config, asdf_metadata_file):
        image = MeasurementImage(asdf_metadata_file, image='/data', gain=1.,
                                 fits_compat=False)
        with pytest.raises(TypeError, match='already describes its image'):
            config.load_image(image, gain=2.)

    def test_load_images_takes_images_that_differ(self, config, fits_multiple_hdu,
                                                  asdf_metadata_file):
        """Each carries its own selector, which a shared kwarg could not express"""
        images = [MeasurementImage(fits_multiple_hdu, image='IMAGE2', gain=1.,
                                   fits_compat=False),
                  MeasurementImage(asdf_metadata_file, image='/data', gain=2.,
                                   fits_compat=False)]
        group = config.load_images(images)

        assert [im.image for im in group.images()] == ['IMAGE2', '/data']
        assert [im.gain for im in group.images()] == [1., 2.]

    def test_load_image_does_not_use_the_fits_compat_metadata(self, config,
                                                              fits_with_primary):
        """A FITS file read through the new interface behaves like any other"""
        legacy, = config.load_fits_image(fits_with_primary).images()
        modern, = config.load_image(fits_with_primary).images()

        assert isinstance(legacy.meta, dict)
        assert legacy.meta['GAIN'] == '42'
        assert not isinstance(modern.meta, dict)
        assert modern.meta['GAIN'] == 42.0


class TestDataCubeSlices:
    def test_one_slice_per_layer(self, cube_asdf_file):
        slices = data_cube_slices(cube_asdf_file, image='/data', gain=1.)

        assert [s.image_layer for s in slices] == [0, 1, 2]
        assert all(s.image == '/data' and s.is_data_cube for s in slices)

    def test_serves_both_formats(self, cube_fits_file, cube_asdf_file):
        assert len(data_cube_slices(cube_fits_file, gain=1.)) == 3
        assert len(data_cube_slices(cube_asdf_file, image='/data', gain=1.)) == 3

    def test_a_weight_cube_is_matched_layer_by_layer(self, cube_asdf_file):
        slices = data_cube_slices(cube_asdf_file, weight_file=cube_asdf_file,
                                  image='/data', weight='/data',
                                  weight_type='weight', gain=1.)
        assert [s.weight_layer for s in slices] == [0, 1, 2]

    def test_a_two_dimensional_array_is_not_a_cube(self, asdf_metadata_file):
        with pytest.raises(ValueError, match='Not a data cube'):
            data_cube_slices(asdf_metadata_file, image='/data', gain=1.)
