#!/bin/bash

deploy_dir=$(greadlink -f $(dirname "$BASH_SOURCE"))
[[ -f "${deploy_dir}/build-macos" ]] && rm -r "${deploy_dir}/build-macos"

meson setup "${deploy_dir}/build-macos" "${deploy_dir}/.." --prefix="${deploy_dir}/ZeGrapher.app" --bindir="Contents/MacOS"
cd "${deploy_dir}/build-macos"
meson compile
meson install
cd ..

macdeployqt ZeGrapher.app -dmg