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
from collections.abc import Iterable, Sequence
from itertools import repeat, zip_longest
from typing import Any, IO, TypeAlias, cast

import _SourceXtractorPy as cpp

from .measurement_images import (DataCubeSlice, FitsFile, ImageGroup, MeasurementGroup,
                                 MeasurementImage)
from .model_fitting import FittableParameter, ModelFitting, ParameterBase


OutputColumn: TypeAlias = FittableParameter | cpp.Aperture | list[FittableParameter] | list[cpp.Aperture]
# TODO: It may be possible to implement a special type just for leaf
# nodes, but in the meantime the is_leaf() requirement is a runtime check
Target: TypeAlias = MeasurementImage | MeasurementGroup | list[MeasurementImage] | list[MeasurementGroup]


class MeasurementConfig:
    def __init__(self) -> None:
        self._measurement_images: dict[int, MeasurementImage] = {}
        self._apertures_for_image: dict[int, cpp.Aperture] = {}
        self._used_names: set[str] = set()
        self._model_fitting_parameter_columns: list[tuple[str, FittableParameter]] = []
        self._aperture_columns: list[tuple[str, cpp.Aperture]] = []
        self._column_map = {
            ParameterBase: self._model_fitting_parameter_columns,
            cpp.Aperture: self._aperture_columns
        }
        self._model_fitting = ModelFitting()

    @property
    def apertures_for_image(self) -> dict[int, cpp.Aperture]:
        return self._apertures_for_image

    @property
    def measurement_images(self) -> dict[int, MeasurementImage]:
        return self._measurement_images

    @property
    def model_fitting_parameter_columns(self) -> list[tuple[str, FittableParameter]]:
        return self._model_fitting_parameter_columns

    @property
    def aperture_columns(self) -> list[tuple[str, cpp.Aperture]]:
        return self._aperture_columns

    @property
    def model_fitting(self) -> ModelFitting:
        return self._model_fitting

    def add_measurement_image(self, image: MeasurementImage | MeasurementGroup) -> None:
        if isinstance(image, MeasurementImage):
            if image.id not in self._measurement_images:
                self._measurement_images[image.id] = image
        elif image.is_leaf():
            # See below note about type narrowing
            for member in image:
                member = cast(MeasurementImage, member)
                self.add_measurement_image(member)
        else:
            for item in image:
                # cast: MeasurementGroup.__iter__ returns two different
                # types of iterators depending on whether or not it has subgroups or is a leaf
                # This is a bit clunky and might be good to reconsider; or at least, calling
                # image.is_leaf() does not allow type narrowing here.
                item = cast(tuple[str, MeasurementGroup], item)
                _, subgroup = item
                self.add_measurement_image(subgroup)

    # TODO: better typing for kwargs passed through to DataCubeSlice
    def load_fits_image(self, image: str, psf: str | list[str] | None = None,
                        weight: str | FitsFile | list[FitsFile] | list[str] | None = None,
                        **kwargs: Any) -> ImageGroup:
        """
        Creates an image group with the images of a (possibly multi-HDU) single FITS file.

        If image is multi-hdu, psf and weight can either be multi hdu or lists of individual files.

        In any case, they are matched in order and HDUs not containing images (two dimensional arrays) are ignored.

        :param image: The filename of the FITS file containing the image(s)
        :param psf: psf file or list of psf files
        :param weight: FITS file for the weight image or a list of such files

        :return: A ImageGroup representing the images
        """

        image_file = FitsFile(image)
        if "image_hdu" in kwargs.keys():
            image_hdu_list = [kwargs.pop("image_hdu")]
        else:
            image_hdu_list = image_file.hdu_list

        psf_file_list: Sequence[str | None]

        # handles the PSFs
        if isinstance(psf, list):
            if len(psf) != len(image_hdu_list):
                raise ValueError("The number of psf files must match the number of images!")
            psf_file_list = psf
            psf_hdu_list = [0] * len(psf_file_list)
        else:
            if "psf_hdu" in kwargs.keys():
                psf_hdu_list = [kwargs.pop("psf_hdu")] * len(image_hdu_list)
            else:
                # NOTE: Technically need not build a list here, but it is
                # normally a small list in practice
                psf_hdu_list = list(range(len(image_hdu_list)))
            psf_file_list = [psf] * len(image_hdu_list)

        weight_file_list: Sequence[str | FitsFile | None]

        # handles the weight maps
        if isinstance(weight, list):
            if len(weight) != len(image_hdu_list):
                raise ValueError("The number of weight files must match the number of images!")
            weight_file_list = weight
            weight_hdu_list = [0] * len(weight_file_list)
        elif weight is None:
            weight_file_list = [None] * len(image_hdu_list)
            weight_hdu_list = [0] * len(weight_file_list)
        else:
            if not isinstance(weight, FitsFile):
                weight = FitsFile(weight)
            if "weight_hdu" in kwargs.keys():
                weight_hdu_list = [kwargs.pop("weight_hdu")] * len(image_hdu_list)
            else:
                weight_hdu_list = weight.hdu_list
            weight_file_list = [weight] * len(image_hdu_list)

        image_list = []
        it = zip(image_hdu_list, psf_file_list, psf_hdu_list, weight_file_list, weight_hdu_list)
        for hdu, psf_file, psf_hdu, weight_file, weight_hdu in it:
            image_list.append(MeasurementImage(image_file, psf_file, weight_file,
                                               image_hdu=hdu, psf_hdu=psf_hdu,
                                               weight_hdu=weight_hdu,
                                               **kwargs))
        for img in image_list:
            self._measurement_images[img.id] = img
        return ImageGroup(images=image_list)

    # TODO: better typing for kwargs passed through to DataCubeSlice
    def load_fits_images(self, images: str | list[str], psfs: list[str] | None = None,
                         weights: list[str] | None = None, **kwargs: Any) -> ImageGroup:
        """Creates an image group for the given images.

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

        if isinstance(images, list):
            if len(images) == 0:
                raise ValueError("An empty list passed to load_fits_images")

            if not isinstance(psfs, list) or len(psfs) != len(images):
                raise ValueError("The number of image files and psf files must match!")

            if not isinstance(weights, list) or len(weights) != len(images):
                raise ValueError("The number of image files and weight files must match!")

            groups = []
            for f, p, w in zip_longest(images, psfs, weights):
                groups.append(self.load_fits_image(f, p, w, **kwargs))

            image_list: list[MeasurementImage] = []
            for g in groups:
                # Here too, g could be an Iterable[ImageGroup] or Iterable[MeasurementImage]
                # depending on whether it's a leaf node.  By construction, self.load_fits_image
                # above always returns a leaf ImageGroup, but this is not captured in the typing.
                # Would be better to split ImageGroup into distinct classes for leaf and non-leaf
                # cases.  For now explicitly cast to the known correct type
                image_list += cast(Iterable[MeasurementImage], g)
            for img in image_list:
                self._measurement_images[img.id] = img
            return ImageGroup(images=image_list)
        else:
            return self.load_fits_image(images, psfs, weights, **kwargs)

    # TODO: better typing for kwargs passed through to DataCubeSlice
    def load_fits_data_cube(self, image: str, psf: str | list[str] | None = None,
                            weight: str | list[FitsFile] | None = None, image_cube_hdu: int = -1,
                            weight_cube_hdu: int = -1, **kwargs: Any) -> ImageGroup:
        """
        Creates an image group with the images of a data cube.

        weight can be a matching datacube, multi-hdu or list of individual images
        psf can be a multi-hdu or list of individual images to be matched

        In any case, they are matched in order and HDUs not containing images are ignored.

        :param image: The filename of the FITS file containing the image datacube
        :param psf: psf file or list of psf files
        :param weight: FITS file contianing a weight datacube, a MEF containing the weights
             or a list of such files
        :param image_cube_hdu: hdu containing the image datacube, default = first HDU containing image data
        :param weight_cube_hdu: hdu containing the weight datacube, default = first HDU containing image data

        :return: A ImageGroup representing the images
        """

        image_cube_file = FitsFile(image)

        if image_cube_hdu < 0:
            cube_hdu = image_cube_file.hdu_list[0]
        else:
            cube_hdu = image_cube_hdu

        dims = image_cube_file.get_dimensions(cube_hdu)

        if len(dims) != 3:
            raise ValueError("Not a data cube!")

        cube_size = dims[2]
        image_layer_list = range(cube_size)
        psf_hdu_list: Iterable[int]
        psf_file_list: Iterable[str] | Iterable[None]
        weight_hdu_list: Iterable[int]
        weight_layer_list: Iterable[int]
        weight_file_list: Iterable[FitsFile] | Iterable[None]

        # handles the PSFs
        if isinstance(psf, list):
            if len(psf) != cube_size:
                raise ValueError("The number of psf files must match the number of images!")
            psf_file_list = psf
            psf_hdu_list = repeat(0, cube_size)
        elif psf is None:
            psf_file_list = repeat(None, cube_size)
            psf_hdu_list = repeat(0, cube_size)
        else:
            psf_file_list = repeat(psf, cube_size)
            psf_hdu_list = range(cube_size)

        # handles the weight maps
        if isinstance(weight, list):
            if len(weight) != cube_size:
                raise ValueError("The number of weight files must match the number of images!")
            weight_file_list = weight
            weight_hdu_list = repeat(0, cube_size)
            weight_layer_list = repeat(0, cube_size)
        elif weight is None:
            weight_file_list = repeat(None, cube_size)
            weight_hdu_list = repeat(0, cube_size)
            weight_layer_list = repeat(0, cube_size)
        else:
            weight_fits_file = FitsFile(weight)
            if weight_cube_hdu < 0:
                weight_hdu = weight_fits_file.hdu_list[0]
            else:
                weight_hdu = weight_cube_hdu

            weight_dims = weight_fits_file.get_dimensions(weight_hdu)
            if len(weight_dims) == 3:
                # handle weight as data cube
                if dims[2] != cube_size:
                    raise ValueError("The weight map cube doesn't match the image cube")

                weight_file_list = repeat(weight_fits_file, cube_size)
                weight_hdu_list = repeat(weight_hdu, cube_size)
                weight_layer_list = range(cube_size)
            else:
                # weight is a FITS without a datacube, assume MEF
                weight_file_list = repeat(weight_fits_file, cube_size)
                weight_hdu_list = weight_fits_file.hdu_list
                weight_layer_list = repeat(0, cube_size)

        image_list = []
        for psf_file, psf_hdu, weight_file, weight_hdu, image_layer, weight_layer in zip(
                psf_file_list, psf_hdu_list, weight_file_list, weight_hdu_list, image_layer_list,
                weight_layer_list):
            image_list.append(DataCubeSlice(image_cube_file, psf_file, weight_file,
                                            image_hdu=cube_hdu, psf_hdu=psf_hdu,
                                            weight_hdu=weight_hdu,
                                            image_layer=image_layer, weight_layer=weight_layer,
                                            **kwargs))

        for img in image_list:
            self._measurement_images[img.id] = img
        return ImageGroup(images=image_list)

    def print_measurement_images(self, file: IO[str] | None = None) -> None:
        """
        Print a human-readable representation of the configured measurement images.

        Parameters
        ----------
        file : file object
            Where to print the representation. Defaults to sys.stderr
        """
        file = file or sys.stderr
        print('Measurement images:', file=file)
        for i in self._measurement_images:
            im = self._measurement_images[i]
            print(f'Image {i}', file=file)
            print(f'      File: {im.file}', file=file)
            print(f'       PSF: {im.psf_file}', file=file)
            print(f'    Weight: {im.weight_file}', file=file)

    def add_aperture_photometry(self, target: Target, apertures: float | list[float]) -> list[cpp.Aperture]:
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
        >>> context = MeasurementConfig()
        >>> measurement_group = MeasurementGroup(load_fits_images(frames, psfs))
        >>> all_apertures = []
        >>> for img in measurement_group:
        >>>     all_apertures.extend(context.add_aperture_photometry(img, [5, 10, 20]))
        >>> context.add_output_column('aperture', all_apertures)
        """
        if isinstance(target, list):
            target_list = target
        else:
            target_list = cast(list[MeasurementImage] | list[MeasurementGroup], [target])

        if isinstance(apertures, list):
            apertures_list = apertures
        else:
            apertures = [apertures]

        apertures = [float(a) for a in apertures]
        outputs = []

        for t in target_list:
            if isinstance(t, MeasurementGroup):
                if not t.is_leaf():
                    raise Exception('The MeasurementGroup is not a leaf')
                elif len(t) != 1:
                    raise Exception(f'The MeasurementGroup contains {len(t)} images')
                img = next(iter(t))
            else:
                img = t

            if not isinstance(img, MeasurementImage):
                raise Exception(
                    f'Only MeasurementImage supported as targets, got {type(img)}')
            else:
                if img.id in self._apertures_for_image:
                    raise Exception(f'Apertures already set for the image {img.id}')
                self._apertures_for_image[img.id] = cpp.Aperture(apertures_list)
                outputs.append(self._apertures_for_image[img.id])

        return outputs

    def print_output_columns(self, file: IO[str] | None = None) -> None:
        """
        Print a human-readable representation of the configured output columns.

        Parameters
        ----------
        file : file object
            Where to print the representation. Defaults to sys.stderr
        """
        file = file or sys.stderr

        if self._model_fitting_parameter_columns:
            print('Model fitting parameter outputs:', file=file)
            for name, param_ids in self._model_fitting_parameter_columns:
                print(f'    {name} : {param_ids}', file=file)

        if self._aperture_columns:
            print('Aperture outputs:', file=file)
            for name, aper_ids in self._aperture_columns:
                print(f'    {name} : {aper_ids}', file=file)

    def add_output_column(self, name: str, params: OutputColumn) -> None:
        """
        Add a new set of columns to the output catalog.

        Parameters
        ----------
        name : str
            Name/prefix of the new set of columns
        params : list of columns
            Homogenous list of properties to add to the output with the given name/prefix. They
            must be subtype of one of the known ones: ParameterBase for model fitting, or Aperture
            for aperture photometry.

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
        if name in self._used_names:
            raise ValueError(f'Column {name} is already set')
        self._used_names.add(name)

        if isinstance(params, list):
            params_list = params
        else:
            params_list = cast(list[FittableParameter] | list[cpp.Aperture], [params])

        # TODO: Should check that all items are the same param type; could be implemented
        # as part of a type guard.
        param_type = type(params_list[0])

        known_subclass = False
        for base in self._column_map:
            if issubclass(param_type, base):
                # TODO: This cast could be avoided with a little judicious
                # refactoring I think, but for now it's fine.
                cols = cast(list[tuple[str, list[Any]]], self._column_map[base])
                cols.append((name, params_list))
                known_subclass = True
        if issubclass(param_type, ParameterBase):
            self.model_fitting._register_parameter(cast(FittableParameter, params_list[0]))

        if not known_subclass:
            raise TypeError(f'{param_type} is not a known column type')


global_measurement_config = MeasurementConfig()
