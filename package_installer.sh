#!/bin/bash
# ============================================================================
# package_installer.sh - build the installer of the current platform
#
#   Windows: U3LogicAnalyzer_Setup.exe                    (NSIS)
#   macOS:   U3LogicAnalyzer-<ver>-macos-<arch>.dmg       (app bundle, drag to
#                                                          /Applications)
#   Linux:   U3LogicAnalyzer-<ver>-linux-<arch>.tar.gz    (archive + install.sh)
#            U3LogicAnalyzer-<ver>-linux-<arch>.deb       (needs dpkg-deb)
#            U3LogicAnalyzer-<ver>-<arch>.AppImage        (needs appimagetool,
#                                                          auto-downloaded)
#
#   The AppImage is accompanied by U3LogicAnalyzer-<ver>-<arch>.AppImage.run.sh.
#   Ubuntu 24.04 and later do not ship libfuse2, so running the AppImage
#   directly fails with "dlopen(): error loading libfuse.so.2"; use the
#   .run.sh wrapper, --appimage-extract-and-run, or install libfuse2.
#   Optionally drop uruntime-<arch> into dist/ to build an AppImage that
#   runs without FUSE at all.
#
# Usage:
#   ./package_installer.sh               # Build + package + create installer
#   ./package_installer.sh --qt5         # Same, built against Qt 5
#   ./package_installer.sh --skip-build  # Skip the build, package existing dist/
# ============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build_cmake"
DIST_DIR="${SCRIPT_DIR}/dist/U3LogicAnalyzer"
SETUP_NAME="U3LogicAnalyzer_Setup"
SKIP_BUILD=0
CLEAN=1

# Qt version: 6 (default) or 5. It decides which build.sh runs and, on
# Linux, which Qt libraries the deb has to depend on.
QT_MAJOR=6
for arg in "$@"; do
    case "$arg" in
        --qt5) QT_MAJOR=5 ;;
        --qt6) QT_MAJOR=6 ;;
    esac
done

# Version comes from CMake so the packages cannot drift from the binary.
CMAKE_LISTS="${SCRIPT_DIR}/pulseview/CMakeLists.txt"
VERSION=$(sed -n 's/^[[:space:]]*set(PV_VERSION_STRING[[:space:]]*"\([^"]*\)".*/\1/p' \
    "${CMAKE_LISTS}" 2>/dev/null | head -1)
if [ -z "${VERSION}" ]; then
    echo "[warning] Could not read PV_VERSION_STRING from ${CMAKE_LISTS}, falling back to 0.1.0"
    VERSION=0.1.0
fi
echo ">>> Version: ${VERSION} (from pulseview/CMakeLists.txt)"

for arg in "$@"; do
    case $arg in
        --skip-build) SKIP_BUILD=1 ;;
        --clean) CLEAN=1 ;;
        --no-clean) CLEAN=0 ;;
        --qt5|--qt6) ;;   # already picked up above
        *) echo "[error] unknown option: $arg" >&2; exit 1 ;;
    esac
done

if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ -n "$MSYSTEM" ]]; then
    IS_WINDOWS=1
else
    IS_WINDOWS=0
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
    IS_MACOS=1
else
    IS_MACOS=0
fi

# --- Build + package ---
if [ $SKIP_BUILD -eq 0 ]; then
    echo ">>> Step 1/3: build and package... (Qt ${QT_MAJOR})"
    BUILD_ARGS=(--package "--qt${QT_MAJOR}")
    [ $CLEAN -eq 1 ] && BUILD_ARGS+=(--clean)
    bash "${SCRIPT_DIR}/build.sh" "${BUILD_ARGS[@]}"
else
    # The existing dist/ was built against some Qt: read it back from the
    # cache so the packages below do not claim the wrong dependencies.
    CACHE="${BUILD_DIR}/logicanalyzer_build/CMakeCache.txt"
    if [ -f "${CACHE}" ]; then
        CACHED_QT=$(sed -n 's/^QT_VERSION_MAJOR:[^=]*=\([0-9]\).*/\1/p' "${CACHE}" | head -1)
        if [ -n "${CACHED_QT}" ] && [ "${CACHED_QT}" != "${QT_MAJOR}" ]; then
            echo "[warning] dist/ was built against Qt ${CACHED_QT}, but Qt ${QT_MAJOR} was asked for"
            echo "          (drop --skip-build or pass --qt${CACHED_QT} to package what is there)"
            QT_MAJOR="${CACHED_QT}"
        fi
    fi
    echo ">>> Qt: ${QT_MAJOR} (from the existing build)"

    if [ $IS_WINDOWS -eq 1 ]; then
        [ -f "${DIST_DIR}/U3LogicAnalyzer.exe" ] || {
            echo "[error] ${DIST_DIR}/U3LogicAnalyzer.exe not found, run ./build.sh --package first"
            exit 1
        }
    elif [ $IS_MACOS -eq 1 ]; then
        [ -d "${DIST_DIR}/U3LogicAnalyzer.app" ] || {
            echo "[error] ${DIST_DIR}/U3LogicAnalyzer.app not found, run ./build.sh --package first"
            exit 1
        }
    else
        [ -f "${DIST_DIR}/U3LogicAnalyzer" ] || {
            echo "[error] ${DIST_DIR}/U3LogicAnalyzer not found, run ./build.sh --package first"
            exit 1
        }
    fi
    echo ">>> Skipping the build, using the existing dist/U3LogicAnalyzer/"
fi

# ============================================================================
# Windows: NSIS installer
# ============================================================================
if [ $IS_WINDOWS -eq 1 ]; then

# --- Check for NSIS ---
if ! command -v makensis &>/dev/null; then
    echo ">>> Installing NSIS..."
    pacman -S --needed --noconfirm mingw-w64-x86_64-nsis
fi

# --- Generate the NSIS script ---
echo ">>> Step 2/3: generating the installer script..."

NSIS_SCRIPT="${SCRIPT_DIR}/dist/setup.nsi"


