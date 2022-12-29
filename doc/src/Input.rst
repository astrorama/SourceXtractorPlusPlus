.. File Input.rst

.. include:: global.rst

.. _input:

Input files
-----------

|SourceXtractor++| accepts images stored in |FITS|_ :cite:`1981AAS_44_363W`.
Contrary to the original |SExtractor|, |SourceXtractor++| relies on the `FITSIO library <https://heasarc.gsfc.nasa.gov/fitsio/>`_
and is therefore compatible with all standard variants of |FITS|, including compressed images.
Both "Basic FITS" (one single header and one single body) and |MEF|_ files are recognized.
Binary |SourceXtractor++| catalogs produced from |MEF| images are |MEF| files themselves.
If the catalog output format is set to ASCII, all catalogs from the individual extensions are concatenated in one big file.

Currently, only the first data-plane of multichannel images with :math:`{\rm NAXIS} > 2`, is loaded.

In |SourceXtractor++|, as in all similar programs, |FITS| axis #1 is traditionally referred to as the *x* axis, and |FITS| axis #2 as the *y* axis.

Detection and measurement images
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

|SourceXtractor++| distinguishes between two kinds of science images, detection and measurement images ():

- **The detection image** is where the sources are extracted.
  Rough estimates of position and shapes are obtained from the detection image; they define the photometric apertures and initial guesses for model-fitting parameters that will be applied to the measurement images.
  Currently there can be only one detection image per run, and sources are detected from a single channel.

- **Measurement images are where the sources are measured**.
  There can be hundreds (or even more) of them.
  They need not share the same pixel grid or have the same size as the detection image, which they may only partially overlap.
  However if the pixel grid is different, both detection and measurement image headers must contain valid |WCS|_ information :cite:`2002AA_395_1061G,2002AA_395_1077C`.
  |SourceXtractor++| uses that information to precisely match celestial positions and areas on all images.
  A scaling parameter for pixel values can be applied to any measurement image independently, provided either as a ``flux_scale`` optional argument to :func:`~config.measurement_images.load_fits_image()` in the Python measurement configuration file, or as the value of a |FITS| header keyword (:param:`FLXSCALE` by default).
  Note that sources need not be detectable at all on measurement images for the software to work.

.. _fig_detandmeas:

.. figure:: figures/detandmeas.*
   :figwidth: 100%
   :align: center

   Detection and measurement images (see text). The concept of measurement groups is detailed in the :ref:`grouping and splitting section <groups>`.

Weight-maps
~~~~~~~~~~~

Both detection and measurement images can have their own weight-maps.
Weight-maps are companion images that indicate how "noisy" every science pixel is, in terms of variance.
The |SourceXtractor++| ``--weight-image`` command line option and the :func:`~config.measurement_images.load_fits_image()` measurement configuration script command may be used to specify the names of the input detection and measurement weight-maps, respectively.
See the :ref:`Weighting section <chap_weighting>` for details.

.. _chap_inputflagmaps:

Flag-maps
~~~~~~~~~

Flag maps are images in integer format having the same dimensions as the science images, with pixel values that can be used to flag some pixels (for instance, “bad” or noisy pixels).
Flag map usage is described in the :ref:`flagging section <chap_flagging>`.

PSF models
~~~~~~~~~~

PSF models must be specified in the :ref:`model-fitting module <chap_model>`. |SourceXtractor++| accepts several |PSF|_ formats:

* using no PSF at all is specified with ``.., psf="nopsf",..`` in the configuration files (ASCII or python);
* a constant PSF can be given with a FITS image, and the keyword ``SAMPLING`` provides the sampling of the PSF image with respect to the corresponding measurement image (SAMPLING=0.5 means the 2 PSF pixels correspond to one pixel in the measurement image);
* a variable PSF can be provided as FITS binary tables in the `PSFEx format <https://psfex.readthedocs.io/en/latest/Appendices.html#psf-file-format-description>`_.
* a variable PSF can be provided in a **stacked PSF format**. In this FITS format:
** the first extension is a primary extension without data;
** the second extension is an image extension named ``PSF_IMAGE`` containing, aligned in a grid, the PSF data for the various regions in the measurement image;
** the third extension is a table extension with the mandatory columns:
*** ``X_CENTER``: the central x-position of a grid PSF;
*** ``Y_CENTER``: the central y-position of a grid PSF;
*** ``X``: the x-position center of the area for the corresponding grid PSF;
*** ``Y``: the x-position center of the area for the corresponding grid PSF;
*** ``RA`` and ``DEC``: the sky positions for ```X``` and ```Y```;


One PSF model must be assigned for every measurement image!.

.. _fig_psf_data:

.. figure:: figures/psf_data.*
   :figwidth: 100%
   :align: center

   The PSF data in the second extension of a stacked image PSF.

