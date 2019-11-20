#!/bin/bash
set -ex

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Two parameters required: package and version"
  exit 1
fi

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

# Install dependencies
yum install -y -q git git-lfs ${PYTHON}-pytest ${PYTHON}-astropy ${PYTHON}-numpy ${PYTHON}-matplotlib
if [ "${PYTHON}" == "python2" ]; then
  yum install -y -q python2-enum34 python2-pathlib python2-configparser
fi

# Get the relevant version
yum install -y "$1-$2"

# Checkout the tests
if ! [ -d /tmp/sourcextractor-litmus ]; then
  git clone --depth=1 https://github.com/astrorama/SourceXtractor-litmus.git /tmp/sourcextractor-litmus
fi
cd /tmp/sourcextractor-litmus
git lfs pull

# Patch config file
sed -i "s:binary=.*:binary=/usr/bin/sourcextractor++:" pytest.ini
sed -i "s:output_area=.*:output_area=/tests:" pytest.ini
sed -i "s:pythonpath=.*:pythonpath=:" pytest.ini
sed -i '/\[pytest\]/a filterwarnings=ignore::RuntimeWarning' pytest.ini

cat pytest.ini

# Run skipping the report generation
if ! [ -x $(command -v pytest-3) ]; then
  PYTEST=pytest
else
  PYTEST=pytest-3
fi

${PYTEST} -v -m "not report"
