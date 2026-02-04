#!/bin/bash

# --- 1. Configuration ---
APP_NAME="CW_Trainer-GNR"
ROOT_DIR=$(pwd)
BUILD_DIR="$ROOT_DIR/build-windows"
ASSETS_DIR="$ROOT_DIR/Build_Assets"
OUTPUT_DIR="$ROOT_DIR/Download/Windows"
SRC_DIR="$ROOT_DIR/Source_code"
QMAKE_BIN="x86_64-w64-mingw32-qmake-qt6"

# Ensure output directory exists and is CLEAN of subfolders
mkdir -p "$OUTPUT_DIR"
rm -rf "$OUTPUT_DIR/Downloader" "$OUTPUT_DIR/Portable" # Delete the messy ones

echo "======================================================="
echo "🔍 PRE-BUILD CHECK: Current Release Files"
echo "======================================================="
ls -lh "$OUTPUT_DIR" | grep -E "v.*(Setup.exe|Portable.zip)" || echo "No previous builds found."
echo "-------------------------------------------------------"

# --- 2. Interactive Input ---
read -p "Enter Version Number (e.g., 1.0.0): " VERSION_NUM
echo "Select Version Flag: "
echo "		1) 	STBL"
echo "		2) 	DEV"
echo "		3) 	BETA"
read -p "Choice [1-3]: " FLAG_CHOICE

case $FLAG_CHOICE in
    1) VERSION_FLAG="STBL" ;;
    2) VERSION_FLAG="DEV" ;;
    3) VERSION_FLAG="BETA" ;;
    *) VERSION_FLAG="BETA" ;;
esac

FULL_VERSION="${VERSION_NUM}-${VERSION_FLAG}"
INSTALLER_NAME="${APP_NAME}_v${FULL_VERSION}_Setup.exe"
PORTABLE_NAME="${APP_NAME}_v${FULL_VERSION}_Portable.zip"

echo "-------------------------------------------------------"
echo "🚀 PROPOSED BUILD DETAILS:"
echo "   Version:   $FULL_VERSION"
echo "   Installer: $INSTALLER_NAME"
echo "   Portable:  $PORTABLE_NAME"
echo "-------------------------------------------------------"

read -p "Confirm build from SOURCE and packaging? (y/n): " CONFIRM
if [[ $CONFIRM != "y" ]]; then 
    echo "❌ Build aborted."
    exit 0 
fi

START_TIME=$SECONDS

# --- 3. SOURCE COMPILATION ---
echo ""
echo "🧹 [STEP 1/5] Cleaning environment..."
cd "$BUILD_DIR" || exit
rm -rf release/ 
mkdir -p release

echo ""
echo "🔨 [STEP 2/5] Running QMake from Source..."
$QMAKE_BIN "$SRC_DIR/CW_Trainer-GNR.pro" -spec win32-g++

echo ""
echo "🛠️ [STEP 3/5] Compiling C++ code..."
make clean
make -j$(nproc)

if [ ! -f "release/CW_Trainer-GNR.exe" ]; then
    echo "❌ ERROR: Compilation failed!"
    exit 1
fi

echo "✅ Fresh EXE created. Syncing to Build_Assets..."
cp -v "release/CW_Trainer-GNR.exe" "$ASSETS_DIR/CW_Trainer-GNR.exe"

# --- 4. Packaging ---
echo ""
echo "📦 [STEP 4/5] Creating Portable ZIP..."
cd "$ASSETS_DIR" || exit

# Zip the contents of Build_Assets directly into the Windows folder
# We exclude the .nsi script and any old .exe remnants
#zip -r "$OUTPUT_DIR/$PORTABLE_NAME" . -x "installer.nsi" "*.exe_setup"
# Zip everything, but exclude the installer script and any old Setup artifacts
zip -r "$OUTPUT_DIR/$PORTABLE_NAME" . -x "installer.nsi" "*_Setup.exe"
echo ""
echo "🛠️ [STEP 5/5] Building NSIS Installer..."
# makensis uses the updated path in your .nsi file
makensis -V4 -DVERSION_NUM="$VERSION_NUM" -DVERSION_FLAG="$VERSION_FLAG" installer.nsi

DURATION=$(( SECONDS - START_TIME ))

echo ""
echo "======================================================="
echo "🎉 SUCCESS: Build $FULL_VERSION is complete!"
echo "Total Time: $((DURATION / 60))m $((DURATION % 60))s"
echo "Final files in: $OUTPUT_DIR"
echo "======================================================="
ls -lh "$OUTPUT_DIR" | grep "$FULL_VERSION"

# --- 5. AUDIO ALERT ---
for i in {1..3}; do echo -ne "\a"; sleep 0.2; done
