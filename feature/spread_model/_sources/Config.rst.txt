.. File Config.rst

.. include:: global.rst

.. _chap_config:

Configuration
=============

There are two types of configuration settings: those dedicated purely to measurements, and those related to the source extraction process and to global operations of |SourceXtractor++|.
Measurement settings can be rather complex and require a :ref:`Python configuration script <chap_configuration_script>` (see the :ref:`Measurement section <chap_measurements>`).
Global configuration settings may be changed using command-line options (prefixed with a ``--``), however a configuration file is often more convenient for storing settings that do not change from run to run.

.. note::

  Command-line options can be abbreviated, provided that there is no more than one full matching keyword.
  Example: ``--config-file=foo.conf`` may be abbreviated as ``--conf=foo.conf``.

.. _chap_config_file:

The configuration file
----------------------

|SourceXtractor++| searches for the configuration file at the beginning of a run, starting from system repositories (which makes it possible to apply specific, system-wide configuration settings), to the current repository. Each time it is run, |SourceXtractor++|_ looks for a configuration file. The name of a dedicated configuration files is given with: ::

    $ sourcextractor++ --config-file sepp.config

Creating a configuration file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

|SE++|_ dumps all parameters with their default values into a configuration files with: ::

    $ sourcextractor++ --dump-default-config > my_sepp.config


Format of the configuration file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Configuration instructions follow the ``<option> = <value>`` format.
There must be no more than one ``<option> = <value>`` instance per line.
Comments must be preceded with a ``#``. Boolean parameters are set/unset with 1/0 such as: ``output-flush-unsorted=1``, and the individual parameters of a parameter list are separted with a "," such as ``output-properties=SourceIDs,PixelCentroid,WorldCentroid``

.. _param_list:

Configuration parameter list
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here is a complete list of all the configuration parameters known to |SourceXtractor++|.
Please refer to the next sections for a detailed description of their meaning.