cat > "${NSIS_SCRIPT}" << 'NSIEOF'
; U3LogicAnalyzer Windows Setup
; Written by package_installer.sh, so a build from source produces the same
; layout the released installer does: same directory, same shortcuts, same
; uninstall entry, and both user guides on disk.

!define PRODUCT_NAME "U3LogicAnalyzer"
!define PRODUCT_PUBLISHER "WeAct Studio"
!define PRODUCT_EXE "U3LogicAnalyzer.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_PUBLISHER} ${PRODUCT_NAME}"

VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName" "${PRODUCT_PUBLISHER} ${PRODUCT_NAME}"
VIAddVersionKey "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2026 ${PRODUCT_PUBLISHER}. GNU General Public License v3.0"
VIAddVersionKey "FileDescription" "Logic analyzer software base on sigrok pulseview"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"

BrandingText "${PRODUCT_PUBLISHER} ${PRODUCT_NAME} V${PRODUCT_VERSION}"

SetCompressor /SOLID lzma
SetCompressorDictSize 32
SetDatablockOptimize on
RequestExecutionLevel admin

!include "MUI2.nsh"
!include "LogicLib.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "${ICON_FILE}"
!define MUI_UNICON "${ICON_FILE}"

Name "${PRODUCT_PUBLISHER} ${PRODUCT_NAME} V${PRODUCT_VERSION}"
OutFile "${OUT_FILE}"
InstallDir "$PROGRAMFILES64\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "InstallLocation"

!insertmacro MUI_PAGE_WELCOME
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "${DIST_DIR}\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

; Either guide opens from here, the one in the language the installer runs in.
; The empty readme define is what makes MUI use the function instead.
!define MUI_FINISHPAGE_SHOWREADME
!define MUI_FINISHPAGE_SHOWREADME_TEXT "$(ReadmeGuideText)"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION OpenReadme
!define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCT_EXE}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "SimpChinese"

LangString ReadmeGuideText ${LANG_ENGLISH} "Open the user guide"
LangString ReadmeGuideText ${LANG_SIMPCHINESE} "打开使用说明"

Section "Install" SecMain
  SectionIn RO
  SetOutPath "$INSTDIR"

  ; The installer is written into this same directory, so a second run would
  ; pack the previous one into the new one. Exclude it.
  File /r /x "${SETUP_FILE}.exe" "${DIST_DIR}\*"

  CreateDirectory "$SMPROGRAMS\${PRODUCT_PUBLISHER} ${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER} ${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_EXE}" "" "$INSTDIR\${PRODUCT_EXE}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER} ${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_EXE}" "" "$INSTDIR\${PRODUCT_EXE}"

  WriteUninstaller "$INSTDIR\uninstall.exe"

  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_PUBLISHER} ${PRODUCT_NAME}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_EXE}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "https://github.com/WeActStudio"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoModify" 1
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoRepair" 1
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\uninstall.exe"
  RMDir /r "$INSTDIR"

  Delete "$SMPROGRAMS\${PRODUCT_PUBLISHER} ${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_PUBLISHER} ${PRODUCT_NAME}\Uninstall.lnk"
  RMDir "$SMPROGRAMS\${PRODUCT_PUBLISHER} ${PRODUCT_NAME}"

  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"

  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
SectionEnd

Function OpenReadme
  ${If} $LANGUAGE == ${LANG_SIMPCHINESE}
    StrCpy $R0 "$INSTDIR\doc\U3LogicAnalyzer-UsageGuide.zh_CN.pdf"
  ${Else}
    StrCpy $R0 "$INSTDIR\doc\U3LogicAnalyzer-UsageGuide.pdf"
  ${EndIf}
  ExecShell "open" "$R0"
FunctionEnd
NSIEOF


# --- Build the installer ---
echo ">>> Step 3/3: building the installer..."
cd "${DIST_DIR}"

# makensis is a Windows program: every path handed to it has to be a Windows
# one. No brace escaping here - the value is a command line argument, not an
# NSIS string literal.
WIN_DIST_DIR=$(cygpath -w "${DIST_DIR}")
WIN_OUT_FILE=$(cygpath -w "${DIST_DIR}/${SETUP_NAME}.exe")
WIN_ICON_FILE=$(cygpath -w "${SCRIPT_DIR}/pulseview/icons/pulseview.ico")

makensis \
    -DPRODUCT_VERSION="${VERSION}.0" \
    -DOUT_FILE="${WIN_OUT_FILE}" \
    -DDIST_DIR="${WIN_DIST_DIR}" \
    -DICON_FILE="${WIN_ICON_FILE}" \
    -DSETUP_FILE="${SETUP_NAME}" \
    "${NSIS_SCRIPT}"

# The generated script is a build by-product; dist/ is shipped as it stands.
rm -f "${NSIS_SCRIPT}"

echo ""
echo "============================================"
echo " Installer done!"
echo " ${DIST_DIR}/${SETUP_NAME}.exe"
echo " Qt: ${QT_MAJOR}"
echo "============================================"

# ============================================================================
# macOS: .dmg disk image with the app bundle
# ============================================================================
elif [ $IS_MACOS -eq 1 ]; then

APP="${DIST_DIR}/U3LogicAnalyzer.app"
ARCH="$(uname -m)"

# The revision goes in the file name as well as the volume name. Builds of one
# release are kept side by side, and a name without it makes them overwrite
# each other. There is no .git when the sources arrived as an archive, hence
# the file build.sh refreshes on every build.
PV_REV="$(tr -d '[:space:]' < "${SCRIPT_DIR}/pulseview/.git-revision" 2>/dev/null || true)"
if [ -z "$PV_REV" ] && command -v git >/dev/null 2>&1; then
    PV_REV="$(git -C "${SCRIPT_DIR}" rev-parse --short=7 HEAD 2>/dev/null || true)"
fi
case "$PV_REV" in
    ''|*[!0-9a-f]*) PV_REV="" ;;
esac

VERSION_TAG="$VERSION"
if [ -n "$PV_REV" ] && [ "${VERSION#*-git-}" = "$VERSION" ]; then
    VERSION_TAG="${VERSION}-git-${PV_REV}"
fi

