#!/bin/bash

set -e

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))

version=$(bash "${deploy_dir}"/../version.sh get-vcs)

[[ -d "${deploy_dir}/appdir-linux" ]] && rm -rf "${deploy_dir}/appdir-linux"

meson setup \
  -D optimization=3 \
  -D debug=false \
  -D b_ndebug=true \
  -D loglevel=off \
  -D prefix="${deploy_dir}/appdir-linux/usr" \
  "${deploy_dir}/build-linux" \
  "${deploy_dir}/.."

cd "${deploy_dir}/build-linux"
meson compile
meson install
cd ..

wget -c -nv "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
chmod a+x linuxdeploy-x86_64.AppImage

wget -c -nv "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
chmod a+x linuxdeploy-plugin-qt-x86_64.AppImage

export QMAKE=`which qmake6`
export EXTRA_QT_PLUGINS="waylandcompositor"
export EXTRA_PLATFORM_PLUGINS="libqwayland-egl.so;libqwayland-generic.so"
export QML_SOURCES_PATHS="${deploy_dir}/../src/"
export NO_STRIP=1

# ./linuxdeploy-x86_64.AppImage --list-plugins
./linuxdeploy-x86_64.AppImage --appdir appdir-linux --plugin qt --output appimage

mv ZeGrapher-x86_64.AppImage ZeGrapher-x86_64-$version.AppImage
