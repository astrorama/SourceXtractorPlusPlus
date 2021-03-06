# SourceXtractor++
[![Conda](https://img.shields.io/conda/v/astrorama/sourcextractor)](https://anaconda.org/astrorama/sourcextractor)
[![Fedora package](https://img.shields.io/fedora/v/sourcextractor++/rawhide)](https://koji.fedoraproject.org/koji/packageinfo?packageID=30842)
[![Build RPM](https://github.com/astrorama/SourceXtractorPlusPlus/workflows/Build%20RPM/badge.svg)](https://github.com/astrorama/SourceXtractorPlusPlus/actions)
[![Coverity](https://scan.coverity.com/projects/19270/badge.svg?flat=1)](https://scan.coverity.com/projects/astrorama-sextractorxx)
[![Documentation](https://readthedocs.org/projects/sourcextractorplusplus/badge/?version=latest)](https://sourcextractorplusplus.readthedocs.io/en/latest/Introduction.html)
[![CodeFactor](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/badge/develop)](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/overview/develop)

The next generation SExtractor.

Documentation: https://sourcextractorplusplus.readthedocs.io/en/latest/Introduction.html

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

[We also build our own RPMs as part of our CI setup](https://astrorama.jfrog.io/artifactory/rpm/stable/)
They can be installed adding a `.repo` file under `/etc/yum.repos.d` with the following content:

```ini
[Artifactory-Astrorama]
name=Artifactory-Astrorama
baseurl=https://astrorama.jfrog.io/artifactory/rpm/stable/fedora/$releasever/$basearch
enabled=1
gpgcheck=0
```

And install with

```bash
dnf install SourceXtractorPlusPlus
```