BASENAME="U3LogicAnalyzer-${VERSION_TAG}-macos-${ARCH}"
DMG="${SCRIPT_DIR}/dist/${BASENAME}.dmg"
STAGE="${SCRIPT_DIR}/dist/${BASENAME}-stage"

echo ">>> Step 2/3: building ${BASENAME}.dmg..."

# The image must not seal a stale signature: a run of the app from dist/
# writes __pycache__ into the framework and invalidates it.
codesign --verify --deep --strict "${APP}" >/dev/null 2>&1 || {
    echo "[warning] the app bundle does not verify; rebuilding it is safer"
    echo "          (run ./build.sh --package, or ./package_installer.sh without --skip-build)"
}

# ditto, not cp -R: it keeps the symlinks, permissions and extended
# attributes that the code signature seals.
SETTINGS="${SCRIPT_DIR}/packaging/macos/dmg-settings.py"
BACKGROUND="${SCRIPT_DIR}/packaging/macos/dmg-background.png"

# dmgbuild writes the .DS_Store itself instead of asking Finder to, which a
# non-interactive session cannot authorise. It is optional: without it the
# image still works, it just has no background and no icon layout.
# It is often installed in the same prefix as the sysroot, which is not on
# PATH, so look there as well.
DMGBUILD=""
for cand in "${DMGBUILD_BIN:-}" "$(command -v dmgbuild 2>/dev/null)" \
            "${PV_SYSROOT:+${PV_SYSROOT}/bin/dmgbuild}"; do
    [ -n "$cand" ] && [ -x "$cand" ] && DMGBUILD="$cand" && break
done

# Finder keys its remembered window size on the volume name
# (com.apple.finder.plist, FXRecentFolders) and that beats the WindowBounds
# written into .DS_Store, so an older image of the same name leaves the window
# at whatever size it was last dragged to. The version and the revision in the
# name give every build a fresh record.
VOL_NAME="U3LogicAnalyzer ${VERSION_TAG}"

rm -f "${DMG}"
if [ -n "$DMGBUILD" ] && [ -f "$SETTINGS" ] && [ -f "$BACKGROUND" ]; then
    echo "  dmgbuild: background image and icon layout"
    "$DMGBUILD" -s "$SETTINGS" \
        -D "app_path=${APP}" \
        -D "bg_path=${BACKGROUND}" \
        "$VOL_NAME" "${DMG}"
else
    echo "  (dmgbuild not found: image without background or layout;"
    echo "   install it with pip install 'dmgbuild==1.6.1')"
    rm -rf "${STAGE}"
    mkdir -p "${STAGE}"
    ditto "${APP}" "${STAGE}/U3LogicAnalyzer.app"
    ln -s /Applications "${STAGE}/Applications"
    hdiutil create -volname "$VOL_NAME" -srcfolder "${STAGE}" \
        -ov -format UDZO -quiet -imagekey zlib-level=9 "${DMG}"
    rm -rf "${STAGE}"
fi

# Get Info on the image file shows this. It is an extended attribute rather
# than anything inside the image, so a browser download can drop it; the same
# text is painted into the background for that reason.
/usr/bin/python3 - "${DMG}" "${VOL_NAME}" <<'PY' 2>/dev/null || true
import plistlib, subprocess, sys

dmg, volume = sys.argv[1], sys.argv[2]
comment = (
    f"WeAct Studio {volume}\n"
    "开源逻辑分析仪，基于 sigrok 与 PulseView 构建。\n"
    "https://github.com/WeActStudio/WeActStudio.U3LogicAnalyzer"
)
data = plistlib.dumps(comment, fmt=plistlib.FMT_BINARY)
subprocess.run(
    ["xattr", "-wx", "com.apple.metadata:kMDItemFinderComment", data.hex(), dmg],
    check=True)
PY

echo ""
echo "============================================"
echo " Installer done! (Qt ${QT_MAJOR})"
echo " ${DMG}"
echo " $(du -h "${DMG}" | cut -f1) on disk"
echo ""
echo " Open the image and drag U3LogicAnalyzer into Applications."
echo " The bundle is ad-hoc signed and not notarised, so on a machine other"
echo " than the build one the first launch needs right click -> Open."
echo "============================================"

# ============================================================================
# Linux: tar.gz archive (+ .deb when dpkg-deb is available)
# ============================================================================
else

ARCH=$(dpkg --print-architecture 2>/dev/null || echo "")
[ -z "${ARCH}" ] && ARCH=$(uname -m)
case "${ARCH}" in
    x86_64|amd64) ARCH=amd64 ;;
    aarch64|arm64) ARCH=arm64 ;;
esac

# Also the app id: the desktop file name, the icon name and (through
# QGuiApplication::setDesktopFileName) the Wayland app id all use it.
APP_NAME="U3LogicAnalyzer"
# Name of the Debian package: lower case, as Debian policy wants it.
DEB_PKG="u3logicanalyzer"

# A Qt 6 build gets a -qt6 tag: the same version of the Qt 5 and Qt 6
# packages would otherwise overwrite each other in dist/.
if [ "${QT_MAJOR}" = "6" ]; then QT_TAG="-qt6"; else QT_TAG=""; fi

BASENAME="U3LogicAnalyzer-${VERSION}-linux-${ARCH}${QT_TAG}"
STAGE="${SCRIPT_DIR}/dist/${BASENAME}"
TARBALL="${STAGE}.tar.gz"
DEB="${STAGE}.deb"

# The binary links QtWidgets/QtSvg/QtDBus; the runtime packages carry the
# same major version number as the Qt they belong to (libqt5widgets5,
# libqt6widgets6, ...).
QT_DEPS="libqt${QT_MAJOR}widgets${QT_MAJOR}, libqt${QT_MAJOR}svg${QT_MAJOR}, libqt${QT_MAJOR}dbus${QT_MAJOR}"

