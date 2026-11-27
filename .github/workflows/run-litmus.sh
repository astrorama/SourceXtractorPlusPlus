#!/bin/sh
set -eu

# Run from the Litmus checkout, using the same installation for pytest and its child processes.
install_dir=$(realpath "$GITHUB_WORKSPACE"/SourceXtractorPlusPlus/InstallArea/*)
export LITMUS_BINARY="$install_dir/bin/sourcextractor++"
test -x "$LITMUS_BINARY"
results="$GITHUB_WORKSPACE/litmus-results"
mkdir -p "$results"

# Litmus reads this section itself and expands environment variables in the paths.
sed -i -e 's|^binary=.*|binary=${LITMUS_BINARY}|' \
       -e 's|^output_area=.*|output_area=${GITHUB_WORKSPACE}/litmus-results|' \
       -e 's|^pythonpath=.*|pythonpath=|' pytest.ini
# Check images consume too much space on a hosted runner.
sed -i 's/^check-image/#check-image/' sourcex.config

# Include slow iterative Levmar tests. Other slow cases need a separate opt-in.
exec python3 "$CMAKE_PREFIX_PATH/scripts/env.py" --xml "$install_dir/SourceXtractorPlusPlusEnvironment.xml" \
  .venv/bin/python -m pytest -v --run-slow -k 'not gsl and not levmar_classic and (not slow or levmar_iterative)' \
  -W ignore::RuntimeWarning \
  --html="$results/report.html" --self-contained-html --junitxml="$results/junit.xml"
