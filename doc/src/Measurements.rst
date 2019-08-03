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
PSF models can be computed using the |PSFEx|_ package.
|SExtractor++| can fit mixtures of models to clumps of overlapping objects, which is generally effective at taking care of the contamination by the light of neighbors.
The main inconvenient of model-fitting is that it is slow.

.. _groups:

Grouping
--------

There are two types of groups: image groups and measurement groups.
|SExtractor++| measures the properties of sources from instances spread across multiple exposures.
Each exposure has its own context defined by a specific combination of filter, instrument, epoch, etc.
Depending on science goals, one may choose to group images by time of observation (e.g., to build light curves), filter (for regular photometry), ...
This is the purpose of |SExtractor++|'s grouping mechanisms.

.. _configuration_script:

The measurement configuration script
------------------------------------

Grouping procedures, as well as measurement settings and catalog outputs must be defined in a configuration script that uses the 'Python language <https://python.org>'_: the measurement configuration script.
The Python language makes it possible to set up arbitrarily refined rules to control the measurement process.



Both types of groups must be defined in a common 

.. _measurement_groups:

.. toctree::

  Position
  Photom
  Model


