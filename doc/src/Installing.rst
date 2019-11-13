.. File Installing.rst

.. include:: global.rst

***********************
Installing the software
***********************

Hardware requirements
=====================

Terminal
--------

|SourceXtractor++| runs in (ANSI) text-mode from a shell.
A graphical environment is not necessary to operate the software.

Memory
------

Memory requirements depend mostly on the number of images to be analyzed.
A rule of thumb is that |SourceXtractor++| requires about 100MB of resident memory per input frame.
Extra-memory is taken advantage of through caching of pixel data.

CPUs
----

|SourceXtractor++|'s measurement pipeline is multithreaded and can take advantage of multiple CPU cores.


Obtaining |SourceXtractor++|
------------------------

Software requirements
---------------------

Installation
------------

To install from the |GitHub| source package, you must first uncompress the archive:

.. code-block:: console

  $ unzip sextractorxx-<version>.zip

A new directory called :file:`sextractorxx-<version>` should now appear at the current location on your disk.


--------------
Installing the SourceXtractor++ source package on Ubuntu 19.04
----------------------------------------------------------

The follwing ios a protokol for the installation of the entire SourceXtractor++ on a Ubuntu-19.04 system which was freshly installed as a virtual machine.

1. Install necessary libraries
the following packages are necessary to run SourceXtractor++:

* CCFits;
* cfitsio;
* boost;
* fftw3;
* python (recommende is python3, python2 is still accepted);
* astropy;
* log4cpp;
* wcslib;

These packages can be installed with the usual tools either at the beginning of the installation process or whenever a missing library is reported. Please make sure to always install the development packages as well!

The following packages are optional:

* readline
* ncurses (optional, for the project bar)
* levmar (optional, for model fitting: http://users.ics.forth.gr/~lourakis/levmar/)

2. Do the setup::

   > cd ~
   > mkdir Work
   > cd Work
   > mkdir Projects
   > cd Projects

3. Download and install Elements::

   > git clone https://github.com/astrorama/Elements.git Elements
   > cd Elements
   > git checkout 5.8
   > # follow the README.md
   > # mkdir build; cd build; cmake -DCMAKE_INSTALL_PREFIX=/usr -DUSE_SPHINX=OFF .. ; make; make install
   > # "make install" needs sudo privileges

Some comments on the cmake flags:

* "-DPYTHON_EXPLICIT_VERSION=3 forces to use python3. Using python2 is still possible, however not recommended. Also, using python2 would result into using using an older version of astropy which would need to be installed via 'pip';
* "-DUSE_SPHINX=OFF" switches the sphinx documentation system off, which is not necessary to use and develop SourceXtractor++. Though python3-sphinx is installed, cmake reported an error with sphinx, and switching it off is the easiest solution.

4. Download and install Alexandria::

   > cd ~/Work/Projects/
   > # point to the Elements cmake files (can be put in the .bashrc):
   > export ElementsProject_DIR=/usr/lib64/cmake/ElementsProject
   > # set some cmake flags (can be put in the .bashrc):
   > export CMAKEFLAGS="-DPYTHON_EXPLICIT_VERSION=3 -DUSE_SPHINX=OFF"
   > git clone https://github.com/astrorama/Alexandria.git Alexandria
   > cd Alexandria
   > git checkout deuclidized/2.10
   > make configure
   > make all
   > make install

5. Download and install SourceXtractor++::

   > cd ~/Work/Projects/
   > # set the CMAKE_PROJECT_PATH (put in the .bashrc)
   > export CMAKE_PROJECT_PATH=~/Work/Projects
   > git clone https://github.com/ebertin/sextractorxx.git SExtractorxx
   > cd SExtractorxx
   > git checkout develop
   > make configure
   > make all
   > make install

6. Start working with SourceXtractor++

   After the binaries are now compiled and available, the SourceXtractor++ can be run with::

   > ~/Work/Projects/SExtractorxx/build/run sextractor++ --help
