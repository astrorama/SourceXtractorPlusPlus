.. File Photom.rst

.. include:: global.rst

.. _photometry:

Aperture photometry
===================

Besides :ref:`isophotal <flux_iso_def>`, |PSF| and :ref:`model-fitting <models_def>` flux estimates, |SExtractor++| can currently perform two types of aperture flux measurements: :ref:`fixed-aperture <flux_aper_def>` and :ref:`adaptive-aperture <flux_auto_def>`.

Both types are applied to the measurement image(s). In |SE++| runs with only a detection image it is necessary to define the detection image as measurement in a python configuration files such as: ::

   from glob import glob
   import numpy as np
   from sourcextractor.config import *

   top = load_fits_image(
      '../EUC_MER_BGSUB-MOSAIC-VIS_TILE79171-SM.fits',
      weight='../EUC_MER_MOSAIC-VIS-RMS_TILE79171-SM.fits',
      weight_type='rms',
      constant_background = 0.0,
      weight_absolute=1
   )

   mesgroup = MeasurementGroup(top)

   all_apertures = []
   for img in mesgroup:
      all_apertures.extend(add_aperture_photometry(img, 10 ) )
   add_output_column('aperture', all_apertures)

.. _flux_aper_def:

Fixed-aperture flux
-------------------

.. _flux_auto_def:

The fixed aperture flux measurements are requested with ``output-properties=...,AperturePhotometry,...``. The diameter of the aperture is specified in the python configuration file (see above) with ``all_apertures.extend(add_aperture_photometry(img,<diameter [pix]>))``. It is possible to give a vector with several diameters ``[diam_1, diam_2, diam_3, ...]``.

It is also necessary to append the aperture measurements to the output with a given column name such as ``add_output_column('aperture', all_apertures)``. The measurements have the dimension ``n x m`` for each object with ``n`` the number of measurement images and ``m`` the number of diameters. 

The fixed aperture checkimage is specified with ``check-image-aperture=<name.fits>`` and provides a visual impression of the apertures for each measurement image.

Automatic aperture flux
-----------------------

|SExtractor++|’s automatic aperture photometry routine derives from Kron’s “first moment” algorithm :cite:`1980ApJS_43_305K`:


:cite:`1980ApJS_43_305K` and :cite:`1987AA_183_177I` have shown that for stars and galaxy profiles convolved with Gaussian seeing, :math:`\ge 90\%` of the flux is expected to lie inside a circular aperture of radius :math:`k r_{\rm Kron}` with :math:`k = 2`, almost independently of the magnitude.
Experiments have shown :cite:`1996AAS_117_393B` that this conclusion remains unchanged if one replaces the circular aperture with the "Kron elliptical aperture" :math:`{\cal K}` with reduced pseudo-radius :math:`k r_{\rm Kron}`.

Automatic aperture flux measurements are requested with ``output-properties=...,AutoPhotometry,...``. The scale factor :math:`k` for the Kron radius :math:`r_{\rm Kron}` and the minimal Kron radius :math:`r_{\rm Kron, min}` can be adjusted with the parameters ``auto-kron-factor=`` and ``auto-kron-min-radius=``, respectively. The measurements have the dimension ``n`` for each object with ``n`` the number of measurement images.

The automatic aperture checkimage is specified with ``check-image-auto-aperture=<name.fits>`` and provides a visual impression of the automatic apertures for each measurement image.


.. _background_def:

..
  I think we dont have the local background, so from here onit it pointless.
  Local background
  ----------------

..
  Almost all |SExtractor++| measurements are done using background-subtracted pixel values.
  In crowded fields, or in images where the background is irregular, the :ref:`background model <background_model>` may be significantly inaccurate, locally creating biases in photometric estimates.

