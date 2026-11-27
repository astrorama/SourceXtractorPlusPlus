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
from __future__ import annotations

import sys
from collections.abc import Sequence
from typing import Any, IO, TypeAlias, cast, overload

import _SourceXtractorPy as cpp

from .measurement_images import (
    AuxFile, DataCubeSlice, FitsFile, ImageGroup, MeasurementGroup,
    MeasurementImage, Psf, Weight, _filename_of)
from .model_fitting import FittableParameter, ModelFitting, ParameterBase


OutputColumn: TypeAlias = 'FittableParameter | cpp.Aperture | list[FittableParameter] | list[cpp.Aperture]'
# TODO: It may be possible to implement a special type just for leaf
# nodes, but in the meantime the is_leaf() requirement is a runtime check
Target: TypeAlias = MeasurementImage | MeasurementGroup | list[MeasurementImage] | list[MeasurementGroup]


class MeasurementConfig:
    """The measurement configuration manager"""

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
            for member in image.images():
                self.add_measurement_image(member)
        else:
            for _, subgroup in image.subgroups():
                self.add_measurement_image(subgroup)

    # TODO: better typing for kwargs passed through to DataCubeSlice
    def load_fits_image(self, image: str, psf: str | list[str] | None = None,
                        weight: AuxFile | Sequence[AuxFile] | None = None,
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
        if "image_hdu" in kwargs:
            image_hdus = [kwargs.pop("image_hdu")]
        else:
            image_hdus = image_file.hdu_list

        psfs = Psf.broadcast(psf, len(image_hdus), kwargs.pop("psf_hdu", None))
        weights = Weight.broadcast(weight, len(image_hdus),
                                   kwargs.pop("weight_hdu", None))

        image_list = [
            MeasurementImage(image_file, psf_aux.path, weight_aux.file,
                             image=hdu, psf_hdu=psf_aux.hdu,
                             weight=weight_aux.selector, **kwargs)
            for hdu, psf_aux, weight_aux
            in zip(image_hdus, psfs, weights, strict=True)
        ]

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

            psf_list = Psf.one_per_image(psfs, len(images))
            weight_list = Weight.one_per_image(weights, len(images))

            groups = []
            for f, psf_aux, weight_aux in zip(images, psf_list, weight_list,
                                              strict=True):
                groups.append(self.load_fits_image(f, psf_aux.path,
                                                   weight_aux.file, **kwargs))

            image_list: list[MeasurementImage] = []
            for g in groups:
                image_list += g.images()
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

        cube_size = _cube_depth(image_cube_file, cube_hdu)
        weights = Weight.broadcast(
            weight, cube_size,
            weight_cube_hdu if weight_cube_hdu >= 0 else None)
        psfs = Psf.broadcast(psf, cube_size)

        image_list = [
            DataCubeSlice(image_cube_file, psf_aux.path, weight_aux.file,
                          image=cube_hdu, psf_hdu=psf_aux.hdu,
                          weight=weight_aux.selector, image_layer=layer,
                          weight_layer=weight_aux.layer, **kwargs)
            for layer, psf_aux, weight_aux
            in zip(range(cube_size), psfs, weights, strict=True)]

        for img in image_list:
            self._measurement_images[img.id] = img

        return ImageGroup(images=image_list)

    # TODO: better typing for kwargs passed through to MeasurementImage
    def load_image(self, measurement_image: MeasurementImage | str | cpp.ImageFile,
                   psf_file: str | None = None,
                   weight_file: str | cpp.ImageFile | None = None,
                   **kwargs: Any) -> ImageGroup:
        """Creates an image group holding one already-described image.

        A `MeasurementImage` says everything about one image: which file, which
        image within it, its weight and PSF, its gain. Pass one, and this only
        registers it. Anything else is taken as the arguments to build one,
        which is a convenience rather than the point of the interface.

        Which image within the file is said with the `image` selector, an index
        or a name; left out it means the file's only image, or the first of
        several. An ASDF file holding more than one array that could be an
        image has to be named, since nothing in such a file says which is
        meant.

        Parameters
        ----------
        measurement_image : MeasurementImage or str or ImageFile
            The image, or a file to build one from
        psf_file : str
            A PSF file: a FITS image or a PSFEx model
        weight_file : str or ImageFile
            A file containing the pixel weights
        **kwargs
            Everything `MeasurementImage` takes, `image`, `weight`, `wcs` and
            the gain and saturation settings included

        Returns
        -------
        ImageGroup
            A group holding the single image

        Raises
        ------
        TypeError
            If a `MeasurementImage` is given along with arguments to build one

        See Also
        --------
        load_images : several already-described images
        data_cube_slices : one image per layer of a data cube
        load_fits_image : every image HDU of one FITS file
        """

        image = _as_measurement_image(measurement_image, psf_file,
                                      weight_file, kwargs)
        self._measurement_images[image.id] = image
        return ImageGroup(images=[image])

    # TODO: better typing for kwargs passed through to MeasurementImage
    def load_images(self,
                    measurement_images: Sequence[MeasurementImage] | str | list[str],
                    psf_files: list[str] | None = None,
                    weight_files: list[str] | None = None,
                    **kwargs: Any) -> ImageGroup:
        """Creates an image group out of several already-described images.

        Pass `MeasurementImage` objects and this only registers them, which is
        how images that differ from one another are described -- each carries
        its own selector, weight and gain. Passing filenames instead builds one
        image from each, with everything in `kwargs` applying to all of them,
        so a selector given that way has to name an image every file holds.

        Parameters
        ----------
        measurement_images : sequence of MeasurementImage, or str, or list of str
            The images, or files to build one from each
        psf_files : list of str
            PSF files, one per image, or None
        weight_files : list of str
            Weight files, one per image, or None

        Returns
        -------
        ImageGroup
            A group holding one image per entry

        Raises
        ------
        ValueError
            If a list of psf or weight files does not match the number of images

        See Also
        --------
        load_image : one image
        data_cube_slices : one image per layer of a data cube
        """

        entries = (measurement_images
                   if isinstance(measurement_images, (list, tuple))
                   else [measurement_images])
        if len(entries) == 0:
            raise ValueError("An empty list passed to load_images")

        psf_list = Psf.one_per_image(psf_files, len(entries))
        weight_list = Weight.one_per_image(weight_files, len(entries))

        image_list: list[MeasurementImage] = []
        for entry, psf_aux, weight_aux in zip(entries, psf_list, weight_list,
                                              strict=True):
            image_list.append(_as_measurement_image(entry, psf_aux.path,
                                                    weight_aux.file, dict(kwargs)))

        for image in image_list:
            self._measurement_images[image.id] = image

        return ImageGroup(images=image_list)

    # TODO: better typing for kwargs passed through to DataCubeSlice
    def load_data_cube(self, image_file: str,
                       psf_file: str | None = None,
                       weight_file: str | cpp.ImageFile | None = None,
                       **kwargs: Any) -> ImageGroup:
        """Creates an image group with the layers of a data cube.

        A convenience for ``load_images(data_cube_slices(...))``; see
        `data_cube_slices` for what the layers become and how the weights are
        matched to them.

        Returns
        -------
        ImageGroup
            A group holding one image per layer

        See Also
        --------
        data_cube_slices : the slices this loads, which can be built and
            adjusted individually instead
        """

        return self.load_images(
            data_cube_slices(image_file, psf_file, weight_file, **kwargs))

    def print_measurement_images(self, file: IO[str] | None = None) -> None:
        """
        Print a human-readable representation of the configured measurement images.

        Parameters
        ----------
        file : file object
            Where to print the representation. Defaults to `sys.stderr`
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
        target : MeasurementImage, MeasurementGroup, or list
            Target images on which to measure the aperture photometry. Leaf MeasurementGroup with a single image
            are accepted as a convenience, and may be a list of either.

        apertures : float or list of float
            Diameter of the aperture. As different MeasurementImage may not be aligned, nor have equivalent pixel size,
            the aperture is interpreted as diameter in pixels of a circle on the detection image.
            A transformation will be applied for each frame, so the covered area is equivalent.

        Returns
        -------
        list[Aperture]
            An `Aperture` object is an internal representation of a property on the measurement frame that contains the
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
            Where to print the representation. Defaults to `sys.stderr`
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
"""Single `MeasurementConfig` instance used by the global functions"""


def data_cube_slices(image_file: str, psf_file: str | list[str] | None = None,
                     weight_file: str | cpp.ImageFile | list[str] | None = None,
                     **kwargs: Any) -> list[DataCubeSlice]:
    """One `DataCubeSlice` per layer of a data cube, of any supported format.

    An array of three dimensions is a data cube whichever file it came from,
    its slowest-changing axis being the layers, and each layer is measured as
    a separate image.

    The weights are a sequence of arrays matched in order to the layers: a list
    of files, one file holding a matching cube, or one file holding as many
    arrays. One file holding a single array is shared by every layer. Layers
    needing weights unrelated to each other are described by building the
    slices individually instead.

    Parameters
    ----------
    image_file : str
        The file containing the image cube
    psf_file : str or list of str
        A PSF file shared by every layer, or one per layer
    weight_file : str, ImageFile, or list of str
        Where the weights come from, in any of the shapes above
    **kwargs
        Everything `DataCubeSlice` takes, the `image` and `weight` selectors
        included

    Returns
    -------
    list of DataCubeSlice
        One per layer, in order

    Raises
    ------
    ValueError
        If the selected array is not a data cube, or if the weights or PSFs do
        not amount to one per layer.

    See Also
    --------
    MeasurementConfig.load_data_cube : loads these in one call
    """

    kwargs.setdefault('fits_compat', False)
    cube_image = cpp.ImageFile(_filename_of(image_file))
    depth = _cube_depth(cube_image, kwargs.get('image'))
    weights = Weight.broadcast(weight_file, depth, kwargs.pop('weight', None))
    psfs = Psf.broadcast(psf_file, depth, kwargs.pop('psf_hdu', None))

    return [
        DataCubeSlice(image_file, psf_aux.path, weight_aux.file,
                      image_layer=layer, psf_hdu=psf_aux.hdu,
                      weight=weight_aux.selector,
                      weight_layer=weight_aux.layer, **kwargs)
        for layer, psf_aux, weight_aux
        in zip(range(depth), psfs, weights, strict=True)
    ]


def _as_measurement_image(image: MeasurementImage | str | cpp.ImageFile,
                          psf_file: str | None,
                          weight_file: str | cpp.ImageFile | None,
                          kwargs: dict[str, Any]) -> MeasurementImage:
    """
    Take an already-described image, or build one from the arguments

    Raises
    ------
    TypeError
        If an image is given along with arguments that would build one, since
        it already says everything they would.
    """

    if not isinstance(image, MeasurementImage):
        # A file read through this interface is not given the FITS-specific
        # behaviour that MeasurementImage keeps for the load_fits_* functions
        kwargs.setdefault('fits_compat', False)
        return MeasurementImage(image, psf_file, weight_file, **kwargs)

    if psf_file is not None or weight_file is not None or kwargs:
        raise TypeError(f'{type(image).__name__} already describes its image; '
                        f'pass either one of those or the arguments to build it')

    return image


def _cube_depth(image_file: cpp.ImageFile, selector: Any) -> int:
    """
    How many layers the selected array has, which has to be a data cube

    Takes an open file so that a caller already holding one need not reopen it.

    Raises
    ------
    ValueError
        If the selected array is not three-dimensional.
    """

    dims = image_file.get_dimensions(selector if selector is not None else 0)

    if not dims.is_cube:
        raise ValueError("Not a data cube!")

    return dims.depth
