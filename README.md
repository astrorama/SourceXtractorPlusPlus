# SourceXtractor++
[![Conda](https://img.shields.io/conda/v/astrorama/sourcextractor)](https://anaconda.org/astrorama/sourcextractor)
[![Fedora package](https://img.shields.io/fedora/v/sourcextractor++/rawhide)](https://koji.fedoraproject.org/koji/packageinfo?packageID=30842)
[![Build RPM](https://github.com/astrorama/SourceXtractorPlusPlus/workflows/Build%20RPM/badge.svg)](https://github.com/astrorama/SourceXtractorPlusPlus/actions)
[![Coverity](https://scan.coverity.com/projects/19270/badge.svg?flat=1)](https://scan.coverity.com/projects/astrorama-sextractorxx)
[![Documentation](https://readthedocs.org/projects/sourcextractorplusplus/badge/?version=latest)](https://sourcextractorplusplus.readthedocs.io/en/latest/Introduction.html)
[![CodeFactor](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/badge/develop)](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/overview/develop)

The next generation SExtractor.

Documentation: https://astrorama.github.io/SourceXtractorPlusPlus/

# Installation

## Conda

SourceXtractor++ is available on [Anaconda Cloud for Linux and MacOSX](https://anaconda.org/astrorama/sourcextractor)

```bash
conda install -c conda-forge -c astrorama sourcextractor
 ```

We would recommend, however, to install it into its own environment.

```bash
conda create -n sourcex -c astrorama -c conda-forge sourcextractor
conda activate sourcex
 ```

**Note:** If you want the development version to test a future functionality or bugfix, it can also be done with conda:

```bash
conda create -n sourcex-dev -c astrorama/label/develop -c astrorama -c conda-forge sourcextractor
conda activate sourcex-dev
```

## Fedora / CentOS

SourceXtractor++ is shipped directly in Fedora and CentOS. It can be installed simply as follows:

```bash
sudo dnf install sourcextractor++
```

[We also build our own RPMs as part of our CI setup](http://repository.astro.unige.ch/euclid/devel/)
They can be installed adding a `.repo` file under `/etc/yum.repos.d` with the following content:

```ini
[Astrorama]
name=Astrorama
baseurl=http://repository.astro.unige.ch/euclid/devel/stable/fedora/$releasever/$basearch
enabled=1
gpgcheck=0
```

And install with

```bash
dnf install SourceXtractorPlusPlus
```

## References and Acknowledgment

While we are working on several refereed papers describing SourceXtractor++ and its software architecture, the software package can be referenced with two ADASS conference proceedings:

- [Bertin, E.; Schefer, M. ; Apostolakos, N. ; Álvarez-Ayllón, A.; Dubath, P. ; Kümmel, M.: 2020ASPC..527..461B](https://ui.adsabs.harvard.edu/abs/2020ASPC..527..461B/abstract)
- [Kümmel, M. ; Bertin, E.; Schefer, M. ; Apostolakos, N. ; Álvarez-Ayllón, A.; Dubath, P.: 2020ASPC..527...29K](https://ui.adsabs.harvard.edu/abs/2020ASPC..527...29K/abstract) 

The ressources for the SourceXtractor++ development are mostly provided by funding from the Euclid satellite project via the various national agencies.
For Euclid Consortium papers it is sufficient to point to the [SourceXtractor++ webpage](https://github.com/astrorama/SourceXtractorPlusPlus) and add
the standard EC acknowledgement text.

For scientific publication which are non EC papers we would be greatful if the following acknowledgment could be added:

```
This research made use of {\tt SourceXtractor++}\footnote{https://github.com/astrorama/SourceXtractorPlusPlus},
an open source software package developed for the Euclid satellite project.
```