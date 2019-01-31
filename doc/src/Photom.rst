.. File Photom.rst

.. include:: global.rst

.. _photometry:

Aperture photometry
===================

Besides :ref:`isophotal <flux_iso_def>`, |PSF| and :ref:`model-fitting <models_def>` flux estimates, |SExtractor| can currently perform two types of flux measurements: :ref:`fixed-aperture <flux_aper_def>` and :ref:`adaptive-aperture <flux_auto_def>`.

.. _flux_aper_def:

Fixed-aperture flux
-------------------

.. _flux_auto_def:

Automatic aperture flux
-----------------------

|SExtractor|’s automatic aperture photometry routine derives from Kron’s “first moment” algorithm :cite:`1980ApJS_43_305K`:


:cite:`1980ApJS_43_305K` and :cite:`1987AA_183_177I` have shown that for stars and galaxy profiles convolved with Gaussian seeing, :math:`\ge 90\%` of the flux is expected to lie inside a circular aperture of radius :math:`k r_{\rm Kron}` with :math:`k = 2`, almost independently of the magnitude.
Experiments have shown :cite:`1996AAS_117_393B` that this conclusion remains unchanged if one replaces the circular aperture with the "Kron elliptical aperture" :math:`{\cal K}` with reduced pseudo-radius :math:`k r_{\rm Kron}`.


.. _background_def:

Local background
----------------

Almost all |SExtractor| measurements are done using background-subtracted pixel values.
In crowded fields, or in images where the background is irregular, the :ref:`background model <background_model>` may be significantly inaccurate, locally creating biases in photometric estimates.

