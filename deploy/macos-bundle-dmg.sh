#!/bin/bash

set -e

deploy_dir=$(greadlink -f $(dirname "$BASH_SOURCE"))

meson setup \
  -D optimization=3 \
  -D debug=false \
  -D b_ndebug=true \
  -D debug_logs=false \
  --prefix="${deploy_dir}/ZeGrapher.app" \
  --bindir="Contents/MacOS" \
  "${deploy_dir}/build-macos" \
  "${deploy_dir}/.."

cd "${deploy_dir}/build-macos"
meson compile
meson install
cd ..

macdeployqt ZeGrapher.app -dmg -verbose=2 -executable=ZeGrapher.app/Contents/MacOS/ZeGrapher -qmldir="$deploy_dir/../src/" -fs=APFS
# the "-executable=" bit rewrites library search paths with 'install_name_tool'
