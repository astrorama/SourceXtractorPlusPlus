.. File Measurements.rst

.. include:: global.rst

Measurements
============

Once sources have been detected and deblended, they enter the measurement phase.
|SExtractor++| performs three categories of measurements: isophotal, aperture, and model-fitting.

.. _isophotal_measurements:

Isophotal measurements are done *on the detection image* only.
They exclusively take into account object pixels with values exceeding the detection threshold.
They run quick and are reasonably immune to light contamination by neighbors.
However as such they generally only provide heavily biased estimates of physical quantities, especially for fluxes.
Therefore their usage should be restricted to rough estimation and to make first guesses.
Isophotal measurements do not require any specific configuration.
They depend only on detection, deblending and background-subtraction settings.

.. _aperture_measurements:

Aperture measurements are done on the measurement images.
They involve pixels within geometric apertures, such as disks and ellipses.
The size and shape of the apertures may be fixed, or adaptive to every object.
Aperture measurements are largely sub-optimal from the point-of-view of |SNR|_.
However they are generally tolerant to object shape variations, and offer a good control of measurement biases.
They can be very sensitive to contamination by the light of neighbors, although this can be mitigated (see ??)

.. _model_measurements:

Model-fitting measurements are performed on the measurement images.
They are close to optimum from the point-of-view of |SNR|, but they require accurate PSF models (one per image).
|PSF|_ models can be computed using the |PSFEx|_ package.
|SExtractor++| can fit mixtures of models to clumps of overlapping objects, which is generally effective at taking care of the contamination by the light of neighbors.
The main inconvenient of model-fitting is that it is much slower than aperture photometry.

.. _configuration_script:

The measurement configuration script
------------------------------------

Measurement settings, as well as `grouping procedures <groups>`_ and catalog outputs must be defined in a configuration script that uses the `Python language <https://python.org>`_: the measurement configuration script.
Thanks to its flexibility, the Python language makes it possible to set up arbitrarily complex rules to finely control the measurement process.
|SExtractor++|'s configuration library classes and functions must be imported at the beginning of the script:

.. code-block:: python

  from sextractorxx.config import *

The same goes for other Python libraries that might be needed for the current configuration, such as `glob <https://docs.python.org/library/glob.html>`_ for filename expansion, `math <https://docs.python.org/library/math.html>`_ or `NumPy <https://numpy.org/>`_ for numerical computations, ...

.. note::
   The measurement configuration script is excusively meant to be executed by the |SExtractor++| built-in Python interpreter, and cannot be run on its own.

Measurement images
~~~~~~~~~~~~~~~~~~

Measurement images are scientific images stored as |FITS|_ files and used only for measurements. 
Every measurement image can be associated a |PSF| model (|PSFEx|'s :file:`.psf` file), and a weight map.
The :func:`load_fits_image()` function creates a measurement image from a |FITS| image filename:

<measurement_image> = :param:`load_fits_image (` <image_filename> [ , psf_file = <psf_filename> ] [ , weight_file = <weight_filename> ] :param:`)`

For example:

.. code-block:: python

  mesimage = load_fits_image("image.fits")

The list of currently loaded measurement images can be displayed using the :func:`print_measurement_images()` function:

:param:`print_measurement_images (` [<file_object>] :param:`)`

The following will print the list of measurement images on screen (more precisely, on the `stderr <https://en.wikipedia.org/wiki/Standard_streams#Standard_error_(stderr)>`_ channel):

.. code-block:: python

  print_measurement_images()

In practice, one will generally want to load several images with a single instruction, not one by one.
However before addressing this point we need to introduce the concept of `grouping <groups>`_.

.. _groups:

Grouping and splitting
~~~~~~~~~~~~~~~~~~~~~~

|SExtractor++| measures the properties of sources from instances spread across multiple exposures.
Each exposure has its own context defined by a specific combination of filter, instrument, epoch, etc.
Depending on science goals, one may choose to group images by time of observation (e.g., to build light curves), filter (for regular photometry), ...
This is the purpose of |SExtractor++|'s grouping (and splitting) mechanisms.

There are two types of groups: image groups and measurement groups.

.. _image_groups:

Image groups
^^^^^^^^^^^^

Image groups offer a convenient way to load many images, and group them according to specific criteria.
The easiest way to instantiate an image group is through the :func:`load_fits_images()` function:

<image_group> = :param:`load_fits_images (` <list_of_image_filenames> [ , psf_list = <list_of_psf_filenames> ] [ , weight_list = <list_of_weight_filenames> ]  :param:`)`

All non-empty lists must contain the same number of items.
In practice, the most convenient way to load large series of images, |PSF| models and weight maps is through filename expansion and sorting, for instance:

.. code-block:: python

  imagegroup = load_fits_images(
      sorted(glob("image_??.fits")),
      psf_list=sorted(glob("image_??.psf")),
      weight_list=sorted(glob("image_??.weight.fits"))
  )

It is possible to add images or another group to a pre-existing image group using the :func:`add_images()` method, e.g.:

.. code-block:: python

  anotherimage = load_fits_image(
      "anotherimage.fits",
      psf_file="anotherimage.psf",
      weight_file="anotherimage.weight.fits"
  )
  imagegroup.add_images(anotherimage)





.. _measurement_groups:

Measurement groups
^^^^^^^^^^^^^^^^^^

.. toctree::

  Position
  Photom
  Model


