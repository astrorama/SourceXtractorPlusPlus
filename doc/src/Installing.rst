.. File Installing.rst

.. include:: global.rst

***********************
Installing the software
***********************

Hardware requirements
=====================

Terminal
--------

|SExtractor++| runs in (ANSI) text-mode from a shell.
A graphical environment is not necessary to operate the software.

Memory
------

Memory requirements depend mostly on the number of images to be analyzed.
A rule of thumb is that |SExtractor++| requires about 100MB of resident memory per input frame.
Extra-memory is taken advantage of through caching of pixel data.

CPUs
----

|SExtractor++|'s measurement pipeline is multithreaded and can take advantage of multiple CPU cores.


Obtaining |SExtractor++|
------------------------

Software requirements
---------------------

Installation
------------

To install from the |GitHub| source package, you must first uncompress the archive:

.. code-block:: console

  $ unzip sextractorxx-<version>.zip

A new directory called :file:`sextractorxx-<version>` should now appear at the current location on your disk.

