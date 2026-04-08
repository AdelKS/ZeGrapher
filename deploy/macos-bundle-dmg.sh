#!/bin/bash

set -e

# Install dependencies if not already present
echo "Checking and installing dependencies..."
brew install boost coreutils qtbase qtsvg qtdeclarative qttools qttranslations meson pkg-config create-dmg

deploy_dir=$(readlink -f $(dirname "$BASH_SOURCE"))
version=$(bash "${deploy_dir}"/../version.sh get-vcs)
arch=`uname -m`
app_name="ZeGrapher-macOS-$arch-$version"
dmg_name="ZeGrapher-macOS-$arch-$version"

echo "Building ZeGrapher $version for macOS ($arch)..."

# Build the application
meson setup \
  -D optimization=3 \
  -D debug=false \
  -D b_ndebug=true \
  -D loglevel=off \
  -D b_lto=true \
  --prefix="${deploy_dir}/$app_name" \
  --bindir="Contents/MacOS" \
  "${deploy_dir}/build-macos" \
  "${deploy_dir}/.."

cd "${deploy_dir}/build-macos"
meson compile
meson install
cd ..

echo "Running macdeployqt to bundle Qt frameworks..."
macdeployqt $app_name -verbose=2 -executable="$app_name"/Contents/MacOS/ZeGrapher -qmldir="$deploy_dir/../src/"
# the "-executable=" bit rewrites library search paths with 'install_name_tool'

echo "Creating enhanced DMG installer..."

# Prepare DMG staging directory
dmg_staging="${deploy_dir}/dmg-staging"
rm -rf "$dmg_staging"
mkdir -p "$dmg_staging"

# Copy app bundle to staging
cp -R "$app_name" "$dmg_staging/ZeGrapher.app"

# Code sign the app bundle with ad-hoc signature (deep signing)
# This is required for macOS to run the app, even without a developer certificate
echo "Code signing app bundle..."
codesign --force --deep --sign - "$dmg_staging/ZeGrapher.app"

# Copy README if it exists
if [ -f "${deploy_dir}/dmg-resources/README.txt" ]; then
    cp "${deploy_dir}/dmg-resources/README.txt" "$dmg_staging/"
fi

# Build create-dmg command with options
dmg_opts=(
    --volname "ZeGrapher $version"
    --volicon "${deploy_dir}/../appdata/ZeGrapher.icns"
    --window-pos 200 120
    --window-size 660 450
    --icon-size 128
    --icon "ZeGrapher.app" 150 170
    --hide-extension "ZeGrapher.app"
    --app-drop-link 480 170
    --eula "${deploy_dir}/../LICENSE"
    --hdiutil-quiet
)

# Add background image if it exists
if [ -f "${deploy_dir}/dmg-resources/dmg-background.png" ]; then
    dmg_opts+=(--background "${deploy_dir}/dmg-resources/dmg-background.png")
fi

# Add README icon if it exists
if [ -f "${deploy_dir}/dmg-resources/README.txt" ]; then
    dmg_opts+=(--icon "README.txt" 150 330)
fi

# Create the DMG using create-dmg (https://github.com/create-dmg/create-dmg)
create-dmg "${dmg_opts[@]}" \
    "$dmg_name.dmg" \
    "$dmg_staging"

# Clean up staging
rm -rf "$dmg_staging"

echo "DMG created: $dmg_name.dmg"
echo "Installation package complete!"
