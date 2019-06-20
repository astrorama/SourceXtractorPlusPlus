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

.. toctree::

  Position
  Photom
  Model