.. raw:: latex

  {\footnotesize


.. tabularcolumns:: |p{0.4\textwidth}|p{0.15\textwidth}|p{0.45\textwidth}|

===================================== ================= =======================================
Option                                Default           Use
===================================== ================= =======================================
**Generic options** 
-----------------------------------------------------------------------------------------------
``version``                                             Print version string
``help``                                                Produce help message
``config-file``                       `---`             Name of a configuration file
``log-level``                         ``INFO``          Log level: FATAL, ERROR, WARN, INFO 
                                                        (default), DEBUG
``log-file``                          `---`             Name of a log file
``list-output-properties``                              List the possible output properties for
                                                        the given input parameters and exit
``property-column-mapping-all``                         Show the columns created for each 
                                                        property
``property-column-mapping``                             Show the columns created for each 
                                                        property, for the given configuration
``progress-min-interval``             5                 Minimal interval to wait before 
                                                        printing a new log entry with the 
                                                        progress report
``progress-bar-disable``                                Disable progress bar display
\ 
------------------------------------- ----------------- ---------------------------------------
**Auto (Kron) photometry options**
-----------------------------------------------------------------------------------------------
``auto-kron-factor``                  `2.5`             Scale factor for AUTO (Kron) photometry
``auto-kron-min-radius``              `3.5`             Minimum radius for AUTO (Kron) 
                                                        photometry
\ 
------------------------------------- ----------------- ---------------------------------------
**Background modelling**
-----------------------------------------------------------------------------------------------
``background-cell-size``              `64`              Background mesh cell size to determine 
                                                        a value
``smoothing-box-size``                `3`               Background median filter size
\ 
------------------------------------- ----------------- ---------------------------------------
**Check images**
-----------------------------------------------------------------------------------------------
``check-image-model-fitting``         `---`             Path to save the model fitting check 
                                                        image
``check-image-residual``              `---`             Path to save the model fitting residual
                                                        check image
``check-image-background``            `---`             Path to save the background check image
``check-image-variance``              `---`             Path to save the variance check image
``check-image-segmentation``          `---`             Path to save the segmentation check 
                                                        image
``check-image-partition``             `---`             Path to save the partition check image
``check-image-grouping``              `---`             Path to save the grouping check image
``check-image-filtered``              `---`             Path to save the filtered check image
``check-image-thresholded``           `---`             Path to save the thresholded check 
                                                        image
``check-image-auto-aperture``         `---`             Path to save the auto aperture check 
                                                        image
``check-image-aperture``              `---`             Path to save the aperture check image
``check-image-moffat``                `---`             Path to save the moffat check image
``check-image-psf``                   `---`             Path to save the PSF check image
\ 
------------------------------------- ----------------- ---------------------------------------
**Cleaning**
-----------------------------------------------------------------------------------------------
``use-cleaning``                                        Enable the cleaning of sources 
                                                        (remove false detections near bright 
                                                        objects)
``cleaning-minarea``                  `3`               Minimum # of pixels above threshold
\ 
------------------------------------- ----------------- ---------------------------------------
**Detection image**
-----------------------------------------------------------------------------------------------
``background-value``                  `---`             Background value to be subtracted from 
                                                        the detection image
``detection-threshold``               `1.5`             Detection threshold above the 
                                                        background
``segmentation-algorithm``            ``LUTZ``          Segmentation algorithm to be used. 
                                                        Currently LUTZ is the only choice
``segmentation-disable-filtering``                      Disables filtering
``segmentation-filter``               `---`             Loads a filter
``detection-image``                   `---`             Path to a fits format image to be used 
                                                        as detection image.
``detection-image-gain``              `0`               Detection image gain in e-/ADU (0 = 
                                                        infinite gain)
``detection-image-flux-scale``                          Detection image flux scale
``detection-image-saturation``                          Detection image saturation level (0 = 
                                                        no saturation)
``detection-image-interpolation``     `1`               Interpolate bad pixels in detection 
                                                        image
``detection-image-interpolation-gap`` `5`               Maximum number if pixels to interpolate
                                                        over
\ 
------------------------------------- ----------------- ---------------------------------------
**External flag options**
-----------------------------------------------------------------------------------------------
``flag-image-*``                      `---`             FITS file containing the external 
                                                        flag
``flag-type-*``                       `---`             The combination type of the external 
                                                        flag (OR, AND, MIN, MAX, MOST)
\ 
------------------------------------- ----------------- ---------------------------------------
**Extraction**
-----------------------------------------------------------------------------------------------
``detect-minarea``                    `3`               Minimum # of pixels above threshold
``use-attractors-partition``                            Enables the use of attractors for 
                                                        partitioning
\ 
------------------------------------- ----------------- ---------------------------------------
**Grouping**
-----------------------------------------------------------------------------------------------
``grouping-algorithm``                ``NONE``          Grouping algorithm to be used
                                                        (NONE, MOFFAT)
``grouping-moffat-threshold``         `0.02`            Threshold used for Moffat grouping.
\ 
------------------------------------- ----------------- ---------------------------------------
**Magnitude**
-----------------------------------------------------------------------------------------------
``magnitude-zeropoint``               `0`               Magnitude zero point calibration
\ 
------------------------------------- ----------------- ---------------------------------------
**Measurement config**
-----------------------------------------------------------------------------------------------
``python-config-file``                `---`             Measurements Python configuration file
``python-arg``                        `---`             Parameters to pass to Python via 
                                                        `sys.argv`
\ 
------------------------------------- ----------------- ---------------------------------------
**Memory usage**
-----------------------------------------------------------------------------------------------
``tile-memory-limit``                  `512`            Maximum memory used for image tiles 
                                                        cache in megabytes
``tile-size``                          `256`            Image tiles size in pixels
\ 
------------------------------------- ----------------- ---------------------------------------
**Model Fitting**
-----------------------------------------------------------------------------------------------
``model-fitting-iterations``          `1000`            Maximum number of iterations allowed 
                                                        for model fitting
\ 
------------------------------------- ----------------- ---------------------------------------
**Multi-threading**
-----------------------------------------------------------------------------------------------
``thread-count``                      `4`               Number of worker threads (0=disable all
                                                        multithreading)
\ 
------------------------------------- ----------------- ---------------------------------------
**Multi-thresholding**
-----------------------------------------------------------------------------------------------
``partition-multithreshold``                            Activate multithreshold partitioning
``partition-threshold-count``         `32`              of thresholds
``partition-min-area``                `3`               Minimum area in pixels to consider 
                                                        partitioning
``partition-min-contrast``            `0.005`           Minimum contrast for partitioning
\ 
------------------------------------- ----------------- ---------------------------------------
**Output configuration**
-----------------------------------------------------------------------------------------------
``output-catalog-filename``           `---`             The file to store the output catalog
``output-catalog-format``             ``FITS``          The format of the output catalog, one 
                                                        of ASCII or FITS
``output-properties``                 ``PixelCentroid`` The output properties to add in the 
                                                        output catalog
\ 
------------------------------------- ----------------- ---------------------------------------
**Plugin configuration**
-----------------------------------------------------------------------------------------------
``plugin-directory``                  `---`             Path to a directory that contains the 
                                                        plugins
``plugin``                            `---`             Defines a plugin to load (without file 
                                                        extension). Can be used multiple times
\ 
------------------------------------- ----------------- ---------------------------------------
**Variable PSF**
-----------------------------------------------------------------------------------------------
``psf-filename``                      `---`             PSF image file (FITS format)
``psf-fwhm``                          `---`             Generate a Gaussian PSF with the given 
                                                        full-width half-maximum (in pixels)
``psf-pixel-sampling``                `---`             Generate a Gaussian PSF with the given 
                                                        pixel sampling step size
\ 
------------------------------------- ----------------- ---------------------------------------
**Weight map**
-----------------------------------------------------------------------------------------------
``weight-image``                      `---`             Path to a FITS format image to be used 
                                                        as weight image
``weight-absolute``                   `0`               Is the weight map provided as absolute 
                                                        values or relative to background
``weight-type``                       ``BACKGROUND``    Weight image type (BACKGROUND, RMS,
                                                        VARIANCE, WEIGHT)
``weight-scaling``                    `1`               Weight map scaling factor
``weight-threshold``                  `---`             Threshold for pixels to be considered 
                                                        bad pixels. In same units as weight 
                                                        map.
``weight-usesymmetry``                `1`               Use object symmetry to replace pixels 
                                                        above the weight threshold for 
                                                        photometry
===================================== ================= =======================================

.. raw:: latex

  }
