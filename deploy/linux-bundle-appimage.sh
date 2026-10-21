#!/bin/bash

set -e

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))

# meson imports PyYAML from its own Python, so meson, ninja and PyYAML go in a venv
python_env="${deploy_dir}/python-env"
python3 -m venv "$python_env"
source "$python_env/bin/activate"
pip3 install meson ninja pyyaml

version=$(python3 "${deploy_dir}"/../appdata/releases.py --version)

[[ -d "${deploy_dir}/appdir-linux" ]] && rm -rf "${deploy_dir}/appdir-linux"

meson setup \
  -D optimization=3 \
  -D debug=false \
  -D b_ndebug=true \
  -D loglevel=off \
  -D b_lto=true \
  -D test=true \
  -D prefix="${deploy_dir}/appdir-linux/usr" \
  "${deploy_dir}/build-linux" \
  "${deploy_dir}/.."

cd "${deploy_dir}/build-linux"
meson compile
meson test --print-errorlogs
meson install --skip-subprojects
cd ..

arch=$(uname -m)

wget -c -nv "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-$arch.AppImage"
chmod a+x linuxdeploy-$arch.AppImage

wget -c -nv "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-$arch.AppImage"
chmod a+x linuxdeploy-plugin-qt-$arch.AppImage

export QMAKE=`which qmake6`
export EXTRA_QT_PLUGINS="waylandcompositor"
export EXTRA_PLATFORM_PLUGINS="libqwayland.so"
export QML_SOURCES_PATHS="${deploy_dir}/../src/"
export NO_STRIP=1
# don't bundle glib, libdbus and their dependencies. The AppImage then loads the
# host's versions. The Ubuntu 26.04 versions need glibc 2.43, which older distros lack.
export LINUXDEPLOY_EXCLUDED_LIBRARIES="libglib-2.0.so*;libgio-2.0.so*;libgobject-2.0.so*;libgmodule-2.0.so*;libgthread-2.0.so*;libmount.so*;libblkid.so*;libselinux.so*;libffi.so*;libpcre2-8.so*;libdbus-1.so*;libsystemd.so*"
# the AppImages of linuxdeploy run without FUSE, which CI runners lack
export APPIMAGE_EXTRACT_AND_RUN=1

# ./linuxdeploy-$arch.AppImage --list-plugins
./linuxdeploy-$arch.AppImage --appdir appdir-linux --plugin qt --output appimage

# the other bundles name the architecture 'arm64'
mv ZeGrapher-$arch.AppImage ZeGrapher-Linux-${arch/aarch64/arm64}-$version.AppImage
