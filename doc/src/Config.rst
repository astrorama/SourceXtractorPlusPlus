.. File Config.rst

.. include:: global.rst

.. _config_file:

The configuration file
----------------------

Each time it is run, |SExtractor++|_ looks for a configuration file. The name of th configuration files is given with: ::

    $sourcextractor++ --config-file sepp.config

Creating a configuration file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

|SE++|_ dumps all parameters with their default values into a configuration files with: ::

    $sourcextractor++ --dump-default-config > my_sepp.config


Format of the configuration file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The |SE++| configuration file is an ASCII file with:

* parameters and value separated by "=" such as: ``detection-image=coadd.fits``;
* empty lines or lines starting with ``#`` are ignored;
* boolean parameters are set/unset with 1/0 such as: ``output-flush-unsorted=1``;
* the individual parameters of a parameter list are separted with a "," such as ``output-properties=SourceIDs,PixelCentroid,WorldCentroid``
