.. File Input.rst

.. include:: global.rst

.. _input:

Input files
-----------

|SExtractor++| accepts images stored in |FITS|_ :cite:`1981AAS_44_363W`.
Contrary to the original |SExtractor|, |SExtractor++| relies on the `FITSIO library <https://heasarc.gsfc.nasa.gov/fitsio/>`_
and is therefore compatible with all standard variants of |FITS|, including compressed images.
Both "Basic FITS" (one single header and one single body) and |MEF|_ files are recognized.
However there are some limitations with the current version of |SExtractor++|:

* there can be only one extension supported per filename (e.g., :param:`image.fits[1]` or :param:`image.fits[foo]`)
* in multichannel images with :math:`{\rm NAXIS} > 2`, only the first data-plane is loaded

..  Binary |SExtractor++| catalogs produced from |MEF| images are |MEF| files themselves.
    If the catalog output format is set to ASCII, all catalogs from the individual extensions are concatenated in one big file.
    In |SExtractor++|, as in all similar programs, |FITS| axis #1 is traditionally referred to as the *x* axis, and |FITS| axis #2 as the *y* axis.

Detection and measurement images
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

|SExtractor++| distinguishes between two kinds of science images, detection and measurement images:

- **The detection image** is where the sources are extracted.
  Rough estimates of position and shapes are obtained from the detection image; they define the photometric apertures and initial guesses for model-fitting parameters that will be applied to the measurement images.
  Currently there can be only one detection image per run, and sources are detected from a single channel.

- **Measurement images are where the sources are measured**.
  There can be hundreds or even more of them.
  They need not share the same pixel grid or have the same size as the detection image, which they may only partially overlap.
  However if the pixel grid is different, both detection and measurement image headers must contain valid |WCS|_ information :cite:`2002AA_395_1061G,2002AA_395_1077C`.
  |SExtractor++| uses that information to precisely match celestial positions and areas on all images.
  A scaling parameter for pixel values can be applied to any measurement image independently, provided either as a ``flux_scale`` optional argument to :func:`~config.measurement_images.load_fits_image()`, or as the value of a |FITS| header keyword (:param:`FLXSCALE` by default).
  Note that sources need not be detectable at all on measurement images for the software to work.

Weight-maps
~~~~~~~~~~~

Every detection and measurement image can have its own weight-map.


PSF models
~~~~~~~~~~
|PSF|_ model files must be provided as FITS binary tables in the `PSFEx format <https://psfex.readthedocs.io/en/latest/Appendices.html#psf-file-format-description>`_.


