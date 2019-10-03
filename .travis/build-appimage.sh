#!/bin/bash
set -ex

# Platform-specific configuration
source /etc/os-release

CMAKEFLAGS="-DINSTALL_DOC=ON -DRPM_NO_CHECK=OFF"

if [ $NAME == 'Fedora' ] && [ $VERSION_ID -ge 30 ]; then
  PYTHON="python3"
  PIP="pip3" 
else
  PYTHON="python2"
  PIP="pip2"
fi


# Astrorama repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/fedora/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF

# Install pip, virtualenv, fuse and which
yum install -y ${PYTHON}-pip python3-virtualenv fuse-libs which

# Install sextractor from rpms
yum install -y SExtractorxx

# Download linuxdeploy
curl -OL 'https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage'
chmod a+x 'linuxdeploy-x86_64.AppImage'

# Create a virtualenv with the dependencies
# are self-contained
APPDIR=`mktemp -d`
virtualenv --clear --no-pip --no-wheel --no-setuptools --python="${PYTHON}" "${APPDIR}"
source "${APPDIR}/bin/activate"

"${PIP}" --isolated install --ignore-installed --prefix "/usr" --root "${APPDIR}" astropy

deactivate

virtualenv --relocatable "${APPDIR}"

sed -i 's/VIRTUAL_ENV=.*/VIRTUAL_ENV="${APPIMAGE_MOUNT}"/g' "${APPDIR}/bin/activate"

# virtualenv create symlinks to the system Python installation
# Replace them with the actual content
# virtualenv has --always-copy, but it is quite buggy and the result ends being unusable
PYTHONDIRNAME="$(ls -1 ${APPDIR}/usr/lib64/ | grep '^python')"
for symlink in $(find "${APPDIR}/lib/${PYTHONDIRNAME}" -type l); do
    original="$(readlink "${symlink}")"
    echo "Replacing symlink with copy ${symlink} => ${original}"
    rm -f "${symlink}"
    cp -rfv "${original}" "${symlink}"
done

# These files are missing for some reason ¯\_(ツ)_/¯
DISTUTILSDIR="$(dirname "$(${PYTHON} -c 'import distutils; print(distutils.__file__)')")"
cp -vf ${DISTUTILSDIR}/*.py "${APPDIR}/lib/${PYTHONDIRNAME}/distutils"

# Bundle
SRCDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/../"
RSCDIR="${SRCDIR}/SEMain/auxdir/SEMain"
SCRIPTDIR="${SRCDIR}/SEMain/scripts/"

./linuxdeploy-x86_64.AppImage \
    --appdir "${APPDIR}" \
    -d "${RSCDIR}/sextractor++.desktop" -i "${RSCDIR}/sextractor++.png" \
    --custom-apprun="${SCRIPTDIR}/AppRun" \
    -e $(which sextractor++) \
    -o appimage

# Try it
unset PYTHONPATH
./sextractor++-x86_64.AppImage --version
if [[ $? != 0 ]]; then
    echo "Failed to build the AppImage!"
    exit 1
else
    echo "Success!"
    exit 0
fi

