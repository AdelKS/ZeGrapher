#!/bin/bash

set -e

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
macdeployqt $app_name -dmg -verbose=2 -executable="$app_name"/Contents/MacOS/ZeGrapher -qmldir="$deploy_dir/../src/"
# the "-executable=" bit rewrites library search paths with 'install_name_tool'

# Remove the default DMG created by macdeployqt
rm -f ZeGrapher*.dmg

echo "Creating enhanced DMG installer..."

# Check if create-dmg is available (https://github.com/create-dmg/create-dmg)
# Install with: brew install create-dmg
if command -v create-dmg &> /dev/null; then
    echo "Using create-dmg for professional installer..."
    
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
    
    # Create the DMG
    create-dmg "${dmg_opts[@]}" \
        "$dmg_name.dmg" \
        "$dmg_staging"
    
    # Clean up staging
    rm -rf "$dmg_staging"
    
else
    echo "create-dmg not found, using custom DMG creation..."
    echo "For a better installer experience, install create-dmg: brew install create-dmg"
    
    # Fallback: Create a custom DMG using hdiutil
    dmg_temp="${deploy_dir}/dmg-temp"
    dmg_staging="${deploy_dir}/dmg-staging"
    
    rm -rf "$dmg_temp" "$dmg_staging"
    mkdir -p "$dmg_staging"
    
    # Copy app to staging with proper name
    cp -R "$app_name" "$dmg_staging/ZeGrapher.app"
    
    # Code sign the app bundle with ad-hoc signature (deep signing)
    echo "Code signing app bundle..."
    codesign --force --deep --sign - "$dmg_staging/ZeGrapher.app"
    
    # Create Applications symlink
    ln -s /Applications "$dmg_staging/Applications"
    
    # Copy README if exists
    if [ -f "${deploy_dir}/dmg-resources/README.txt" ]; then
        cp "${deploy_dir}/dmg-resources/README.txt" "$dmg_staging/"
    fi
    
    # Create a temporary DMG
    echo "Creating temporary DMG..."
    hdiutil create -volname "ZeGrapher $version" \
        -srcfolder "$dmg_staging" \
        -ov -format UDRW \
        "$dmg_temp.dmg"
    
    # Mount it
    echo "Mounting DMG for customization..."
    device=$(hdiutil attach -readwrite -noverify -noautoopen "$dmg_temp.dmg" | \
             grep '^/dev/' | sed 's|^/dev/||' | awk '{print $1}')
    
    if [ -z "$device" ]; then
        echo "Error: Failed to mount DMG"
        exit 1
    fi
    
    # Wait for mount
    sleep 2
    
    volume="/Volumes/ZeGrapher $version"
    
    # Set custom icon for volume if available
    if [ -f "${deploy_dir}/../appdata/ZeGrapher.icns" ]; then
        cp "${deploy_dir}/../appdata/ZeGrapher.icns" "$volume/.VolumeIcon.icns"
        SetFile -c icnC "$volume/.VolumeIcon.icns"
    fi
    
    # Create .background folder and copy background image if it exists
    mkdir -p "$volume/.background"
    if [ -f "${deploy_dir}/dmg-resources/dmg-background.png" ]; then
        cp "${deploy_dir}/dmg-resources/dmg-background.png" "$volume/.background/background.png"
    fi
    
    # Set window properties using AppleScript
    echo "Configuring DMG window appearance..."
    osascript <<EOF
tell application "Finder"
    tell disk "ZeGrapher $version"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {200, 120, 860, 570}
        set viewOptions to the icon view options of container window
        set arrangement of viewOptions to not arranged
        set icon size of viewOptions to 128
        set background picture of viewOptions to file ".background:background.png"
        
        -- Position icons
        set position of item "ZeGrapher.app" of container window to {150, 170}
        set position of item "Applications" of container window to {480, 170}
        
        -- Position README if it exists
        try
            set position of item "README.txt" of container window to {320, 340}
        end try
        
        close
        open
        update without registering applications
        delay 2
    end tell
end tell
EOF
    
    # Ensure changes are written
    sync
    
    # Unmount
    echo "Finalizing DMG..."
    hdiutil detach "/dev/${device}"
    
    # Convert to compressed read-only DMG
    hdiutil convert "$dmg_temp.dmg" -format UDZO -o "$dmg_name.dmg"
    
    # Clean up
    rm -rf "$dmg_temp.dmg" "$dmg_staging"
fi

echo "DMG created: $dmg_name.dmg"
echo "Installation package complete!"
