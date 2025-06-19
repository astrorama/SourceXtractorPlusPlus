.. File Background.rst

.. include:: global.rst

.. _chap_background:

Modeling the background
=======================

On linear detectors, the value measured at each pixel is the sum of a "background" signal and light coming from the sources of interest.
To be able to detect the faintest objects and make accurate measurements, |SourceXtractor++| needs first computing a precise estimate of the background level at any position of the image: a *background map*.
Strictly speaking, there should be one background map per source, that is, what would the image look like if that very source was missing.
However, one can start by assuming that most discrete sources do not overlap too severely — which is generally the case for high galactic latitude fields, and that the background varies smoothly across the field. |SourceXtractor++|'s current background model is essentially the same as that of |SExtractor|_ :cite:`1996AAS_117_393B`.

Background estimation
---------------------

To compute the background map, |SourceXtractor++| makes a first pass through the pixel data, estimating the local background in each cell of a square grid that covers the whole frame.
The background estimator is a combination of :math:`\kappa\,\sigma` clipping and mode estimation, similar to Stetson’s |DAOPHOT|_ program :cite:`1987PASP_99_191S,1992ASPC_23_90D`.

Briefly, the local background histogram is clipped iteratively until convergence at :math:`\pm 3\sigma` around its median. The mode of the histogram is estimated using:

.. math::
  :label: sexbackmode

  \mbox{Mode} = 2.5 \times \mbox{Median} - 1.5 \times \mbox{Mean}.

Using simulated images, the expression above was found more accurate with clipped distributions :cite:1996AAS_117_393B than the usual approximation (e.g., :cite:`stuart2009kendall`):

.. math::
  :label: ksbackmode

  \mbox{Mode} = 3 \times \mbox{Median} - 2 \times \mbox{Mean}.

:numref:`fig_modevsmean` shows that the mode estimation in :eq:`sexbackmode` is considerably less affected by source crowding than a simple clipped mean :cite:`1981AJ_86_476J,1987AA_183_177I` but it is :math:`\approx 30\%` noisier.
Obviously :eq:`sexbackmode` is not valid for any distribution; |SourceXtractor++| falls back to a simple median for estimating the local background value if the mode and the median disagree by more than 30%.

.. _fig_modevsmean:

.. figure:: figures/modevsmean.*
   :figwidth: 100%
   :align: center

   Simulations of 32×32 pixels background cells contamined by random Gaussian profiles.
   The true background lies at 0 ADUs.
   While being a bit noisier, the clipped "mode" gives a more robust estimate than the clipped mean in crowded regions.

.. _chap_backfilter:

Background map
--------------

Once the values of all background cells have been estimated, a median box filter is applied to suppress possible local overestimations due to bright stars.
Median filtering helps reducing possible ringing effects of the bicubic-spline around bright features.
The final background map is a (natural) bicubic-spline interpolation between grid cells.

In parallel with the making of the background map, a *background noise map*, that is, a map of the background noise standard deviation is produced.
It is used as an internal weight map when the ``weight-type`` configuration parameter is set to ``weight-type=background``, which is the default.

Configuration and tuning
------------------------

.. note::
  All background configuration parameters also affect background noise maps.

The choice of the ``background-cell-size`` is very important.
If it is too small, the background estimation is affected by the presence of
objects and random noise.
Most importantly, part of the flux of the most extended objects can be absorbed into the background map.
If the cell size is too large, it cannot reproduce the small scale variations of the background.
Therefore a good compromise must be found by the user.
For reasonably sampled images, cell sizes between 32 to 512 pixels are generally appropriate.

The user has some control over background map filtering by specifying the size of the median filter box .
The ``smoothing-box-size`` configuration parameter sets the box size in pixels.
``smoothing-box-size=3`` is the default and is sufficient in most cases.
``smoothing-box-size=1`` deactivates filtering.
Larger dimensions may occasionally be used to compensate for small background cell sizes, or in the presence of large image artifacts.

By default, the computed background maps are automatically subtracted from input images.
However there are situations where subtracting a *constant* from the detection image may be more relevant, e.g., for images with strongly non-Gaussian background noise |pdf|_\ s).
The ``background-value`` configuration parameter may be set to subtract a specific value from the input detection image, instead of the background map.
It is unset by default.

Computing cost
--------------

The background estimation operation is generally |I/O|\ -bound, unless the image file already resides in the disk cache.

