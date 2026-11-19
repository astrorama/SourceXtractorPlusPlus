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


class TestLoadFitsImages:
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


class TestLoadFitsImageWeightMismatch:
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
