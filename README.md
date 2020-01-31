# SourceXtractor++
[![Conda](https://img.shields.io/conda/v/astrorama/sourcextractor)](https://anaconda.org/astrorama/sourcextractor)
[![Build Status](https://travis-ci.org/astrorama/SourceXtractorPlusPlus.svg?branch=develop)](https://travis-ci.org/astrorama/SourceXtractorPlusPlus)
[![Coverity](https://scan.coverity.com/projects/19270/badge.svg?flat=1)](https://scan.coverity.com/projects/astrorama-sextractorxx)
[![Documentation](https://readthedocs.org/projects/sourcextractorplusplus/badge/?version=latest)](https://sourcextractorplusplus.readthedocs.io/en/latest/Introduction.html)
[![CodeFactor](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/badge/develop)](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/overview/develop)

The next generation SExtractor.

Documentation: https://sourcextractorplusplus.readthedocs.io/en/latest/Introduction.html


[RPMs for Fedora and CentOS are available](https://dl.bintray.com/astrorama/travis/master/) You can add a `.repo` file under `/etc/yum.repos.d` with the following content:

```ini
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/fedora/$releasever/$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
```

And install normally with

```bash
dnf install SourcextractorPlusPlus
```

SourceXtractor++ is also available on [Anaconda Cloud for Linux and MacOSX](https://anaconda.org/astrorama/sourcextractor)
```bash
 conda install -c astrorama sourcextractor 
 ```