# --- Icon theme tree -------------------------------------------------------
# The shipped PNG is 200x200, which matches no hicolor directory: app
# centres (AppStream) reject an icon whose real size differs from the
# directory it sits in, so build a proper hicolor tree instead. Python +
# Pillow are used when present; otherwise the original file is copied,
# which is only a cosmetic deviation.
scale_icon() {   # $1 source, $2 target, $3 size
    python3 - "$1" "$2" "$3" <<'PYEOF' 2>/dev/null || cp -f "$1" "$2"
import sys
from PIL import Image
src, dst, size = sys.argv[1], sys.argv[2], int(sys.argv[3])
Image.open(src).convert("RGBA").resize((size, size), Image.LANCZOS).save(dst)
PYEOF
}

ICON_TREE="${DIST_DIR}/share/icons/hicolor"
if [ -f "${DIST_DIR}/share/icons/${APP_NAME}.png" ]; then
    mkdir -p "${ICON_TREE}/256x256/apps" "${ICON_TREE}/48x48/apps" "${ICON_TREE}/scalable/apps"
    scale_icon "${DIST_DIR}/share/icons/${APP_NAME}.png" \
               "${ICON_TREE}/256x256/apps/${APP_NAME}.png" 256
    scale_icon "${DIST_DIR}/share/icons/${APP_NAME}.png" \
               "${ICON_TREE}/48x48/apps/${APP_NAME}.png" 48
    [ -f "${DIST_DIR}/share/icons/${APP_NAME}.svg" ] && \
        cp "${DIST_DIR}/share/icons/${APP_NAME}.svg" "${ICON_TREE}/scalable/apps/"
fi

# --- AppStream metadata ----------------------------------------------------
# Without it an app centre (GNOME Software, Ubuntu App Center) has nothing
# to read the name, the icon and the description from and shows a
# placeholder tile, even though the menu entry itself is fine.
METAINFO="${SCRIPT_DIR}/dist/${APP_NAME}.metainfo.xml"
cat > "${METAINFO}" << METEOF
<?xml version="1.0" encoding="UTF-8"?>
<component type="desktop-application">
  <id>${APP_NAME}</id>
  <name>${APP_NAME}</name>
  <summary>CH32H417 Logic Analyzer</summary>
  <developer_name>WeAct Studio</developer_name>
  <metadata_license>CC0-1.0</metadata_license>
  <project_license>GPL-3.0-or-later</project_license>
  <description>
    <p>PulseView based acquisition software for the WeAct Studio CH32H417
    logic analyzer: captures with up to 16 channels and decodes them with
    the bundled sigrok protocol decoders.</p>
  </description>
  <launchable type="desktop-id">${APP_NAME}.desktop</launchable>
  <icon type="stock">${APP_NAME}</icon>
  <categories>
    <category>Development</category>
    <category>Science</category>
  </categories>
  <provides>
    <binary>${APP_NAME}</binary>
  </provides>
  <url type="homepage">https://github.com/WeActStudio</url>
</component>
METEOF

# --- install.sh / uninstall.sh shipped inside the archive ---
cat > "${SCRIPT_DIR}/dist/install.sh" << 'INSEOF'
#!/bin/bash
# Install U3LogicAnalyzer: program files, menu entry, icons and udev rules.
#   sudo ./install.sh           # system wide: /opt/U3LogicAnalyzer
#   ./install.sh --user         # current user only
set -e
DIR="$(cd "$(dirname "$0")" && pwd)"

if [ "$(id -u)" -eq 0 ] && [ "$1" != "--user" ]; then
    PREFIX=/opt/U3LogicAnalyzer
    APP_DIR=/usr/share/applications
    ICON_ROOT=/usr/share/icons/hicolor
    META_DIR=/usr/share/metainfo
    UDEV_DIR=/etc/udev/rules.d
else
    PREFIX="$HOME/.local/opt/U3LogicAnalyzer"
    APP_DIR="$HOME/.local/share/applications"
    ICON_ROOT="$HOME/.local/share/icons/hicolor"
    META_DIR="$HOME/.local/share/metainfo"
    UDEV_DIR=""
fi

echo ">>> Installing to ${PREFIX}"
mkdir -p "$PREFIX" "$APP_DIR" "$ICON_ROOT/48x48/apps" "$ICON_ROOT/scalable/apps"
cp -r "$DIR"/. "$PREFIX"/
rm -f "$PREFIX/install.sh" "$PREFIX/uninstall.sh"
chmod +x "$PREFIX/U3LogicAnalyzer"
if [ -f "$PREFIX/u3logicanalyzer-cli" ]; then chmod +x "$PREFIX/u3logicanalyzer-cli"; fi

# libsigrok only searches standard XDG data dirs for FX2 firmware, not the
# install prefix, so point the launcher at the bundled copy explicitly.
sed "s|^Exec=.*|Exec=env SIGROK_FIRMWARE_DIR=$PREFIX/share/sigrok-firmware $PREFIX/U3LogicAnalyzer|; s|^Icon=.*|Icon=U3LogicAnalyzer|" \
    "$DIR/U3LogicAnalyzer.desktop" > "$APP_DIR/U3LogicAnalyzer.desktop"

# A hicolor tree from the packager is preferred: there the sizes match the
# directories, which is what app centres check. Fall back to the plain
# icon files when the tree is not part of this archive.
if [ -d "$DIR/share/icons/hicolor" ]; then
    cp -r "$DIR/share/icons/hicolor/." "$ICON_ROOT/" 2>/dev/null || true
else
    cp "$DIR/share/icons/U3LogicAnalyzer.png" "$ICON_ROOT/48x48/apps/" 2>/dev/null || true
    cp "$DIR/share/icons/U3LogicAnalyzer.svg" "$ICON_ROOT/scalable/apps/" 2>/dev/null || true
fi

# AppStream metadata: without it an app centre shows a placeholder tile.
if [ -f "$DIR/U3LogicAnalyzer.metainfo.xml" ]; then
    mkdir -p "$META_DIR"
    cp "$DIR/U3LogicAnalyzer.metainfo.xml" "$META_DIR/"
    echo ">>> Installed the AppStream metadata"
fi

