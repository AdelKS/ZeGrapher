#!/bin/bash

set -e

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))

echo "Checking and installing dependencies..."
brew install python coreutils pkg-config create-dmg glaze

# meson imports PyYAML from its own Python, and Homebrew has no PyYAML formula,
# so meson, ninja and PyYAML go in a venv
python_env="${deploy_dir}/python-env"
"$(brew --prefix python)/bin/python3" -m venv "$python_env"
source "$python_env/bin/activate"
pip3 install meson ninja pyyaml aqtinstall

# Homebrew builds Qt for the macOS it runs on. The official Qt runs on older
# ones, so the oldest macOS that runs the bundle is the one meson.build sets
qt_version=$(aqt list-qt mac desktop --spec 6.11 --latest-version)
aqt install-qt mac desktop "$qt_version" clang_64 -m qtshadertools -O "${deploy_dir}/qt"
qt_dir="${deploy_dir}/qt/$qt_version/macos"
export PATH="$qt_dir/bin:$PATH"
export PKG_CONFIG_PATH="$qt_dir/lib/pkgconfig"

version=$(python3 "${deploy_dir}"/../appdata/releases.py --version)
arch=`uname -m`
app_name="ZeGrapher-macOS-$arch-$version"
dmg_name="ZeGrapher-macOS-$arch-$version"

echo "Building ZeGrapher $version for macOS ($arch)..."

meson setup \
  -D optimization=3 \
  -D debug=false \
  -D b_ndebug=true \
  -D loglevel=off \
  -D b_lto=true \
  -D test=true \
  --prefix="${deploy_dir}/$app_name" \
  --bindir="Contents/MacOS" \
  "${deploy_dir}/build-macos" \
  "${deploy_dir}/.."

cd "${deploy_dir}/build-macos"
meson compile
# the official Qt's frameworks load through @rpath, which the tests don't have
DYLD_FRAMEWORK_PATH="$qt_dir/lib" meson test --print-errorlogs
meson install --skip-subprojects
cd ..

echo "Running macdeployqt to bundle Qt frameworks..."
macdeployqt $app_name -verbose=2 -executable="$app_name"/Contents/MacOS/ZeGrapher -qmldir="$deploy_dir/../src/"
# -executable= makes the binary load the bundled frameworks: macdeployqt rewrites
# its library paths with install_name_tool

echo "Creating enhanced DMG installer..."

dmg_staging="${deploy_dir}/dmg-staging"
rm -rf "$dmg_staging"
mkdir -p "$dmg_staging"

cp -R "$app_name" "$dmg_staging/ZeGrapher.app"

# macOS only runs signed apps. An ad-hoc signature doesn't need a developer certificate
echo "Code signing app bundle..."
codesign --force --deep --sign - "$dmg_staging/ZeGrapher.app"

if [ -f "${deploy_dir}/dmg-resources/README.txt" ]; then
    cp "${deploy_dir}/dmg-resources/README.txt" "$dmg_staging/"
fi

dmg_opts=(
    --volname "ZeGrapher $version"
    --volicon "${deploy_dir}/../icons/ZeGrapher.icns"
    --window-pos 200 120
    --window-size 660 450
    --icon-size 128
    --icon "ZeGrapher.app" 150 170
    --hide-extension "ZeGrapher.app"
    --app-drop-link 480 170
    --eula "${deploy_dir}/../LICENSE"
    --hdiutil-quiet
)

if [ -f "${deploy_dir}/dmg-resources/dmg-background.png" ]; then
    dmg_opts+=(--background "${deploy_dir}/dmg-resources/dmg-background.png")
fi

if [ -f "${deploy_dir}/dmg-resources/README.txt" ]; then
    dmg_opts+=(--icon "README.txt" 150 330)
fi

# https://github.com/create-dmg/create-dmg
create-dmg "${dmg_opts[@]}" \
    "$dmg_name.dmg" \
    "$dmg_staging"

rm -rf "$dmg_staging"

echo "DMG created: $dmg_name.dmg"
echo "Installation package complete!"
