#!/bin/bash
set -ex

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
  echo "Three parameters required: package, version and target repository"
  exit 1
fi

TARGET_PACKAGE=$1
TARGET_VERSION=$2
TARGET_REPO=$3

# The changes happened, or have been pulled into, this branch
TARGET_BRANCH=$4
# In the case of pull requests, this is the origin branch
ORIGIN_BRANCH=$5

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
  # Prevent conflicts
  sed -i "/\[epel\]/a exclude=elements*" /etc/yum.repos.d/epel.repo
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
  GIT_LFS_SKIP_SMUDGE=1 git clone --depth=1 https://github.com/astrorama/SourceXtractor-litmus.git /tmp/sourcextractor-litmus
fi
cd /tmp/sourcextractor-litmus

# Try to use a better set of tests by this order
# 1: Those contained on a branch that matches the origin of the pull request (if it is a pull)
# 2: Those contained on a branch that matches the destination of the pull request (or the name of the branch)
# 3: The default branch for the tests (i.e. master)
BRANCHES=("${ORIGIN_BRANCH}" "${TARGET_BRANCH}" "master")
for b in ${BRANCHES[@]}; do
  echo "Try $b"
  git fetch --update-head-ok origin "$b:$b" && git checkout "$b" && break
done

git status
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
if command -v pytest-3 &> /dev/null; then
  PYTEST=pytest-3
elif command -v py.test &> /dev/null; then
  PYTEST=py.test
else
  PYTEST=pytest
fi

${PYTEST} -v -m "not report" -k "not gsl"