if [ -n "$UDEV_DIR" ]; then
    for rules in 99-weact-ch32h417.rules 99-fx2lafw.rules; do
        [ -f "$DIR/$rules" ] || continue
        cp "$DIR/$rules" "$UDEV_DIR/"
        echo ">>> Installed $rules"
    done
    udevadm control --reload-rules 2>/dev/null || true
    udevadm trigger --subsystem-match=usb 2>/dev/null || true
fi

update-desktop-database "$APP_DIR" 2>/dev/null || true
gtk-update-icon-cache -f -t "$ICON_ROOT" 2>/dev/null || true
echo ">>> Done: menu entry U3LogicAnalyzer"
INSEOF

cat > "${SCRIPT_DIR}/dist/uninstall.sh" << 'UNESEOF'
#!/bin/bash
# Remove what install.sh added.
#   sudo ./uninstall.sh         # system wide
#   ./uninstall.sh --user      # current user only
set -e

if [ "$(id -u)" -eq 0 ] && [ "$1" != "--user" ]; then
    PREFIX=/opt/U3LogicAnalyzer
    APP_DIR=/usr/share/applications
    ICON_ROOT=/usr/share/icons/hicolor
    META_DIR=/usr/share/metainfo
    UDEV_DIR=/etc/udev/rules.d
else
    PREFIX="$HOME/.local/opt/U3LogicAnalyzer"
    APP_DIR="$HOME/.local/share/applications"
    ICON_ROOT="$HOME/.local/share/icons/hicolor"
    META_DIR="$HOME/.local/share/metainfo"
    UDEV_DIR="$HOME/.local/share/udev"
fi

rm -rf "$PREFIX" "$UDEV_DIR"/99-weact-ch32h417.rules "$UDEV_DIR"/99-fx2lafw.rules
rm -f "$APP_DIR/U3LogicAnalyzer.desktop"
rm -f "$ICON_ROOT/48x48/apps/U3LogicAnalyzer.png" "$ICON_ROOT/scalable/apps/U3LogicAnalyzer.svg"
rm -f "$ICON_ROOT/256x256/apps/U3LogicAnalyzer.png" "$META_DIR/U3LogicAnalyzer.metainfo.xml"
update-desktop-database "$APP_DIR" 2>/dev/null || true
gtk-update-icon-cache -f -t "$ICON_ROOT" 2>/dev/null || true
echo ">>> Removed U3LogicAnalyzer"
UNESEOF
chmod +x "${SCRIPT_DIR}/dist/install.sh" "${SCRIPT_DIR}/dist/uninstall.sh"

# --- tar.gz ---
echo ">>> Step 2/4: creating the tar.gz archive..."
rm -rf "${STAGE}"
mkdir -p "${STAGE}"
cp -r "${DIST_DIR}/." "${STAGE}/"
cp "${SCRIPT_DIR}/dist/install.sh" "${SCRIPT_DIR}/dist/uninstall.sh" \
   "${METAINFO}" "${STAGE}/"
for rules in 99-weact-ch32h417.rules 99-fx2lafw.rules; do
    cp "${SCRIPT_DIR}/${rules}" "${STAGE}/" 2>/dev/null || true
done
chmod +x "${STAGE}/install.sh" "${STAGE}/uninstall.sh"
tar czf "${TARBALL}" -C "${SCRIPT_DIR}/dist" "${BASENAME}"
echo "  ${TARBALL}"

# --- deb ---
echo ">>> Step 3/4: building the deb package..."
if command -v dpkg-deb >/dev/null 2>&1; then
    DEB_ROOT="${SCRIPT_DIR}/dist/deb-root"
    rm -rf "${DEB_ROOT}"
    mkdir -p "${DEB_ROOT}/DEBIAN" \
             "${DEB_ROOT}/opt/U3LogicAnalyzer" \
             "${DEB_ROOT}/usr/share/applications" \
             "${DEB_ROOT}/usr/share/icons/hicolor/48x48/apps" \
             "${DEB_ROOT}/usr/share/icons/hicolor/scalable/apps" \
             "${DEB_ROOT}/etc/udev/rules.d"

    cp -r "${DIST_DIR}/." "${DEB_ROOT}/opt/U3LogicAnalyzer/"
    chmod +x "${DEB_ROOT}/opt/U3LogicAnalyzer/U3LogicAnalyzer"
    if [ -f "${DEB_ROOT}/opt/U3LogicAnalyzer/u3logicanalyzer-cli" ]; then
        chmod +x "${DEB_ROOT}/opt/U3LogicAnalyzer/u3logicanalyzer-cli"
    fi

    # Same as install.sh: the deb's firmware lives outside the default search dirs.
    sed "s|^Exec=.*|Exec=env SIGROK_FIRMWARE_DIR=/opt/U3LogicAnalyzer/share/sigrok-firmware /opt/U3LogicAnalyzer/U3LogicAnalyzer|; s|^Icon=.*|Icon=U3LogicAnalyzer|; \
         s|^Categories=.*|Categories=Development;Electronics;|" \
        "${DIST_DIR}/U3LogicAnalyzer.desktop" \
        > "${DEB_ROOT}/usr/share/applications/U3LogicAnalyzer.desktop"
    # Version=1.0 keeps desktop-file-validate happy on strict distributions.
    grep -q '^Version=' "${DEB_ROOT}/usr/share/applications/U3LogicAnalyzer.desktop" || \
        sed -i 's/^\[Desktop Entry\]/[Desktop Entry]\nVersion=1.0/' \
            "${DEB_ROOT}/usr/share/applications/U3LogicAnalyzer.desktop"

    # Icons come from the hicolor tree built above, so the sizes match
    # their directories. The unthemed pixmap is what older or non themed
    # launchers fall back to.
    mkdir -p "${DEB_ROOT}/usr/share/icons/hicolor" "${DEB_ROOT}/usr/share/pixmaps"
    cp -r "${ICON_TREE}/." "${DEB_ROOT}/usr/share/icons/hicolor/" 2>/dev/null || true
    cp "${DIST_DIR}/share/icons/${APP_NAME}.png" \
       "${DEB_ROOT}/usr/share/pixmaps/" 2>/dev/null || true
    for rules in 99-weact-ch32h417.rules 99-fx2lafw.rules; do
        cp "${SCRIPT_DIR}/${rules}" \
           "${DEB_ROOT}/etc/udev/rules.d/" 2>/dev/null || true
    done

    # AppStream metadata is what an app centre reads the name, the icon
    # and the description from; without it the entry stays a placeholder.
    mkdir -p "${DEB_ROOT}/usr/share/metainfo"
    cp "${METAINFO}" "${DEB_ROOT}/usr/share/metainfo/${APP_NAME}.metainfo.xml"

    # Installed-Size is what software centres show as the download size;
    # without it they display "0 B".
    INSTALLED_KB=$(du -sk "${DEB_ROOT}" 2>/dev/null | awk '{print $1}')
    [ -z "${INSTALLED_KB}" ] && INSTALLED_KB=0

    cat > "${DEB_ROOT}/DEBIAN/control" << CTRLEOF
