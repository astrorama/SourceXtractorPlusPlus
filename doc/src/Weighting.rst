.. File Weighting.rst

.. include:: global.rst

Weighting
=========

The noise level in astronomical images is often fairly constant, that is, constant values for the gain, the background noise and the detection thresholds can be used over the whole frame.
Unfortunately in some cases, like strongly vignetted or composited images, this approximation is no longer good enough.
This leads to detecting clusters of detected noise peaks in the noisiest parts of the image, or missing obvious objects in the most sensitive ones.
|SExtractor++| is able to handle images with variable noise.
It does it through *weight maps*, which are frames having the same size as the images where objects are detected or measured, and which describe the noise intensity at each pixel.
These maps are internally stored in units of *absolute variance* (in :math:`ADU^2`)
We employ the generic term *weight map* because these maps can also be interpreted as quality index maps: infinite variance (:math:`\ge 10^{30}` by definition in |SExtractor++|) means that the related pixel in the science frame is totally unreliable and should be ignored.
The variance format was adopted as it linearizes most of the operations done over weight maps (see below).

This means that the noise covariances between pixels are ignored. Although raw CCD images have essentially white noise, this is not the case for warped images, for which resampling may induce a strong correlation between neighboring pixels.
In theory, all non-zero covariances within the geometrical limits of the analysed patterns should be taken into account to derive thresholds or error estimates.
Fortunately, the correlation length of the noise is often smaller than the patterns to be detected or measured, and constant over the image.
In that case one can apply a simple *fudge factor* to the estimated variance to account for correlations on small scales.
This proves to be a good approximation in general, although it certainly leads to underestimations for the smallest patterns.

.. _weight-map_format:

Weight-map formats
------------------

|SExtractor++| accepts in input, and converts to its internal variance format, several types of weight-maps.

.. _effect_of_weighting:

Effect of weighting
-------------------

Combining weight maps
---------------------

Weight-map flags
----------------


