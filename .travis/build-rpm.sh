#!/bin/bash
set -ex

# Environment
export VERBOSE=0
export CTEST_OUTPUT_ON_FAILURE=1

# Platform-specific configuration
source /etc/os-release

CMAKEFLAGS="-DINSTALL_DOC=ON -DRPM_NO_CHECK=OFF"

# Default to python 3
PYTHON="python3"

if [ "$ID" == "fedora" ]; then
  if [ "$VERSION_ID" -lt 30 ]; then
    PYTHON="python"
  fi
elif [ "$ID" == "centos" ]; then
  yum install -y epel-release
  if [ "$VERSION_ID" -ge 8 ]; then
    sed -i "s/enabled=0/enabled=1/" /etc/yum.repos.d/CentOS-PowerTools.repo
  else
    PYTHON="python"
  fi
fi

if [ "$PYTHON" == "python3" ]; then
  CMAKEFLAGS="$CMAKEFLAGS -DPYTHON_EXPLICIT_VERSION=3"
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
yum install -y -q cmake make gcc-c++ rpm-build \
  boost-devel $PYTHON-pytest log4cpp-devel CCfits-devel \
  ${PYTHON}-devel boost-${PYTHON}-devel fftw-devel levmar-devel wcslib-devel blas-devel \
  onnxruntime-devel \
  ncurses-devel readline-devel \
  gmock-devel gtest-devel \
  doxygen graphviz $PYTHON-sphinx

if [ "$ID" != "centos" ]; then
  yum install -y -q gsl-devel
fi

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
cmake -DCMAKE_INSTALL_PREFIX=/usr -DINSTALL_TESTS=OFF $CMAKEFLAGS /src
make $MAKEFLAGS rpm | grep -vE "^${PRUNE_REGEX}"

