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
As of version 0.7.1, |SourceXtractor++| scales reasonably well up to 8 cores.

Obtaining |SourceXtractor++|
----------------------------

The source package may be downloaded from the official repository `https://github.com/astrorama/SourceXtractorPlusPlus`.
Links to binary packages for a selection of operating systems are also available at the URL above.

Software requirements
---------------------

|SourceXtractor++| relies on the |Elements|_ and |Alexandria|_ packages, which may be downloaded from `Astrorama  <https://github.com/astrorama>`_ repositories.
|SourceXtractor++| requires the following packages for installation:

* `Boost <https://www.boost.org/>`_
* `CCFITS <https://heasarc.gsfc.nasa.gov/fitsio/CCfits/>`_
* `CFITSIO <https://heasarc.gsfc.nasa.gov/fitsio/>`_
* `FFTW3 <http://www.fftw.org/>`_
* `Log4cpp <http://log4cpp.sf.net/>`_
* `OpenCV <https://opencv.org/>`_
* `Python <https://www.python.org/>`_ (Python3.x recommended)
* `Python-AstroPy <https://www.astropy.org/>`_
* `WCSlib <https://www.atnf.csiro.au/people/mcalabre/WCS/index.html>`_

The following packages are optional:

* `Readline <https://tiswww.case.edu/php/chet/readline/rltop.html>`_
* `Ncurses <https://invisible-island.net/ncurses/>`_ (progress bar display)
* `LevMar <http://users.ics.forth.gr/~lourakis/levmar/>`_ (model fitting)

All or most of these packages are available in the main linux distributions.
You will also need the ``-devel`` version of the packages if you compile |SourceXtractor++| from the source.

Installation
------------

To install from the |GitHub| source package, you must first uncompress the archive:

.. code-block:: console

  $ unzip sextractorxx-<version>.zip

A new directory called :file:`sextractorxx-<version>` should now appear at the current location on your disk.


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
