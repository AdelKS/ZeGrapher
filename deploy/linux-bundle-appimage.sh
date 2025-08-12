#!/bin/bash

set -e

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))

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

appimagetool=$(echo appimagetool-*.AppImage)
if [[ ! -f "$appimagetool" ]]; then
  wget -c https://github.com/$(wget -q https://github.com/probonopd/go-appimage/releases/expanded_assets/continuous -O - | grep "appimagetool-.*-x86_64.AppImage" | head -n 1 | cut -d '"' -f 2)
  chmod +x appimagetool-*.AppImage
fi

export QTDIR=/usr/lib/qt6/

./"$appimagetool" -s deploy ./appdir-linux/usr/share/applications/*.desktop

# Workaround for Wayland
rm -rf ./appdir-linux/usr/lib/qt6/plugins
cp -r /usr/lib/qt6/plugins ./appdir-linux/usr/lib/qt6/

export VERSION=$(meson introspect ../meson.build --projectinfo | jq -r .version)

./"$appimagetool" ./appdir-linux
