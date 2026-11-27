#!/bin/sh
# Install the two native dependencies missing from Ubuntu 24.04.
set -eu

prefix=$1
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"
mkdir -p "$prefix/lib" "$prefix/include"

curl -fsSL --retry 3 -o levmar.tgz https://users.ics.forth.gr/~lourakis/levmar/levmar-2.6.tgz
echo '3bf4ef1ea4475ded5315e8d8fc992a725f2e7940a74ca3b0f9029d9e6e94bad7  levmar.tgz' | sha256sum -c -
tar -xzf levmar.tgz
# Use reentrant LAPACK solvers, as in Astrorama's Levmar package.
sed -i '/^#define LINSOLVERS_RETAIN_MEMORY$/d' levmar-2.6/levmar.h
make -C levmar-2.6 -j2 ARCHFLAGS="-fPIC -std=gnu99" liblevmar.a
cc -shared -Wl,-z,defs,-soname,liblevmar.so.2 -o "$prefix/lib/liblevmar.so.2" \
  -Wl,--whole-archive levmar-2.6/liblevmar.a -Wl,--no-whole-archive -llapack -lblas -lm
ln -sf liblevmar.so.2 "$prefix/lib/liblevmar.so"
install -m 644 levmar-2.6/levmar.h "$prefix/include/"

# Microsoft's CPU package contains matching C++ headers and a native Linux x64 library.
nuget=https://api.nuget.org/v3-flatcontainer/microsoft.ml.onnxruntime/1.16.3
curl -fsSL --retry 3 -o onnxruntime.nupkg "$nuget/microsoft.ml.onnxruntime.1.16.3.nupkg"
echo '84d247f3650fa15c00a4568a80f304050e1da4dca540bbe6d17493da8350242a  onnxruntime.nupkg' | sha256sum -c -
unzip -q onnxruntime.nupkg 'build/native/include/*.h' runtimes/linux-x64/native/libonnxruntime.so
install -m 644 build/native/include/*.h "$prefix/include/"
install -m 755 runtimes/linux-x64/native/libonnxruntime.so "$prefix/lib/libonnxruntime.so.1.16.3"
ln -sf libonnxruntime.so.1.16.3 "$prefix/lib/libonnxruntime.so"
