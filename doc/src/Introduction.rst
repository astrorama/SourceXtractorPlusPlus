.. File Introduction.rst

.. include:: global.rst

Introduction
============

|SExtractor++|_ (Source-Extractor ++) is a program that extracts a catalog of sources from astronomical images. It is
the successor to the original `SExtractor <https://www.astromatic.net/software/sextractor/>`_ package :cite:`1996AAS_117_393B`. |SExtractor++|_ has been completely rewritten  in C++ and improves over its predecessor in many ways:

* Support for multiple "measurement" images
* Optimized multi-object, multi-frame model-fitting engine
* Possibility to define complex priors and dependencies for model parameters
* Flexible, Python-based configuration file
* Efficient image data caching 
* Multithreaded processing
* Modular code design with support for third-party plug-ins

|SExtractor++|_ is a collaborative effort between `ISDC <https://www.isdc.unige.ch/>`_ (Geneva), `USM <https://www.usm.uni-muenchen.de/>`_ (Munich) and `IAP <http://www.iap.fr/>`_ (Paris).
