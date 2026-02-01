#!/bin/bash

set -e

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))
version=$(bash "${deploy_dir}"/../version.sh get-vcs)
arch=`uname -m`
app_name="ZeGrapher-macOS-$arch-$version"

meson setup \
  -D optimization=3 \
  -D debug=false \
  -D b_ndebug=true \
  -D loglevel=off \
  --prefix="${deploy_dir}/$app_name" \
  --bindir="Contents/MacOS" \
  "${deploy_dir}/build-macos" \
  "${deploy_dir}/.."

cd "${deploy_dir}/build-macos"
meson compile
meson install
cd ..

macdeployqt $app_name -dmg -verbose=2 -executable="$app_name"/Contents/MacOS/ZeGrapher -qmldir="$deploy_dir/../src/" -fs=APFS
# the "-executable=" bit rewrites library search paths with 'install_name_tool'
