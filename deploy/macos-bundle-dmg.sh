#!/bin/bash

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))
[[ -f "${deploy_dir}/build-macos" ]] && rm -r "${deploy_dir}/build-macos"

meson setup "${deploy_dir}/build-macos" "${deploy_dir}/.." -Dprefix="${deploy_dir}/appdir-macos/usr"
cd "${deploy_dir}/build-macos"
meson compile
meson install
cd ..

macdeployqt build-macos/src/ZeGrapher.app -dmg