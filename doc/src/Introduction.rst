.. File Introduction.rst

.. include:: global.rst

Introduction
============

|SExtractor++|_ (Source-Extractor ++) is a program that extracts a catalog of sources from astronomical images. It is
the successor to the `original SExtractor <https://github.com/astromatic/sextractor>`_ package :cite:`1996AAS_117_393B`. |SExtractor++| has been completely rewritten  in C++ and improves over its predecessor in many ways:

* Support for multiple "measurement" images
* Optimized multi-object, multi-frame model-fitting engine
* Possibility to define complex priors and dependencies for model parameters
* Flexible, Python-based configuration file
* Efficient image data caching 
* Multithreaded processing
* Modular code design with support for third-party plug-ins

|SExtractor++| is a collaborative effort between the `Astronomy Department, Université de Genève <https://www.unige.ch/sciences/astro/en/>`_, the `Faculty of Physics, LMU Munich <https://www.en.physik.uni-muenchen.de/>`_, and `the IAP (CNRS/Sorbonne Université) <http://www.iap.fr/>`_.
