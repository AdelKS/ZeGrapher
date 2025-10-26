#!/bin/bash
# This script needs to be run inside an msys2 bash terminal

set -e

pacboy -S --noconfirm gcc meson qt6-base qt6-svg qt6-declarative qt6-tools qt6-translations boost 7zip

deploy_script_dir=$(readlink -f $(dirname "$BASH_SOURCE"))

version=$(bash "${deploy_script_dir}"/../version.sh get-vcs)
output_folder_name="ZeGrapher-Windows-$MSYSTEM-$version"
deploy_dir="$deploy_script_dir"/"$output_folder_name"

[[ -d "${deploy_dir}" ]] && rm -rf "${deploy_dir}"

meson setup \
  -D optimization=3 \
  -D debug=false \
  -D b_ndebug=true \
  -D loglevel=off \
  "${deploy_script_dir}/build-windows" \
  "${deploy_script_dir}/.."

cd "${deploy_script_dir}/build-windows"
meson compile
cd ..

mkdir "${deploy_dir}"
cd "${deploy_dir}"
cp "${deploy_script_dir}"/build-windows/src/ZeGrapher.exe .

# Copy extra DLLs over
ldd ZeGrapher.exe | awk '{print $3}' | grep -v -i WINDOWS/SYSTEM32 | xargs -I{} cp -u {} .

# fix upstream bug, see https://github.com/msys2/MINGW-packages/pull/26127
ln -sf $MINGW_PREFIX/share/qt6/bin/qmlimportscanner.exe $MINGW_PREFIX/bin/qmlimportscanner.exe

windeployqt6.exe --qmldir "${deploy_script_dir}"/../src/QML/ --compiler-runtime ZeGrapher.exe

# move qml modules out of the 'qml' folder
mv qml qml-temp
mv qml-temp/* .
rm -d qml-temp

cd ..

7z a "$output_folder_name".exe -sfx../lib/7zip/7zCon.sfx "${deploy_dir}"
