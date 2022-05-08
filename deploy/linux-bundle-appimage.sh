#!/bin/bash

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))
[[ -f "${deploy_dir}/build-linux" ]] && rm -r "${deploy_dir}/build-linux"

meson setup "${deploy_dir}/build-linux" "${deploy_dir}/.." -Dprefix="${deploy_dir}/appdir-linux/usr"
cd "${deploy_dir}/build-linux"
meson compile
meson install
cd ..

wget -c -nv "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
chmod a+x linuxdeploy-x86_64.AppImage

wget -c -nv "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
chmod a+x linuxdeploy-plugin-qt-x86_64.AppImage

# ./linuxdeploy-x86_64.AppImage --list-plugins
./linuxdeploy-x86_64.AppImage --appdir appdir-linux --plugin qt --output appimage