#!/bin/bash
set -e

if [[ -z "${BINARY_TAG}" ]]; then
    echo "ERROR: Please, define BINARY_TAG"
    exit 2
fi

LINUXDEPLOY=$(which linuxdeploy-x86_64.AppImage 2> /dev/null)
if [[ $? -ne 0 ]]; then
    echo "ERROR: linuxdeploy-x86_64.AppImage not in PATH"
    exit 2
fi

PIP3=$(which pip3 2> /dev/null)
if [[ $? -ne 0 ]]; then
    echo "ERROR: pip3 required"
    exit 2
fi

echo "Using ${LINUXDEPLOY}"
echo "Using ${PIP3}"

ISDEBUG=0
if [[ "${BINARY_TAG}" =~ dbg$ ]]; then
    ISDEBUG=1
    echo "WARNING: Using a debug binary tag ($BINARY_TAG)"
    echo "WARNING: This is the one that will be used for the AppImage"
    echo "WARNING: For distributing, it is recommended to use o2g"
    echo "WARNING: Make sure the dependencies are also available with that level of optimization"
    read -p "WARNING: Are you sure? [yN]" -n 1
    echo
    if [[ ! $REPLY =~ [Yy]$ ]]; then
        echo "Abort"
        exit 2
    fi
fi

# Set KEEPDEBUG before running this script to 1 if you want to strip the debug
# information anyways
KEEPDEBUG=${KEEPDEBUG:-$ISDEBUG}

# Directories
BUILDDIR="$(pwd)/build.appimage"
APPDIR="$(pwd)/InstallArea/AppDir"
RSCDIR="$(pwd)/SEMain/auxdir/SEMain"
SCRIPTDIR="$(pwd)/SEMain/scripts/"

# Build and install
mkdir -p "${BUILDDIR}"

pushd "${BUILDDIR}"

cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DELEMENTS_BUILD_TESTS=OFF -DPYTHON_EXPLICIT_VERSION=3
make -j$(nproc)
make install DESTDIR="${APPDIR}"

popd

# Install into the InstallDir the required Python modules, so we really
# are self-contained
virtualenv --clear --no-pip --no-wheel --no-setuptools --python="$(which python3)" "${APPDIR}"
source "${APPDIR}/bin/activate"

pip3 --isolated install --ignore-installed --prefix "/usr" --root "${APPDIR}" astropy

deactivate

virtualenv --relocatable "${APPDIR}"

sed -i 's/VIRTUAL_ENV=.*/VIRTUAL_ENV="${APPIMAGE_MOUNT}"/g' "${APPDIR}/bin/activate"

# virtualenv create symlinks to the system Python installation
# Replace them with the actual content
# virtualenv has --always-copy, but it is quite buggy and the result ends being unusable
PYTHONDIRNAME="$(ls -1 ${APPDIR}/usr/lib64/ | grep '^python')"
for symlink in $(find "InstallArea/AppDir/lib/${PYTHONDIRNAME}" -type l); do
    original="$(readlink "${symlink}")"
    echo "Replacing symlink with copy ${symlink} => ${original}"
    rm "${symlink}"
    cp -rfv "${original}" "${symlink}"
done

# These files are missing for some reason ¯\_(ツ)_/¯
DISTUTILSDIR="$(dirname "$(python3 -c 'import distutils; print(distutils.__file__)')")"
cp -vf ${DISTUTILSDIR}/*.py "${APPDIR}/lib/${PYTHONDIRNAME}/distutils"

# For non-debug, strip the debug information from the libraries and binaries
if [[ $KEEPDEBUG == 0 ]]; then
    echo "Stripping debug information"
    for lib in $(find "${APPDIR}" -type f -regex '.*\.so\(\.[0-9.]+\)?$'); do
        strip --verbose --strip-debug "${lib}" || true
    done
fi

# Bundle
linuxdeploy-x86_64.AppImage \
    --appdir "${APPDIR}" \
    -d "${RSCDIR}/SExtractor.desktop" -i "${RSCDIR}/SExtractor.png" \
    --custom-apprun="${SCRIPTDIR}/AppRun" \
    -o appimage

# Try it
unset PYTHONPATH
./SExtractor-x86_64.AppImage --version
if [[ $? != 0 ]]; then
    echo "Failed to build the AppImage!"
    exit 1
else
    echo "Success!"
    exit 0
fi
