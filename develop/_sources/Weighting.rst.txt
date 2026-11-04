.. File Weighting.rst

.. include:: global.rst

.. _chap_weighting:

Weighting
=========

The noise level in astronomical images is often fairly constant, that is, constant values for the gain, the background noise and the detection thresholds can be used over the whole frame.
Unfortunately in some cases, like strongly vignetted or composited images, this approximation is no longer good enough.
This leads to detecting clusters of detected noise peaks in the noisiest parts of the image, or missing obvious objects in the most sensitive ones.
|SourceXtractor++| is able to handle images with variable noise.
It does it through *weight maps*, which are frames having the same size as the images where objects are detected or measured, and which describe the noise intensity at each pixel.
These maps are internally stored in units of *absolute variance* (in :math:`ADU^2`).
We employ the generic term *weight map* because these maps can also be interpreted as quality index maps: infinite variance (:math:`\ge 10^{30}` by definition in |SourceXtractor++|) means that the related pixel in the science frame is totally unreliable and should be ignored.

The variance format was adopted as it linearizes most of the operations done over weight maps (see below).
However noise covariances between pixels are ignored.

Effect of weighting
-------------------

Weight-maps modify the working of |SourceXtractor++| in the several ways:

- The detection threshold :math:`t` above the local sky background in the detection image is adjusted for each pixel :math:`i` with variance :math:`\sigma^2_i`: :math:`t_i=`\ ``detection-threshold``\ :math:`\times\sqrt{\sigma^2_i}`, where ``detection-threshold`` is expressed in units of standard deviations of the background noise.
  Pixels having a variance *above* the threshold set with the ``--weight-threshold`` command line option are never detected.
  This may result in splitting objects crossed by a group of bad pixels.
  Interpolation should be used to avoid this problem.
  If convolution filtering is applied for detection then the variance map is also convolved.

- The cleaning process takes into account the exact individual thresholds assigned to each pixel before deciding whether a faint detections must be merged with a larger one.

- Bad pixels are discarded from the background statistics.
  If more than :math:`50\%` of the pixels in a background cell are bad, then the background and the standard deviation values are discarded and interpolated from the nearest valid cells.

- Measurement uncertainties assume that the standard deviation of the local background noise is simply :math:`\sqrt{\sigma^2_i}`.

- Detection image pixels with weights beyond ``weight-threshold`` (above or below depending on the ``--weight-type`` option argument, see below) are discarded.

.. In addition, if the ``WEIGHT_GAIN`` parameter is set to ``Y`` (which is the default), it is assumed that the local pixel gain (i.e., the conversion factor from photo-electrons to ADUs) is inversely proportional to :math:`\sigma^2_i`, the median value over the image being set by the ``GAIN`` configuration parameter.
   In other words, it is then supposed that the changes in noise intensities seen over the images are due to gain changes.
   This is the most common case: correction for vignetting, or coverage depth.
   When this is not the case, for instance when changes are purely dominated by those of the read-out noise, ``WEIGHT_GAIN`` shall be set to ``N``.

  By default, all weight-map values are assumed to be relative values: a robust scaling to the appropriate absolute level is performed internally by comparing the variance-map values in the input weight-map to those of a low-resolution variance map built from the detection image itself.
  It is possible to deactivate this internal rescaling in the detection weight-map by setting the ``--weight-scaling`` option to 0.

.. caution::

  Although raw |CCD|_ exposures have essentially white noise, this is not the case for the resampled images that enter image stacks; resampling induces a significant amount of noise correlation between neighboring pixels.
  Despite the fact that the correlation length is often smaller than the patterns to be detected or measured, and constant over the image, noise correlations bias the weight rescaling process.
  It is therefore recommended to deactivate the weight-map rescaling when working with resampled image and use weight-maps with absolute values whenever possible.


.. _weight-map_format:

Weight-map formats
------------------

Detection weight-map
~~~~~~~~~~~~~~~~~~~~

|SourceXtractor++| reads and converts to its internal variance format several types of weight-maps.
The detection weight-map can either be read from a FITS file specified by the ``--weight-image`` command line option, or computed internally.
The type of detection weight-map must be selected using the ``--weight-type`` command-line option.
Valid ``weight_type`` values are:

* ``background`` :
    The science image itself is used to compute internally a variance map (the ``--weight-type`` command-line option is ignored).
    Robust (:math:`3\sigma`-clipped) variance estimates are first computed within the same background cells as the :ref:`background model <chap_background>` [#backfilter]_.
    The resulting low-resolution variance map is then bicubic-spline-interpolated on the fly to produce the actual full-size variance map.

* ``rms`` :
    The |FITS| image specified using the ``--weight-image`` option is a weight-map in units of standard deviations (in ADUs per pixel).

* ``variance`` :
    The |FITS| image specified using the ``--weight-image`` option is a weight-map in units of relative variance.

* ``weight`` :
    The |FITS| image specified using the ``--weight-image`` option is a weight-map in units of inverse variance.


Measurement weight-map
~~~~~~~~~~~~~~~~~~~~~~

Measurement weight-maps are managed identically to detection weight-maps, except that configuration must be done through the :func:`~config.measurement_images.load_fits_image()` command options in the :ref:`measurement configuration script <chap_configuration_script>`, instead of command line options.

.. Weight-map flags
  ----------------

.. [#backfilter] The :ref:`background map filtering procedure <chap_backfilter>` is also applied to the variance map.

