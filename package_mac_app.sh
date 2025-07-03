#!/bin/bash
set -e

APP_NAME="RedNet"
EXECUTABLE="build/bin/RedNet"
ICON="resources/RedNet.icns"
PLIST="resources/Info.plist"
APP_DIR="${APP_NAME}.app"

# Remove any existing app bundle
rm -rf "$APP_DIR"

# Create directory structure
mkdir -p "$APP_DIR/Contents/MacOS"
mkdir -p "$APP_DIR/Contents/Resources"

# Copy executable
cp "$EXECUTABLE" "$APP_DIR/Contents/MacOS/$APP_NAME"
chmod +x "$APP_DIR/Contents/MacOS/$APP_NAME"

# Copy icon
cp "$ICON" "$APP_DIR/Contents/Resources/RedNet.icns"

# Copy Info.plist
cp "$PLIST" "$APP_DIR/Contents/Info.plist"

echo "${APP_NAME}.app bundle created successfully!" 