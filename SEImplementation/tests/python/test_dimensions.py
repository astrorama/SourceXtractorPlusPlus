"""
The Dimensions type, and the two axis orders the image interfaces report.

FitsFile keeps the FITS 'xy' order it has always used; everything else uses
the NumPy 'yx' order.
"""

import numpy as np
import pytest
from astropy.io import fits

from sourcextractor.config import measurement_images as mi
from sourcextractor.dimensions import Dimensions


@pytest.fixture
def cube_fits(tmp_path):
    """A 5x7 image of 3 layers: NAXIS1=5, NAXIS2=7, NAXIS3=3"""
    path = tmp_path / 'cube.fits'
    fits.HDUList([fits.PrimaryHDU(np.zeros((3, 7, 5), dtype=np.float32))]).writeto(path)
    return str(path)


class TestDimensions:
    def test_yx_is_the_default_order(self):
        dims = Dimensions(5, 7)
        assert dims.axis_order == 'yx'
        assert tuple(dims) == (7, 5)

    def test_xy_order_reverses_the_index_order(self):
        dims = Dimensions(5, 7, axis_order='xy')
        assert tuple(dims) == (5, 7)

    def test_named_accessors_are_order_independent(self):
        for order in ('xy', 'yx'):
            dims = Dimensions(5, 7, 3, axis_order=order)
            assert (dims.width, dims.height, dims.depth) == (5, 7, 3)

    def test_depth_is_only_included_when_a_cube(self):
        assert len(Dimensions(5, 7)) == 2
        assert len(Dimensions(5, 7, 3)) == 3
        assert Dimensions(5, 7).is_cube is False
        assert Dimensions(5, 7, 3).is_cube is True

    def test_cube_index_order(self):
        assert tuple(Dimensions(5, 7, 3, axis_order='xy')) == (5, 7, 3)
        assert tuple(Dimensions(5, 7, 3, axis_order='yx')) == (3, 7, 5)

    def test_behaves_like_a_tuple(self):
        dims = Dimensions(5, 7, axis_order='xy')
        assert isinstance(dims, tuple)
        assert len(dims) == 2
        assert dims[0] == 5 and dims[1] == 7
        assert dims[-1] == 7
        assert list(dims) == [5, 7]
        width, height = dims
        assert (width, height) == (5, 7)

    def test_equality_includes_axis_order(self):
        """The same image described in two orders is not the same Dimensions"""
        assert Dimensions(5, 7, axis_order='xy') == Dimensions(5, 7, axis_order='xy')
        assert Dimensions(5, 7, axis_order='xy') != Dimensions(5, 7, axis_order='yx')
        assert Dimensions(5, 7) != Dimensions(5, 8)

    def test_compares_against_a_plain_tuple_in_index_order(self):
        assert Dimensions(5, 7, axis_order='xy') == (5, 7)
        assert Dimensions(5, 7, axis_order='yx') == (7, 5)
        assert Dimensions(5, 7, axis_order='xy') != (7, 5)

    def test_hashable(self):
        dims = Dimensions(5, 7, axis_order='xy')
        assert hash(dims) == hash((5, 7))
        assert len({dims, Dimensions(5, 7, axis_order='xy')}) == 1

    def test_properties_are_read_only(self):
        dims = Dimensions(5, 7)
        for name in ('width', 'height', 'depth', 'axis_order', 'is_cube'):
            with pytest.raises(AttributeError):
                setattr(dims, name, 1)

    def test_bad_axis_order_rejected(self):
        with pytest.raises(ValueError, match='axis_order must be one of'):
            Dimensions(5, 7, axis_order='zyx')

    def test_repr_names_the_axes(self):
        assert repr(Dimensions(5, 7)) == "Dimensions(width=5, height=7, axis_order='yx')"
        assert repr(Dimensions(5, 7, 3, axis_order='xy')) == (
            "Dimensions(width=5, height=7, depth=3, axis_order='xy')")


class TestFitsFileDimensions:
    def test_keeps_the_fits_axis_order(self, cube_fits):
        """Backwards compatible: NAXIS1, NAXIS2, NAXIS3 as before"""
        dims = mi.cpp.FitsFile(cube_fits).get_dimensions(0)
        assert dims.axis_order == 'xy'
        assert tuple(dims) == (5, 7, 3)
        assert len(dims) == 3
        assert dims[2] == 3

    def test_named_accessors(self, cube_fits):
        dims = mi.cpp.FitsFile(cube_fits).get_dimensions(0)
        assert (dims.width, dims.height, dims.depth) == (5, 7, 3)
        assert dims.is_cube


class TestImageFileDimensions:
    def test_uses_the_numpy_axis_order(self, cube_fits):
        dims = mi.cpp.ImageFile(cube_fits).get_dimensions(0)
        assert dims.axis_order == 'yx'
        assert tuple(dims) == (3, 7, 5)

    def test_image_cube_depth(self, cube_fits):
        """ImageSource::getDepth() is what lets the generic path see a cube"""
        dims = mi.cpp.ImageFile(cube_fits).get_dimensions(0)
        assert dims.depth == 3
        assert dims.is_cube

    def test_by_name(self, fits_multiple_hdu):
        """A FITS image is named by its EXTNAME"""
        dims = mi.cpp.ImageFile(fits_multiple_hdu).get_dimensions('IMAGE2')
        assert dims.axis_order == 'yx'
        assert tuple(dims) == (1, 1)

    def test_two_orders_of_one_image_are_not_equal(self, cube_fits):
        fits_dims = mi.cpp.FitsFile(cube_fits).get_dimensions(0)
        generic = mi.cpp.ImageFile(cube_fits).get_dimensions(0)
        assert fits_dims != generic
        assert (fits_dims.width, fits_dims.height) == (generic.width, generic.height)


class TestCubeFixtures:
    """
    cube.fits and cube.asdf hold the same (depth 3, height 4, width 5) array

    All three axes have different lengths and the two formats must agree about
    which axis is which.
    """

    def test_asdf_cube(self, cube_asdf_file):
        dims = mi.cpp.ImageFile(cube_asdf_file).get_dimensions('data')
        assert (dims.width, dims.height, dims.depth) == (5, 4, 3)
        assert dims.is_cube
        assert dims.axis_order == 'yx'
        assert tuple(dims) == (3, 4, 5)

    def test_fits_cube_through_the_generic_path(self, cube_fits_file):
        dims = mi.cpp.ImageFile(cube_fits_file).get_dimensions(0)
        assert (dims.width, dims.height, dims.depth) == (5, 4, 3)
        assert tuple(dims) == (3, 4, 5)

    def test_fits_cube_keeps_the_fits_axis_order(self, cube_fits_file):
        """FitsFile reports NAXIS1, NAXIS2, NAXIS3 as it always has"""
        dims = mi.cpp.FitsFile(cube_fits_file).get_dimensions(0)
        assert dims.axis_order == 'xy'
        assert tuple(dims) == (5, 4, 3)
        assert (dims.width, dims.height, dims.depth) == (5, 4, 3)

    def test_both_formats_describe_the_same_image(self, cube_fits_file,
                                                  cube_asdf_file):
        fits_dims = mi.cpp.ImageFile(cube_fits_file).get_dimensions(0)
        asdf_dims = mi.cpp.ImageFile(cube_asdf_file).get_dimensions('data')
        assert fits_dims == asdf_dims
