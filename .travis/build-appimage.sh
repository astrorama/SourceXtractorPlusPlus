#!/bin/bash
set -ex

# Platform-specific configuration
source /etc/os-release

CMAKEFLAGS="-DINSTALL_DOC=ON -DRPM_NO_CHECK=OFF"

if [ "$ID" == 'fedora' ]; then
  if [ "$VERSION_ID" -ge 30 ]; then
      PYTHON="python3"
      PYTHON_ENUM="python-enum"
  else
      PYTHON="python2"
      PYTHON_ENUM="python2-enum34"
  fi
elif [ "$ID" == "centos" ]; then
  PYTHON="python2"
  PYTHON_ENUM="python2-enum34"
  yum install -y epel-release
  # Prevent conflicts
  sed -i "/\[epel\]/a exclude=elements*" /etc/yum.repos.d/epel.repo
fi


# Astrorama repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
priority=1
EOF

# Install some dependencies for this script and linuxdeploy
yum install -y fuse-libs which file

# Download linuxdeploy
curl -OL 'https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage'
chmod a+x 'linuxdeploy-x86_64.AppImage'

# Create a virtualenv with the dependencies are self-contained
APPDIR=`mktemp -d`

# Install sourcextractor++, and dependencies, there
yum install -y SourceXtractorPlusPlus ${PYTHON}-numpy ${PYTHON}-astropy ${PYTHON_ENUM} blas

# Copy Python into the AppImage, and some of its dependencies
mkdir -p "${APPDIR}/usr/lib64"
mkdir -p "${APPDIR}/usr/lib"
cp -vr /usr/lib64/python* "${APPDIR}/usr/lib64"
cp -vr /usr/lib/python* "${APPDIR}/usr/lib"

./linuxdeploy-x86_64.AppImage --appdir "${APPDIR}" -e "$(which ${PYTHON})"
ls /usr/lib64/atlas/* | xargs -L1 ./linuxdeploy-x86_64.AppImage --appdir "${APPDIR}" -l
ls /usr/lib64/liberfa.so.1 | xargs -L1 ./linuxdeploy-x86_64.AppImage --appdir "${APPDIR}" -l
for l in $(find /usr/lib64/python2.7/ -name "*.so"); do
    echo "Deploy $l"
    ./linuxdeploy-x86_64.AppImage --appdir "${APPDIR}" -l "$l" &> /dev/null
done

# Bundle
SRCDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/../"
RSCDIR="${SRCDIR}/SEMain/auxdir/SEMain"
SCRIPTDIR="${SRCDIR}/SEMain/scripts/"

./linuxdeploy-x86_64.AppImage \
    --appdir "${APPDIR}" \
    -d "${RSCDIR}/sourcextractor++.desktop" -i "${RSCDIR}/sourcextractor++.png" \
    --custom-apprun="${SCRIPTDIR}/AppRun" \
    -e "/usr/bin/sourcextractor++" \
    -o appimage

# Try it
unset PYTHONPATH
./SourceXtractor++-x86_64.AppImage --version
if [[ $? != 0 ]]; then
    echo "Failed to build the AppImage!"
    exit 1
else
    echo "Success!"
    exit 0
fi

