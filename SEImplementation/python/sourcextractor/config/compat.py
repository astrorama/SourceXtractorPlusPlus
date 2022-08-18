# -*- coding: utf-8 -*-

# Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
import sys
from typing import List, Union

import _SourceXtractorPy as cpp

from .measurement_config import global_measurement_config
from .measurement_images import ImageGroup, MeasurementGroup, MeasurementImage
from .model_fitting import ModelBase

Aperture = cpp.Aperture


def add_aperture_photometry(target: Union[MeasurementImage, MeasurementGroup],
                            apertures: Union[float, List[float]]):
    """
    Flux measurement from the image above the background inside a circular aperture.

    Parameters
    ----------
    target : MeasurementImage object, or leaf MeasurementGroup object with a single image, or a list of either
        Target images on which to measure the aperture photometry. Leaf MeasurementGroup with a single image
        are accepted as a convenience.

    apertures : float, or list of float
        Diameter of the aperture. As different MeasurementImage may not be aligned, nor have equivalent pixel size,
        the aperture is interpreted as diameter in pixels of a circle on the detection image.
        A transformation will be applied for each frame, so the covered area is equivalent.

    Returns
    -------
    list of Aperture objects
        An Aperture object is an internal representation of a property on the measurement frame that contains the
        apertures. To actually get the measurements on the output catalog, you need to add explicitly them to the
        output.

    See Also
    --------
    add_output_column

    Notes
    -----
    This property will generate five columns with the prefix specified by `add_output_column`:
    - ``_flux`` and ``_flux_err``, for the flux and its associated error
    - ``_mag`` and ``_mag_err``, for the magnitude and its associated error
    - ``_flags``, to mark, for instance, saturation, boundary conditions, etc.

    For M apertures and N images, the cells on the output column will be an array of MxN fluxes.

    Examples
    --------
    >>> measurement_group = MeasurementGroup(load_fits_images(frames, psfs))
    >>> all_apertures = []
    >>> for img in measurement_group:
    >>>     all_apertures.extend(add_aperture_photometry(img, [5, 10, 20]))
    >>> add_output_column('aperture', all_apertures)
    """
    global_measurement_config.add_measurement_image(target)
    return global_measurement_config.add_aperture_photometry(target, apertures)


def print_measurement_images(file=sys.stderr):
    """
    Print a human-readable representation of the configured measurement images.

    Parameters
    ----------
    file : file object
        Where to print the representation. Defaults to sys.stderr
    """
    global_measurement_config.print_measurement_images(file)


def load_fits_image(image, psf=None, weight=None, **kwargs):
    """
    Creates an image group with the images of a (possibly multi-HDU) single FITS file.

    If image is multi-hdu, psf and weight can either be multi hdu or lists of individual files.

    In any case, they are matched in order and HDUs not containing images (two dimensional arrays) are ignored.

    :param image: The filename of the FITS file containing the image(s)
    :param psf: psf file or list of psf files
    :param weight: FITS file for the weight image or a list of such files

    :return: A ImageGroup representing the images
    """
    return global_measurement_config.load_fits_image(image, psf, weight, **kwargs)


def load_fits_images(images, psfs=None, weights=None, **kwargs):
    """
    Creates an image group for the given images.

    Parameters
    ----------
    images : list of str
        A list of relative paths to the images FITS files. Can also be single string in which case,
         this function acts like load_fits_image
    psfs : list of str
        A list of relative paths to the PSF FITS files (optional). It must match the length of image_list or be None.
    weights : list of str
        A list of relative paths to the weight files (optional). It must match the length of image_list or be None.

    Returns
    -------
    ImageGroup
        A ImageGroup representing the images

    Raises
    ------
    ValueError
        In case of mismatched list of files
    """
    return global_measurement_config.load_fits_images(images, psfs, weights, **kwargs)


def load_fits_data_cube(self, image, psf=None, weight=None, image_cube_hdu=-1,
                        weight_cube_hdu=-1, **kwargs):
    """
    Creates an image group with the images of a data cube.

    weight can be a matching datacube, multi-hdu or list of individual images
    psf can be a multi-hdu or list of individual images to be matched

    In any case, they are matched in order and HDUs not containing images are ignored.

    :param image: The filename of the FITS file containing the image datacube
    :param psf: psf file or list of psf files
    :param weight: FITS file contianing a weight datacube, a MEF contianing the weights
         or a list of such files
    :param image_cube_hdu: hdu containing the image datacube, default = first HDU containing image data
    :param weight_cube_hdu: hdu containing the weight datacube, default = first HDU containing image data

    :return: A ImageGroup representing the images
    """
    return global_measurement_config.load_fits_data_cube(image, psf, weight, image_cube_hdu,
                                                         weight_cube_hdu, **kwargs)


def print_output_columns(file=sys.stderr):
    """
    Print a human-readable representation of the configured output columns.

    Parameters
    ----------
    file : file object
        Where to print the representation. Defaults to sys.stderr
    """
    global_measurement_config.print_output_columns(file)


def add_output_column(name, params):
    """
    Add a new set of columns to the output catalog.

    Parameters
    ----------
    name : str
        Name/prefix of the new set of columns
    params : list of columns
        List of properties to add to the output with the given name/prefix. They must be subtype
        of one of the known ones: ParameterBase for model fitting, or Aperture for aperture photometry.

    Raises
    ------
    ValueError
        If the name has already been used
    TypeError
        If any of the parameters are not of a known type (see params)

    See Also
    --------
    aperture.add_aperture_photometry
    model_fitting.ParameterBase
    """
    global_measurement_config.add_output_column(name, params)


def add_prior(param, value, sigma):
    global_measurement_config.model_fitting.add_prior(param, value, sigma)


def add_model(group: ImageGroup, model: ModelBase):
    global_measurement_config.add_measurement_image(group)
    global_measurement_config.model_fitting.add_model(group, model)


def set_engine(engine):
    global_measurement_config.model_fitting.set_engine(engine)


def set_max_iterations(max_iter):
    global_measurement_config.model_fitting.set_max_iterations(max_iter)


def set_meta_iterations(max_iter):
    global_measurement_config.model_fitting.set_meta_iterations(max_iter)


def set_meta_iteration_stop(stop):
    global_measurement_config.model_fitting.set_meta_iteration_stop(stop)


def set_deblend_factor(factor):
    global_measurement_config.model_fitting.set_deblend_factor(factor)


def set_modified_chi_squared_scale(scale):
    global_measurement_config.model_fitting.set_modified_chi_squared_scale(scale)


def use_iterative_fitting(use):
    global_measurement_config.model_fitting.use_iterative_fitting(use)
