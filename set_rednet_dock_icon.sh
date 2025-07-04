#!/bin/bash

set -e

# Paths
ICNS="resources/icon.icns"
APP_BUNDLE="build/bin/RedNet.app"
RESOURCES="$APP_BUNDLE/Contents/Resources"
PLIST="$APP_BUNDLE/Contents/Info.plist"

echo "Setting RedNet.app Dock icon..."

# 1. Copy .icns to app bundle
echo "Copying icon.icns to app bundle..."
cp $ICNS $RESOURCES/icon.icns

# 2. Update Info.plist
echo "Updating Info.plist..."
/usr/libexec/PlistBuddy -c "Set :CFBundleIconFile icon.icns" $PLIST 2>/dev/null || \
/usr/libexec/PlistBuddy -c "Add :CFBundleIconFile string icon.icns" $PLIST

echo "RedNet.app Dock icon updated successfully!"
echo "You may need to restart the Dock or log out/in to see the change:"
echo "  killall Dock"
