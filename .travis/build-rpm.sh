#!/bin/bash
set -ex

# Environment
export VERBOSE=0
export CTEST_OUTPUT_ON_FAILURE=1

# Platform-specific configuration
source /etc/os-release

CMAKEFLAGS="-DINSTALL_DOC=ON -DRPM_NO_CHECK=OFF"

if [ "$ID" == "fedora" ]; then
  if [ "$VERSION_ID" -ge 30 ]; then
    PYTHON="python3"
    CMAKEFLAGS="$CMAKEFLAGS -DPYTHON_EXPLICIT_VERSION=3"
  else
    PYTHON="python2"
  fi
elif [ "$ID" == "centos" ]; then
  yum install -y -q epel-release
  PYTHON="python2"
fi

# Astrorama repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1

[bintray--astrorama-fedora-develop]
name=bintray--astrorama-fedora-develop
baseurl=https://dl.bintray.com/astrorama/travis/develop/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF

# From the CMakeLists.txt, retrieve the list of dependencies
cmake_deps=$(grep -oP 'elements_project\(\S+\s+\S+ USE \K(\S+ \S+)*(?=\))' /src/CMakeLists.txt)
rpm_dev_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-devel-" $(i+1)}}')
rpm_doc_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-doc-" $(i+1)}}')
yum install -y -q ${rpm_dev_deps} ${rpm_doc_deps}

# Dependencies
yum install -y -q cmake make gcc-c++ rpm-build
yum install -y -q boost-devel $PYTHON-pytest log4cpp-devel doxygen CCfits-devel
yum install -y -q graphviz $PYTHON-sphinx $PYTHON-sphinxcontrib-apidoc
yum install -y -q gmock-devel gtest-devel
yum install -y -q ${PYTHON}-devel boost-${PYTHON}-devel fftw-devel levmar-devel wcslib-devel
if [ "$ID" != "centos" ]; then
  yum install -y -q gsl-devel
fi
yum install -y -q ncurses-devel readline-devel

# The build log can become quite big, exceeden the limit of Travis.
# We need to trim the output a bit
PRUNE_LOG=(
    "Searching for"
    "Parsing"
    "Preprocessing"
    "Generat"
    "Running dot"
    "Patching"
    "Installing"
)
PRUNE_REGEX=$(printf "(%s)|" "${PRUNE_LOG[@]}")
PRUNE_REGEX=${PRUNE_REGEX:0:$((${#PRUNE_REGEX}-1))}

# Build
mkdir -p /build
cd /build
cmake -DCMAKE_INSTALL_PREFIX=/usr $CMAKEFLAGS /src
make $MAKEFLAGS rpm | grep -vE "^${PRUNE_REGEX}"