Package: ${DEB_PKG}
Version: ${VERSION}
Section: electronics
Priority: optional
Architecture: ${ARCH}
Installed-Size: ${INSTALLED_KB}
Maintainer: WeAct Studio
Homepage: https://github.com/WeActStudio
License: GPL-3+
Depends: ${QT_DEPS}, python3, libglib2.0-0, libusb-1.0-0, libzip4 | libzip2
Description: CH32H417 Logic Analyzer (WeAct Studio)
 PulseView based acquisition software for the WeAct Studio
 CH32H417 logic analyzer. Installed to /opt/U3LogicAnalyzer.
CTRLEOF

    # Debian policy wants a copyright file, and software centres read the
    # holder from it: the licence text on its own names no one.
    mkdir -p "${DEB_ROOT}/usr/share/doc/${DEB_PKG}"
    {
        echo "Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/"
        echo "Upstream-Name: U3LogicAnalyzer"
        echo "Source: https://github.com/WeActStudio/WeActStudio.U3LogicAnalyzer"
        echo ""
        echo "Files: *"
        echo "Copyright: 2026 WeAct Studio"
        echo " Parts carried over from the Q2H2 project: 2026 Q2H2"
        echo "License: GPL-3+"
        [ -f "${SCRIPT_DIR}/LICENSE" ] &&
            sed -e 's/[[:space:]]*$//' -e 's/^/ /' "${SCRIPT_DIR}/LICENSE"
    } > "${DEB_ROOT}/usr/share/doc/${DEB_PKG}/copyright"

    cat > "${DEB_ROOT}/DEBIAN/postinst" << 'PIEOF'
#!/bin/sh
set -e
chmod +x /opt/U3LogicAnalyzer/U3LogicAnalyzer
if [ -f /opt/U3LogicAnalyzer/u3logicanalyzer-cli ]; then chmod +x /opt/U3LogicAnalyzer/u3logicanalyzer-cli; fi
update-desktop-database /usr/share/applications 2>/dev/null || true
gtk-update-icon-cache -f -t /usr/share/icons/hicolor 2>/dev/null || true
udevadm control --reload-rules 2>/dev/null || true
udevadm trigger --subsystem-match=usb 2>/dev/null || true
exit 0
PIEOF

    cat > "${DEB_ROOT}/DEBIAN/postrm" << 'PREOF'
#!/bin/sh
set -e
rm -f /usr/share/applications/U3LogicAnalyzer.desktop
rm -f /usr/share/icons/hicolor/256x256/apps/U3LogicAnalyzer.png
rm -f /usr/share/icons/hicolor/48x48/apps/U3LogicAnalyzer.png
rm -f /usr/share/icons/hicolor/scalable/apps/U3LogicAnalyzer.svg
rm -f /usr/share/pixmaps/U3LogicAnalyzer.png
rm -rf /opt/U3LogicAnalyzer
update-desktop-database /usr/share/applications 2>/dev/null || true
gtk-update-icon-cache -f -t /usr/share/icons/hicolor 2>/dev/null || true
# The udev rules file is owned by the package and removed by dpkg; reload
# so the rule disappears from the running system too.
udevadm control --reload-rules 2>/dev/null || true
udevadm trigger --subsystem-match=usb 2>/dev/null || true
exit 0
PREOF

    chmod 0755 "${DEB_ROOT}/DEBIAN/postinst" "${DEB_ROOT}/DEBIAN/postrm"
    dpkg-deb --build --root-owner-group "${DEB_ROOT}" "${DEB}" 2>/dev/null \
        || dpkg-deb --build "${DEB_ROOT}" "${DEB}"
    rm -rf "${DEB_ROOT}"
    echo "  ${DEB}"
else
    echo "  (dpkg-deb not available, skipping the deb package; install dpkg-dev and retry)"
fi

# --- AppImage ---
echo ">>> Step 4/4: building the AppImage..."

case "${ARCH}" in
    amd64)  AI_ARCH=x86_64 ;;
    arm64)  AI_ARCH=aarch64 ;;
    *)      AI_ARCH="${ARCH}" ;;
esac

APPIMAGE="${SCRIPT_DIR}/dist/U3LogicAnalyzer-${VERSION}-${AI_ARCH}${QT_TAG}.AppImage"
TOOL=""
for c in appimagetool "appimagetool-${AI_ARCH}.AppImage" "appimagetool-x86_64.AppImage"; do
    if command -v "$c" >/dev/null 2>&1; then TOOL="$c"; break; fi
done

# Download helper: curl -> wget -> python3, whichever is available.
download_file() {
    local url="$1" out="$2"
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL -o "${out}" "${url}"
    elif command -v wget >/dev/null 2>&1; then
        wget -q -O "${out}" "${url}"
    elif command -v python3 >/dev/null 2>&1; then
        python3 -c "import urllib.request, sys; urllib.request.urlretrieve(sys.argv[1], sys.argv[2])" \
            "${url}" "${out}"
    else
        return 1
    fi
}

if [ -z "${TOOL}" ]; then
    TOOL="${SCRIPT_DIR}/dist/appimagetool-${AI_ARCH}.AppImage"
    if [ ! -f "${TOOL}" ]; then
        if ! download_file \
            "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-${AI_ARCH}.AppImage" \
            "${TOOL}" 2>/dev/null; then
            rm -f "${TOOL}"
            TOOL=""
        else
            chmod +x "${TOOL}"
        fi
    fi
