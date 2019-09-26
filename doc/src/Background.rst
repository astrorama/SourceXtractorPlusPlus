.. File Background.rst

.. include:: global.rst

.. _background_model:

Modeling the background
=======================

On linear detectors, the value measured at each pixel is the sum of a "background" signal and light coming from the sources of interest.
To be able to detect the faintest objects and make accurate measurements, |SExtractor++| needs first computing a precise estimate of the background level at any position of the image: a *background map*.
Strictly speaking, there should be one background map per source, that is, what would the image look like if that very source was missing.
But, at least for detection, one can start by assuming that most discrete sources do not overlap too severely — which is generally the case for high galactic latitude fields —, and that the background varies smoothly across the field.

Background estimation
---------------------

Background map
--------------

Configuration and tuning
------------------------

Computing cost
--------------

The background estimation operation is generally |I/O|\ -bound, unless the image file already resides in the disk cache.


