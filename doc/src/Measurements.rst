.. File Measurements.rst

.. include:: global.rst

.. _chap_measurements:

Measuring
=========

Once sources have been detected and deblended, they enter the measurement phase.

|SExtractor++| performs three categories of measurements: isophotal, aperture, and model-fitting.

.. _isophotal_measurements:

**Isophotal measurements** are done *on the detection image* only.
They exclusively take into account object pixels with values exceeding the detection threshold.
They run quick and are reasonably immune to light contamination by neighbors.
However as such they generally only provide heavily biased estimates of physical quantities, especially for fluxes.
Therefore their usage should be restricted to rough estimation and to make first guesses.
Isophotal measurements do not require any specific configuration.
They depend only on detection, deblending and background-subtraction settings.

.. _aperture_measurements:

**Aperture measurements** are done on the measurement images.
They involve pixels within geometric apertures, such as disks and ellipses.
The size and shape of the apertures may be fixed, or adaptive to every object.
Aperture measurements are largely sub-optimal from the point-of-view of |SNR|_.
However they are generally tolerant to object shape variations, and offer a good control of measurement biases.
They can be very sensitive to contamination by the light of neighbors, although this can be mitigated (see ??)

.. _model_measurements:

**Model-fitting measurements** are performed on the measurement images.
They are close to optimum from the point-of-view of |SNR|, but they require accurate PSF models (one per image).
|PSF|_ models can be computed using the |PSFEx|_ package.
|SExtractor++| can fit mixtures of models to clumps of overlapping objects, which is generally effective at taking care of the contamination by the light of neighbors.
The main inconvenient of model-fitting is that it is much slower than aperture photometry.

.. _chap_configuration_script:

The measurement configuration script
------------------------------------

Measurement settings, as well as :ref:`grouping procedures <groups>` and catalog outputs must be defined in a configuration script that uses the `Python language <https://python.org>`_: the measurement configuration script.
The script filename is set with the ``python-config-file`` configuration option. 
Thanks to its flexibility, the Python language makes it possible to set up arbitrarily complex rules to finely control the measurement process.
|SExtractor++|'s configuration library classes and functions must be imported at the beginning of the script:

.. code-block:: python

  from sextractorxx.config import *

The same goes for other Python libraries that might be needed for the current configuration, such as `glob <https://docs.python.org/library/glob.html>`_ for filename expansion, `math <https://docs.python.org/library/math.html>`_ or `NumPy <https://numpy.org/>`_ for numerical computations, ...

.. caution::
   The measurement configuration script is excusively meant to be executed by the |SExtractor++| built-in Python interpreter, and cannot be run on its own.

Measurement images
~~~~~~~~~~~~~~~~~~

Measurement images are scientific images stored as |FITS|_ files and used only for measurements. 
Every measurement image can be associated a |PSF| model (|PSFEx|'s :file:`.psf` file), and a weight map.
The :func:`~config.measurement_images.load_fits_image()` function creates a measurement image from a |FITS| image filename; for example:

.. _chap_load_fits_image:

.. code-block:: python

  mesimage = load_fits_image("image.fits")

The list of currently loaded measurement images can be displayed using the :func:`~config.measurement_images.print_measurement_images()` function.
The following will print the list of measurement images on screen (more precisely, on the `stderr <https://en.wikipedia.org/wiki/Standard_streams#Standard_error_(stderr)>`_ channel):

.. code-block:: python

  print_measurement_images()

In practice, one will generally want to load several images with a single instruction, not one by one.
However before addressing this point we need to introduce the concept of :ref:`grouping <groups>`.

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
The most efficient way to instantiate an image group is through the :func:`~config.measurement_images.load_fits_images()` function:

.. code-block:: python

  imagegroup = load_fits_images(["image_01.fits", "image_02.fits"])

In practice, the easier way to load large series of images, |PSF| models and weight maps is through filename expansion and sorting, for instance:

.. code-block:: python

  imagegroup = load_fits_images(
      sorted(glob("image_??.fits")),
      psf_list=sorted(glob("image_??.psf")),
      weight_list=sorted(glob("image_??.weight.fits"))
  )

All non-empty lists must contain the same number of items.
It is possible to add images or another group to a pre-existing image group using the :meth:`~config.measurement_images.ImageGroup.add_images()` method, e.g.:

.. code-block:: python

  anotherimage = load_fits_image(
      "anotherimage.fits",
      psf_file="anotherimage.psf",
      weight_file="anotherimage.weight.fits"
  )
  imagegroup.add_images(anotherimage)

Splitting
^^^^^^^^^

Now, within an image group one may want to create subgroups based on, e.g., band pass filters or epochs. 
This is easily accomplished using the :meth:`~config.measurement_images.ImageGroup.split()` method. 
Splitting may be done according to a |FITS| header keyword with the :class:`~config.measurement_images.ByKeyword()` callable.
For instance, to generate subgroups each with a different filter (assuming all image headers contain the :param:`FILTER` keyword):

.. code-block:: python

  imagegroup.split(ByKeyword('FILTER'))

:class:`~config.measurement_images.ByPattern()` also checks for a |FITS| header keyword, with the difference that a `regular expression <https://en.wikipedia.org/wiki/Regular_expression>`_ provided by the user is applied to the keyword value.
The first matching group acts as the grouping key, which means that a 'capturing group (within parentheses) <https://www.regular-expressions.info/brackets.html>'_ must be present in the regular expression.
For instance, the following command groups images by year of observation, ignoring the rest of the date: 

.. _bypattern:

.. code-block:: python

  imagegroup.split(ByPattern('DATE-OBS', "^(\d{4})"))

.. note::
  Subgroups can themselves be split into subgroups, at any level.

Accessing subgroups
^^^^^^^^^^^^^^^^^^^

Image subgroups are custom `maps <https://en.wikipedia.org/wiki/Associative_array>`_.
One can access individual subgroups much like a `Python dictionary <https://docs.python.org/3/tutorial/datastructures.html#dictionaries>`_.
In the first :ref:`group splitting example above <bypattern>`, the subgroup of images from year 2007 (if they exists) is simply  :param:`imagegroup['2007']`, and looping through all subgroups is as simple as:

.. code-block:: python

  for year, subgroup in imagegroup:
    print(int(year), subgroup)

.. _measurement_groups:

Measurement groups
^^^^^^^^^^^^^^^^^^

Measurements are applied to measurement groups and subgroups.
Measurement groups are similar to image groups, although they maintain additional information and their image and subgroup content cannot be updated.
A measurement group (:class:`~config.measurement_images.MeasurementGroup`) is instanciated from a completed image groups, e.g.,

.. code-block:: python

  mesgroup = MeasurementGroup(imagegroup)

Like image subgroups, measurement subgroups are custom maps and can be managed in a similar way.

Measurements
------------


.. toctree::
  :maxdepth: 3

  Position
  Photom
  Model

