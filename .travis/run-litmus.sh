#!/bin/bash
set -ex

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
  echo "Three parameters required: package, version and target repository"
  exit 1
fi

TARGET_PACKAGE=$1
TARGET_VERSION=$2
TARGET_REPO=$3
TARGET_BRANCH=$4 # Optional

# Platform-specific configuration
source /etc/os-release

if [ "$ID" == "fedora" ]; then
  if [ "$VERSION_ID" -ge 30 ]; then
    PYTHON="python3"
  else
    PYTHON="python2"
  fi
elif [ "$ID" == "centos" ]; then
  yum install -y -q epel-release
  if [ "$VERSION_ID" -ge 8 ]; then
    PYTHON="python3"
  else
    PYTHON="python2"
  fi
fi

# Always master repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
priority=10
EOF

# If not master, always append develop
if [ "${TARGET_REPO}" != "master" ]; then
  cat >> /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora-develop]
name=bintray--astrorama-fedora-develop
baseurl=https://dl.bintray.com/astrorama/travis/develop/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
priority=10
EOF

# Append an additional repository if we are testing the build of a specific branch or pull request
  if  [ "${TARGET_REPO}" != "develop" ]; then
    cat >> /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora-target]
name=bintray--astrorama-fedora-target
baseurl=https://dl.bintray.com/astrorama/travis/${TARGET_REPO}/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
priority=1
EOF
  fi
fi

cat /etc/yum.repos.d/astrorama.repo

# Install dependencies
yum install -y -q git git-lfs ${PYTHON}-pytest ${PYTHON}-astropy ${PYTHON}-numpy ${PYTHON}-matplotlib ${PYTHON}-psutil ${PYTHON}-pip

# Get the relevant version
yum install -y "${TARGET_PACKAGE}-${TARGET_VERSION}"

# Checkout the tests
if ! [ -d /tmp/sourcextractor-litmus ]; then
  git clone --depth=1 https://github.com/astrorama/SourceXtractor-litmus.git /tmp/sourcextractor-litmus
fi
cd /tmp/sourcextractor-litmus
if [ -n "${TARGET_BRANCH}" ]; then
  echo "Trying to use a branch for the tests that matches the target branch"
  (git fetch origin "${TARGET_BRANCH}:${TARGET_BRANCH}" && git checkout "${TARGET_BRANCH}") || true
fi
git lfs pull

# Install requirements
if [ -f "requirements.txt" ]; then
  ${PYTHON} -m pip install --user -r "requirements.txt"
fi

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

${PYTEST} -v -m "not report" -k "not gsl"