fi

if [ -n "${TOOL}" ] && [ -f "${DIST_DIR}/U3LogicAnalyzer" ]; then
    APPDIR="${SCRIPT_DIR}/dist/U3LogicAnalyzer.AppDir"
    rm -rf "${APPDIR}"
    mkdir -p "${APPDIR}/usr/bin" "${APPDIR}/usr/lib" \
             "${APPDIR}/usr/share/applications" \
             "${APPDIR}/usr/share/icons/hicolor/256x256/apps" \
             "${APPDIR}/usr/share/icons/hicolor/48x48/apps" \
             "${APPDIR}/usr/share/pixmaps" \
             "${APPDIR}/usr/share/libsigrokdecode" \
             "${APPDIR}/usr/share/sigrok-firmware"

    cp "${DIST_DIR}/U3LogicAnalyzer" "${APPDIR}/usr/bin/"
    # The command line tool the agent drives; it looks for its libraries in
    # ../lib, which is where the AppImage keeps them.
    if [ -f "${DIST_DIR}/u3logicanalyzer-cli" ]; then
        cp "${DIST_DIR}/u3logicanalyzer-cli" "${APPDIR}/usr/bin/"
        chmod +x "${APPDIR}/usr/bin/u3logicanalyzer-cli"
    fi
    if [ -f "${DIST_DIR}/u3logicanalyzer-mcp-server" ]; then
        cp "${DIST_DIR}/u3logicanalyzer-mcp-server" "${APPDIR}/usr/bin/"
        chmod +x "${APPDIR}/usr/bin/u3logicanalyzer-mcp-server"
    fi
    # Help -> User Guide resolves ../share/doc/u3logicanalyzer from usr/bin.
    for pdf in U3LogicAnalyzer-UsageGuide.pdf U3LogicAnalyzer-UsageGuide.zh_CN.pdf; do
        if [ -f "${DIST_DIR}/doc/${pdf}" ]; then
            mkdir -p "${APPDIR}/usr/share/doc/u3logicanalyzer"
            cp "${DIST_DIR}/doc/${pdf}" "${APPDIR}/usr/share/doc/u3logicanalyzer/"
        fi
    done
    cp -P "${DIST_DIR}"/libsigrok*.so* "${APPDIR}/usr/lib/" 2>/dev/null || true
    cp -r "${DIST_DIR}/share/libsigrokdecode/decoders" \
          "${APPDIR}/usr/share/libsigrokdecode/" 2>/dev/null || true
    # fx2lafw needs these at scan time; without them the board is not listed.
    cp "${DIST_DIR}"/share/sigrok-firmware/*.fw \
       "${APPDIR}/usr/share/sigrok-firmware/" 2>/dev/null || true

    sed "s|^Exec=.*|Exec=U3LogicAnalyzer|; s|^Icon=.*|Icon=U3LogicAnalyzer|" \
        "${DIST_DIR}/U3LogicAnalyzer.desktop" > "${APPDIR}/U3LogicAnalyzer.desktop"
    grep -q '^Version=' "${APPDIR}/U3LogicAnalyzer.desktop" || \
        sed -i 's/^\[Desktop Entry\]/[Desktop Entry]\nVersion=1.0/' \
            "${APPDIR}/U3LogicAnalyzer.desktop"
    cp "${APPDIR}/U3LogicAnalyzer.desktop" "${APPDIR}/usr/share/applications/"

    # Copy (not link) the icon: the root icon is what file managers show.
    # The themed copies come from the hicolor tree, so the task bar gets an
    # icon whose size matches the directory it is looked up in.
    cp "${DIST_DIR}/share/icons/U3LogicAnalyzer.png" "${APPDIR}/U3LogicAnalyzer.png"
    cp "${APPDIR}/U3LogicAnalyzer.png" "${APPDIR}/.DirIcon"
    cp -r "${ICON_TREE}/." "${APPDIR}/usr/share/icons/hicolor/" 2>/dev/null || true
    cp "${DIST_DIR}/share/icons/U3LogicAnalyzer.png" "${APPDIR}/usr/share/pixmaps/"
    [ -f "${DIST_DIR}/share/icons/U3LogicAnalyzer.svg" ] && \
        cp "${DIST_DIR}/share/icons/U3LogicAnalyzer.svg" \
           "${APPDIR}/usr/share/icons/hicolor/scalable/apps/" 2>/dev/null || true

    cat > "${APPDIR}/AppRun" << AREOF
#!/bin/sh
HERE="\$(dirname "\$(readlink -f "\$0")")"
export LD_LIBRARY_PATH="\$HERE/usr/lib:\${LD_LIBRARY_PATH}"
export SIGROKDECODE_DIR="\$HERE/usr/share/libsigrokdecode/decoders"
export SIGROK_FIRMWARE_DIR="\$HERE/usr/share/sigrok-firmware"

# Desktop integration. The shell resolves the icon of a running window
# from its app id (Wayland) or WM_CLASS (X11) by looking up the matching
# .desktop file, and it only watches the data directories it knew about
# when the session started - so exporting XDG_DATA_DIRS alone is not
# enough for an AppImage that appears later. Copy the launcher and the
# icon into the user data directory as well; U3LA_NO_INTEGRATION=1 skips
# it, and every step is best effort (a read-only home just stays as is).
if [ -z "\$U3LA_NO_INTEGRATION" ]; then
    DATA_HOME="\${XDG_DATA_HOME:-\$HOME/.local/share}"
    if mkdir -p "\$DATA_HOME/applications" \\
                "\$DATA_HOME/icons/hicolor/256x256/apps" \\
                "\$DATA_HOME/icons/hicolor/48x48/apps" 2>/dev/null; then
        # Exec must be absolute and must survive this run: the mount point
        # changes every time, so prefer the AppImage itself (the runtime
        # exports its path) and fall back to the extracted binary.
        EXEC="\$HERE/usr/bin/${APP_NAME}"
        [ -n "\$APPIMAGE" ] && [ -f "\$APPIMAGE" ] && EXEC="\$APPIMAGE"
        if [ -f "\$HERE/usr/share/applications/${APP_NAME}.desktop" ]; then
            sed "s|^Exec=.*|Exec=\$EXEC|" \\
                "\$HERE/usr/share/applications/${APP_NAME}.desktop" \\
                > "\$DATA_HOME/applications/${APP_NAME}.desktop.\$\$" 2>/dev/null &&
                mv -f "\$DATA_HOME/applications/${APP_NAME}.desktop.\$\$" \\
                      "\$DATA_HOME/applications/${APP_NAME}.desktop" 2>/dev/null
            rm -f "\$DATA_HOME/applications/${APP_NAME}.desktop.\$\$" 2>/dev/null
        fi
        for size in 256x256 48x48; do
            [ -f "\$HERE/usr/share/icons/hicolor/\$size/apps/${APP_NAME}.png" ] &&
                cp -f "\$HERE/usr/share/icons/hicolor/\$size/apps/${APP_NAME}.png" \\
                      "\$DATA_HOME/icons/hicolor/\$size/apps/" 2>/dev/null
        done
        update-desktop-database "\$DATA_HOME/applications" 2>/dev/null || true
        gtk-update-icon-cache -f -t "\$DATA_HOME/icons/hicolor" 2>/dev/null || true
    fi
fi

# Makes the bundled icon theme and launcher visible to the application
# itself and to anything it starts.
export XDG_DATA_DIRS="\$HERE/usr/share\${XDG_DATA_DIRS:+:\$XDG_DATA_DIRS}"

exec "\$HERE/usr/bin/${APP_NAME}" "\$@"
AREOF
    chmod +x "${APPDIR}/AppRun"

    # A uruntime binary next to dist/ makes the AppImage run without FUSE,
    # so it can simply be double-clicked. Optional: download
    # uruntime-${AI_ARCH} from the AppImageCommunity releases when wanted.
    RUNTIME_ARG=""
    if [ -f "${SCRIPT_DIR}/dist/uruntime-${AI_ARCH}" ]; then
        RUNTIME_ARG="--runtime-file ${SCRIPT_DIR}/dist/uruntime-${AI_ARCH}"
        echo "  (using uruntime: the AppImage will run without FUSE)"
    fi

    # Extract-and-run avoids the FUSE requirement of the AppImage runtime.
    if APPIMAGE_EXTRACT_AND_RUN=1 ARCH="${AI_ARCH}" "${TOOL}" ${RUNTIME_ARG} "${APPDIR}" "${APPIMAGE}"; then
        echo "  ${APPIMAGE}"

        # Ubuntu 22.04+ no longer ships libfuse2, so a plain AppImage run
        # fails with "dlopen(): error loading libfuse.so.2". This wrapper
        # extracts on first use and then starts AppRun, no FUSE needed.
        cat > "${APPIMAGE}.run.sh" << WREOF
#!/bin/sh
# Start the AppImage without FUSE: extract once into a cache directory.
set -e
HERE="\$(cd "\$(dirname "\$(readlink -f "\$0")")" && pwd)"
AI="\${HERE}/U3LogicAnalyzer-${VERSION}-${AI_ARCH}${QT_TAG}.AppImage"
CACHE="\${XDG_CACHE_HOME:-\$HOME/.cache}/U3LogicAnalyzer-appimage"
if [ ! -x "\${CACHE}/squashfs-root/AppRun" ]; then
    mkdir -p "\${CACHE}"
    (cd "\${CACHE}" && "\${AI}" --appimage-extract >/dev/null 2>&1)
fi
exec "\${CACHE}/squashfs-root/AppRun" "\$@"
WREOF
        chmod +x "${APPIMAGE}.run.sh"
        echo "  ${APPIMAGE}.run.sh (runs without FUSE)"
    else
        echo "  (appimagetool failed, skipping the AppImage)"
        rm -f "${APPIMAGE}"
    fi
    rm -rf "${APPDIR}"
else
    echo "  (appimagetool not available, skipping the AppImage;"
    echo "   download appimagetool-${AI_ARCH}.AppImage into dist/ and rerun)"
fi

echo ""
echo "============================================"
echo " Installer done! (Qt ${QT_MAJOR})"
echo " ${TARBALL}"
if [ -f "${DEB}" ]; then
    echo " ${DEB}"
    echo ""
    echo " Install:  sudo apt install ./${DEB##*/}"
    echo " Remove:   sudo apt remove ${DEB_PKG}"
    echo " A software centre only manages apps that come from a software source,"
    echo " so this locally installed deb is not listed there and cannot be"
    echo " removed from it - apt/dpkg is the way (or Synaptic, for a GUI)."
