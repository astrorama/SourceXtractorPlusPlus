#!/bin/bash
set -ex

# Environment
export VERBOSE=1
export CTEST_OUTPUT_ON_FAILURE=1

# Platform-specific configuration
source /etc/os-release

CMAKEFLAGS="-DINSTALL_DOC=ON"

if [ $NAME == 'Fedora' ] && [ $VERSION_ID -ge 30 ]; then
  PYTHON="python3"
  CMAKEFLAGS="$CMAKEFLAGS -DPYTHON_EXPLICIT_VERSION=3"
else
  PYTHON="python2"
fi

# Astrorama repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/fedora/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1

[bintray--astrorama-fedora-develop]
name=bintray--astrorama-fedora-develop
baseurl=https://dl.bintray.com/astrorama/travis/develop/fedora/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF

# From the CMakeLists.txt, retrieve the list of dependencies
cmake_deps=$(grep -oP 'elements_project\(\S+\s+\S+ USE \K(\S+ \S+)*(?=\))' /src/CMakeLists.txt)
rpm_dev_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-devel-" $(i+1)}}')
rpm_doc_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-doc-" $(i+1)}}')
yum install -y ${rpm_dev_deps} ${rpm_doc_deps}

# Dependencies
yum install -y @development-tools cmake gcc-c++ rpm-build
yum install -y boost-devel $PYTHON-pytest log4cpp-devel doxygen CCfits-devel
yum install -y graphviz $PYTHON-sphinx $PYTHON-sphinxcontrib-apidoc
yum install -y gmock-devel gtest-devel
yum install -y ${PYTHON}-devel boost-${PYTHON}-devel fftw-devel levmar-devel wcslib-devel

# Build
mkdir -p /build
cd /build
cmake -DCMAKE_INSTALL_PREFIX=/usr $CMAKEFLAGS /src
make $MAKEFLAGS rpm

