#!/bin/bash
set -ex

# The changes happened, or have been pulled into, this branch
TARGET_BRANCH=${GITHUB_BASE_REF:-$GITHUB_REF}
# In the case of pull requests, this is the origin branch
ORIGIN_BRANCH=${GITHUB_HEAD_REF:-$GITHUB_REF}

# Platform-specific configuration
source /etc/os-release

PYTHON="python3"
if [ "$ID" == "fedora" ] && [ "$VERSION_ID" -lt 30 ]; then
  PYTHON="python"
elif [ "$ID" == "centos" ] && [ "$VERSION_ID" -lt 8 ]; then
  PYTHON="python"
fi

# Checkout the tests
if ! [ -d /tmp/sourcextractor-litmus ]; then
  GIT_LFS_SKIP_SMUDGE=1 git clone --depth=1 https://github.com/astrorama/SourceXtractor-litmus.git /tmp/sourcextractor-litmus
fi
cd /tmp/sourcextractor-litmus

# Try to use a better set of tests by this order
# 1: Those contained on a branch that matches the origin of the pull request (if it is a pull)
# 2: Those contained on a branch that matches the destination of the pull request (or the name of the branch)
# 3: The default branch for the tests (i.e. master)
BRANCHES=("${ORIGIN_BRANCH}" "${TARGET_BRANCH}" "refs/heads/master")
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
sed -i "s:output_area=.*:output_area=/tmp/sourcex/:" pytest.ini
sed -i "s:pythonpath=.*:pythonpath=:" pytest.ini
sed -i '/\[pytest\]/a filterwarnings=ignore::RuntimeWarning' pytest.ini

cat pytest.ini

# Disable check images, they take too much space
sed -i "s:^check-image:#check-image:g" sourcex.config

# Run
if command -v pytest-3 &> /dev/null; then
  PYTEST=pytest-3
elif command -v py.test &> /dev/null; then
  PYTEST=py.test
else
  PYTEST=pytest
fi

${PYTEST} -v -k "not gsl" --html=/tmp/sourcex/report.html