fi
if [ -f "${APPIMAGE}" ]; then
    echo " ${APPIMAGE}"
    echo ""
    echo ""
    echo " How to run the AppImage (Ubuntu 24.04 and later omit libfuse2, so a"
    echo " double click fails with: dlopen(): error loading libfuse.so.2):"
    echo "   1) ${APPIMAGE}.run.sh"
    echo "      Recommended: no root, no FUSE; extracts to ~/.cache on first use."
    echo "   2) ${APPIMAGE} --appimage-extract-and-run"
    echo "      Without root: extracts to /tmp on every start."
    echo "   3) sudo apt install libfuse2"
    echo "      After that ${APPIMAGE##*/} can be started by double clicking."
    [ -n "${RUNTIME_ARG}" ] && echo "      (this package uses uruntime, so FUSE is not needed at all)"
    [ -z "${RUNTIME_ARG}" ] && echo "   4) drop uruntime-${AI_ARCH} into dist/ and rerun to build a FUSE-free AppImage."
    echo ""
    echo " The menu entry and the icons are registered in ~/.local/share on the"
    echo " first run (AppRun), so the shell shows this app by name and icon"
    echo " instead of falling back to the window title and a placeholder."
    echo " Set U3LA_NO_INTEGRATION=1 to skip that."
fi
echo "============================================"

fi
