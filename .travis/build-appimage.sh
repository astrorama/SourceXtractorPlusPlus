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
fi


# Astrorama repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF

# Install some dependencies for this script and linuxdeploy
yum install -y fuse-libs which file

# Download linuxdeploy
curl -OL 'https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage'
chmod a+x 'linuxdeploy-x86_64.AppImage'

# Create a virtualenv with the dependencies are self-contained
APPDIR=`mktemp -d`

# Install sextractorxx, and dependencies, there
yum install --installroot "$APPDIR" --releasever "$VERSION_ID" -y SExtractorxx ${PYTHON}-numpy ${PYTHON}-astropy ${PYTHON_ENUM}

# Get rid of some files to reduce the footprint
for d in "sbin" "var" "etc" "usr/share" "usr/sbin" "usr/local"; do
    rm -rvf "${APPDIR}/$d"
done

# Bundle
SRCDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/../"
RSCDIR="${SRCDIR}/SEMain/auxdir/SEMain"
SCRIPTDIR="${SRCDIR}/SEMain/scripts/"

LD_LIBRARY_PATH="${APPDIR}/usr/lib64" ./linuxdeploy-x86_64.AppImage \
    --appdir "${APPDIR}" \
    -d "${RSCDIR}/sextractor++.desktop" -i "${RSCDIR}/sextractor++.png" \
    --custom-apprun="${SCRIPTDIR}/AppRun" \
    -e "${APPDIR}/usr/bin/sextractor++" \
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

