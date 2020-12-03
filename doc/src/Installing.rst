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
As of version 0.13, |SourceXtractor++| scales reasonably well up to 8 cores.

Obtaining |SourceXtractor++|
============================

From Fedora or EPEL repositories |rawhide|
------------------------------------------

.. |rawhide|  image:: https://img.shields.io/fedora/v/sourcextractor++/rawhide
  :alt: Fedora package
  :target: https://src.fedoraproject.org/rpms/sourcextractor++

|SourceXtractor++| is available in `Fedora <https://src.fedoraproject.org/rpms/sourcextractor++>`_
30 and greater, and in EPEL 7 as well. On those platforms, you can use your
package manager to install in the usual way:

.. code-block:: console

  $ dnf install sourcextractor++

From Anaconda Cloud |anaconda_cloud|
------------------------------------

.. |anaconda_cloud| image:: https://img.shields.io/conda/vn/astrorama/sourcextractor
  :alt: Conda
  :target: https://anaconda.org/astrorama/sourcextractor

|SourceXtractor++| is also available for Linux and MacOS via Anaconda cloud.
You can install it as follows:

.. code-block:: console

  $ conda install -c conda-forge -c astrorama sourcextractor

Or you can, of course, add permanently the ``astrorama`` channel to your
conda configuration:

.. code-block:: console

  $ conda config --add channels conda-forge
  $ conda config --add channels astrorama
  $ conda install sourcextractor


It is recommended to install |SourceXtractor++| into its own
`environment <https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html>`_
to avoid dependency conflicts.

From Sources |github|
---------------------

.. |github| image:: https://img.shields.io/github/v/tag/astrorama/SourceXtractorPlusPlus
  :alt: GitHub tag (latest SemVer)
  :target: https://github.com/astrorama/SourceXtractorPlusPlus/releases/latest

The source package may be downloaded from the official `GitHub repository <https://github.com/astrorama/SourceXtractorPlusPlus>`_.
Links to binary packages for a selection of operating systems are also available at the URL above.

Software requirements
_____________________

|SourceXtractor++| relies on the |Elements|_ and |Alexandria|_ packages, which may be downloaded from `Astrorama  <https://github.com/astrorama>`_ repositories.
They are also available in Fedora and EPEL as ``elements`` and ``elements-alexandria``.

It also requires the following packages for installation:

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
* |LevMar|_ (model fitting engine)
* |GSL|_ (model fitting engine)

All or most of these packages are available in the main Linux distributions.
You will also need the ``-devel`` version of the packages if you compile |SourceXtractor++| from the source.

If you have installed Elements and Alexandria from your distribution repository (right now, only in Fedora), you can skip the following section.

.. code-block:: console

  $ dnf install elements-devel elements-alexandria-devel


Installation of Elements and Alexandria
_______________________________________

Elements is a C++ and CMake framework. It is capable of managing the dependencies
of the projects that are based on it, as long as it knows where to find them.
By convention, this location usually is ``~/Work/Projects``, but you can choose
any other.

.. code-block:: console

  $ export CMAKE_PROJECT_PATH=~/Work/Projects
  $ export CMAKE_PREFIX_PATH=$CMAKE_PROJECT_PATH/Elements/cmake
  $ mkdir -p $CMAKE_PROJECT_PATH

Since Python 2 has been deprecated since the 1st of January 2020, it is recommended to tell Elements and other projects to use Python 3 instead.
You can also disable the build of documentation with ``-DUSE_SPHINX=OFF``.

.. code-block:: console

  $ export CMAKEFLAGS="-DPYTHON_EXPLICIT_VERSION=3 -DUSE_SPHINX=OFF"

You can always add for convenience these environment variables to your
``.bashrc``, or the one corresponding to your shell.


Elements can handle multiple versions of a given project, or with just one.
For the later, it suffices to clone the project into ``$CMAKE_PROJECT_PATH``.

.. code-block:: console

  $ cd $CMAKE_PROJECT_PATH
  $ git clone https://github.com/astrorama/Elements.git --branch 5.10
  $ cd Elements
  $ make install -j # -j uses multiple cores

Similarly for Alexandria:

.. code-block:: console

  $ cd $CMAKE_PROJECT_PATH
  $ git clone https://github.com/astrorama/Alexandria.git --branch 2.15
  $ cd Alexandria
  $ make install -j

Installation
____________

If you have compiled and installed Elements and Alexandria as described on the above section, make sure you are using the proper environment.

.. code-block:: console

  $ export CMAKE_PROJECT_PATH=~/Work/Projects
  $ export CMAKE_PREFIX_PATH=$CMAKE_PROJECT_PATH/Elements/cmake

For system-wide installs, this step is not necessary.

Configure the project flags if you have not yet done it:

.. code-block:: console

  $ export CMAKEFLAGS="-DPYTHON_EXPLICIT_VERSION=3 -DUSE_SPHINX=OFF"

As a reminder, that tells |SourceXtractor++| to compile with Python 3, and not
to build the documentation.
|SourceXtractor++| can be built with Python 2, but this is strongly discouraged.

The sources can be obtained either from the `releases <https://github.com/astrorama/SourceXtractorPlusPlus/releases/latest>`_ as a compressed archive, or cloned with ``git`` the usual way.

If you download an archived version, you need to uncompress it:

.. code-block:: console

  $ unzip SourceXtractorPlusPlus-<version>.zip

A new directory called :file:`SourceXtractorPlusPlus-<version>` should now appear
at the current location on your disk.

If you opt for cloning the repository, make sure you are pointing to the latest commit in the ``master`` branch.

.. code-block:: console

  $ git pull

Either way, enter the directory you have just created, and build the software.

.. code-block:: console

  $ cd SourceXtractorPlusPlus
  $ make -j4

After the binaries are now compiled and available, |SourceXtractor++| can be run with:

.. code-block:: console

  $ ~/Work/Projects/SourceXtractorPlusPlus/build/run sextractor++ --help
