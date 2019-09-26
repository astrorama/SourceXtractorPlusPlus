# -*- coding: utf-8 -*-

# Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
from __future__ import division, print_function
from .measurement_images import MeasurementImage, MeasurementGroup

import _SExtractorPy as cpp

apertures_for_image = {}


Aperture = cpp.Aperture


def add_aperture_photometry(target, apertures):
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
    if not isinstance(target, list):
        target = [target]
    if not isinstance(apertures, list):
        apertures = [apertures]

    apertures = [float(a) for a in apertures]
    outputs = []

    for t in target:
        if isinstance(t, MeasurementGroup):
            if not t.is_leaf():
                raise Exception('The MeasurementGroup is not a leaf')
            elif len(t) != 1:
                raise Exception('The MeasurementGroup contains {} images'.format(len(t)))
            t = [i for i in t][0]

        if not isinstance(t, MeasurementImage):
            raise Exception('Only MeasurementImage supported as targets, got {}'.format(type(t)))
        else:
            if t.id in apertures_for_image:
                raise Exception('Apertures already set for the image {}'.format(t.id))
            apertures_for_image[t.id] = cpp.Aperture(apertures)
            outputs.append(apertures_for_image[t.id])

    return outputs
