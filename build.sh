#!/bin/bash
# ============================================================================
# build.sh - CH32H417 U3LogicAnalyzer Build + Package
# Usage:
#   ./build.sh              # Build only (Qt 6)
#   ./build.sh --qt6        # Build against Qt 6 (default)
#   ./build.sh --qt5        # Build against Qt 5
#   ./build.sh --package    # Build + create dist/U3LogicAnalyzer/ package
#   ./build.sh --clean      # Clean build
#   ./build.sh --debug      # Debug build with symbols, in build_cmake_debug/
#                           # (slower, not packageable: use it under gdb)
# ============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
# U3_BUILD_DIR keeps the tree off the sources: a build can be put on another
# disk without touching this script.
BUILD_DIR="${U3_BUILD_DIR:-${SCRIPT_DIR}/build_cmake}"
PREFIX="${SCRIPT_DIR}/install"
DIST_DIR="${SCRIPT_DIR}/dist/U3LogicAnalyzer"
CLEAN=0
PACKAGE=0
DEBUG=0
BUILD_TYPE="Release"
IS_WINDOWS=0
IS_MACOS=0

# Qt version: 6 (default) or 5. Scanned before the platform detection,
# which needs it to locate the Qt installation.
QT_MAJOR=6
for arg in "$@"; do
    case "$arg" in
        --qt5) QT_MAJOR=5 ;;
        --qt6) QT_MAJOR=6 ;;
    esac
done
# Set per platform below; empty on Linux, where Qt comes from the system.
QT_PREFIX=""
# Handed to the application's cmake call only.
QT_CMAKE_EXTRA="-DQT_VERSION_MAJOR=$QT_MAJOR"

# Platform detection
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ -n "$MSYSTEM" ]]; then
    IS_WINDOWS=1
    export PATH="/mingw64/bin:/usr/bin:$PATH"
    QT_PREFIX="/mingw64/qt${QT_MAJOR}-static"
    QT_CMAKE_EXTRA="$QT_CMAKE_EXTRA -DCMAKE_PREFIX_PATH=$QT_PREFIX"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    IS_MACOS=1
    if command -v brew &>/dev/null; then
        QT_PREFIX="$(brew --prefix "qt@${QT_MAJOR}" 2>/dev/null || brew --prefix qt 2>/dev/null || true)"
        PYTHON_PREFIX="$(brew --prefix python@3.12 2>/dev/null || brew --prefix python3 2>/dev/null || true)"
        if [ -n "$QT_PREFIX" ]; then
            export PATH="$QT_PREFIX/bin:$PATH"
        fi
        if [ -n "$PYTHON_PREFIX" ] && [ -d "$PYTHON_PREFIX/Frameworks/Python.framework/Versions/3.12" ]; then
            export PKG_CONFIG_PATH="$PYTHON_PREFIX/Frameworks/Python.framework/Versions/3.12/lib/pkgconfig:${PKG_CONFIG_PATH}"
        fi
        # Qt is found through CMAKE_PREFIX_PATH like the other formulas; add
        # it here instead of on the command line so the rest is not dropped.
        # "set -e" would end the script on a failed test, so use an if.
        if [ -n "$QT_PREFIX" ]; then
            export CMAKE_PREFIX_PATH="${QT_PREFIX}${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"
        fi
        for formula in "qt@${QT_MAJOR}" boost python@3.12 glib glibmm@2.66 libsigc++@2 libusb hidapi libzip; do
            p="$(brew --prefix "$formula" 2>/dev/null || true)"
            [ -z "$p" ] && continue
            export CMAKE_PREFIX_PATH="${p}${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"
            [ -d "$p/lib" ] && export LDFLAGS="-L$p/lib ${LDFLAGS}"
            [ -d "$p/include" ] && export CPPFLAGS="-I$p/include ${CPPFLAGS}"
            [ -d "$p/lib/pkgconfig" ] && export PKG_CONFIG_PATH="$p/lib/pkgconfig:${PKG_CONFIG_PATH}"
        done
        if ! pkg-config --exists 'glibmm-2.4 >= 2.32.0'; then
            echo "[error] glibmm-2.4 not found. Install: brew install glibmm@2.66 libsigc++@2"
            echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH"
            exit 1
        fi
    fi
fi

JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

while [ $# -gt 0 ]; do
    case $1 in
        --clean) CLEAN=1 ;;
        --package) PACKAGE=1 ;;
        --debug) DEBUG=1 ;;
        --qt5|--qt6) ;;   # already picked up above
        --prefix)
            if [ $# -lt 2 ]; then
                echo "[error] --prefix requires a path argument" >&2
                exit 1
            fi
            PREFIX="$2"
            shift
            ;;
        *)
            echo "[warn] Unknown option: $1" >&2
            ;;
    esac
    shift
done

if [ $DEBUG -eq 1 ]; then
    if [ $PACKAGE -eq 1 ]; then
        echo "[error] --debug cannot be combined with --package: a package is shipped optimised" >&2
        exit 1
    fi
    BUILD_TYPE="Debug"
    # Installed beside the release one as well: a debug build of the sigrok
    # libraries must not quietly replace the ones the release build uses.
    PREFIX="${SCRIPT_DIR}/install_debug"
    # A tree of its own: -O0 objects and release objects do not belong in the
    # same directory, and switching back and forth would rebuild everything
    # anyway. Symbols stay in, which is the point - gdb needs them.
    if [ -n "${U3_BUILD_DIR:-}" ]; then
        BUILD_DIR="${U3_BUILD_DIR}_debug"
    else
        BUILD_DIR="${SCRIPT_DIR}/build_cmake_debug"
    fi
fi

# ============================================================================
# Python version detection
# Detect instead of hardcoding (e.g. "3.14") so that an MSYS2 Python upgrade
# cannot silently break the packaged protocol decoders.
# ============================================================================
PYVER=""
if command -v python3 >/dev/null 2>&1; then
    PYVER="$(python3 -c 'import sys; print("%d.%d" % sys.version_info[:2])' 2>/dev/null || true)"
fi
if [ -z "$PYVER" ]; then
    PYVER="$(pkg-config --modversion python3 2>/dev/null || true)"
fi
if [ -z "$PYVER" ]; then
    echo "[warn] Could not detect the Python version; assuming 3.14" >&2
    PYVER="3.14"
fi
PYDLL="libpython${PYVER}.dll"
PYTHON_SRC="/mingw64/lib/python${PYVER}"

echo "============================================"
echo " CH32H417 U3LogicAnalyzer Build"
echo "============================================"
echo "Source:  $SCRIPT_DIR"
echo "Build:   $BUILD_DIR"
echo "Install: $PREFIX"
echo "Qt:      $QT_MAJOR${QT_PREFIX:+ ($QT_PREFIX)}"
echo "Type:    $BUILD_TYPE"
[ $PACKAGE -eq 1 ] && echo "Package: $DIST_DIR"
echo ""

if [ $CLEAN -eq 1 ]; then
    echo "[CLEAN] Removing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# MinGW detection
MINGW_EXTRA=""
if [ $IS_WINDOWS -eq 1 ]; then
    echo "Detected Windows/MinGW build environment"
    MINGW_EXTRA="-DWIN32=ON"
elif [ $IS_MACOS -eq 1 ]; then
    echo "Detected macOS build environment"
    # GitHub Actions runners may use a newer SDK than end-user Macs; target 11.0 explicitly.
    export MACOSX_DEPLOYMENT_TARGET=11.0
    MINGW_EXTRA="-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0"
fi

# ============================================================================
# Step 1: libsigrok (CH32H417 + Demo)
# ============================================================================
echo ""
echo "===== Step 1/4: libsigrok (CH32H417 + Demo) ====="

mkdir -p libsigrok_build && cd libsigrok_build
cmake "${SCRIPT_DIR}/libsigrok" \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DDISABLE_WERROR=ON $MINGW_EXTRA
cmake --build . -j"$JOBS"
cmake --install .
echo "[OK] libsigrok"

# ============================================================================
# Step 2: libsigrokcxx (C++ bindings)
# ============================================================================
echo ""
echo "===== Step 2/4: libsigrokcxx ====="

cd "$BUILD_DIR" && mkdir -p libsigrokcxx_build && cd libsigrokcxx_build
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH}"
cmake "${SCRIPT_DIR}/libsigrok/bindings/cxx" \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_PREFIX_PATH="$PREFIX" $MINGW_EXTRA
cmake --build . -j"$JOBS"
cmake --install .
echo "[OK] libsigrokcxx"

# ============================================================================
# Step 3: libsigrokdecode
# ============================================================================
echo ""
echo "===== Step 3/4: libsigrokdecode ====="

cd "$BUILD_DIR" && mkdir -p libsigrokdecode_build && cd libsigrokdecode_build
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH}"
cmake "${SCRIPT_DIR}/libsigrokdecode" \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DDISABLE_WERROR=ON $MINGW_EXTRA
cmake --build . -j"$JOBS"
cmake --install .
echo "[OK] libsigrokdecode"

# ============================================================================
# Step 4: U3LogicAnalyzer
# ============================================================================
echo ""
echo "===== Step 4/4: U3LogicAnalyzer ====="

# Generate .qm translation files from .ts sources
if ls "${SCRIPT_DIR}/pulseview/l10n/"*.ts >/dev/null 2>&1; then
    echo "  Generating translations..."
    if [ $IS_WINDOWS -eq 1 ]; then
        export PATH="$QT_PREFIX/bin:$PATH"
    fi
    for ts in "${SCRIPT_DIR}/pulseview/l10n/"*.ts; do
        [ -f "$ts" ] && lrelease "$ts" -qm "${ts%.ts}.qm" 2>/dev/null || true
    done
    echo "  [OK] Translations generated"
fi

cd "$BUILD_DIR" && mkdir -p logicanalyzer_build && cd logicanalyzer_build
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH}"
cmake "${SCRIPT_DIR}/pulseview" \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DSTATIC_PKGDEPS_LIBS=OFF \
    -DENABLE_DECODE=ON \
    -DENABLE_FLOW=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_SIGNALS=OFF \
    -DDISABLE_WERROR=ON $MINGW_EXTRA $QT_CMAKE_EXTRA
cmake --build . -j"$JOBS"

PVDIR="$BUILD_DIR/logicanalyzer_build"
PV_BIN="$PVDIR/U3LogicAnalyzer"
[ $IS_WINDOWS -eq 1 ] && PV_BIN="$PVDIR/U3LogicAnalyzer.exe"

# Runtime DLLs taken from /mingw64/bin.  Neither Qt build is fully
# static: Qt 6 links against the system image and text stack (webp,
# freetype, harfbuzz, png, jpeg, tiff and their dependencies), so these
# are needed next to the executable.  Kept in one place, because the
# copy in the build directory and the copy in the package must agree.
SYSTEM_DLLS="libglib-2.0-0.dll libglibmm-2.4-1.dll libgobject-2.0-0.dll libsigc-2.0-0.dll libstdc++-6.dll libwinpthread-1.dll libgcc_s_seh-1.dll libgmodule-2.0-0.dll libusb-1.0.dll libhidapi-0.dll libintl-8.dll libiconv-2.dll libpcre2-8-0.dll libffi-8.dll libzip.dll libbz2-1.dll liblzma-5.dll zlib1.dll libzstd.dll libpcre2-16-0.dll libpng16-16.dll libjpeg-8.dll libtiff-6.dll libwebp-7.dll libwebpmux-3.dll libwebpdemux-2.dll libsharpyuv-0.dll libbrotlicommon.dll libbrotlidec.dll libfreetype-6.dll libharfbuzz-0.dll libgraphite2.dll libLerc.dll libdeflate.dll libjbig-0.dll libb2-1.dll"

if [ $IS_WINDOWS -eq 1 ]; then
    # Deploy DLLs and create run.bat in build dir
    echo ""
    echo "  Deploying DLLs..."
    PVDIR="$BUILD_DIR/logicanalyzer_build"
    for dll in libsigrok.dll libsigrokcxx.dll libsigrokdecode.dll; do
        cp "$PREFIX/bin/$dll" "$PVDIR/" 2>/dev/null || true
        cp "$BUILD_DIR/libsigrok_build/$dll" "$PVDIR/" 2>/dev/null || true
        cp "$BUILD_DIR/libsigrokdecode_build/$dll" "$PVDIR/" 2>/dev/null || true
    done
    for dll in $SYSTEM_DLLS "$PYDLL"; do
        cp "/mingw64/bin/$dll" "$PVDIR/" 2>/dev/null || true
    done
    # Override the system libusb: the MSYS2 one has no raw-IO API, and an FX2
    # stops mid-capture without that policy (see driver/libusb/README.md).
    LIBUSB_VENDORED="$SCRIPT_DIR/driver/libusb/libusb-1.0.dll"
    if [ -f "$LIBUSB_VENDORED" ]; then
        cp -f "$LIBUSB_VENDORED" "$PVDIR/"
        echo "  libusb-1.0.dll (vendored)"
    fi
#     cat > "$PVDIR/run.bat" << 'RBEOF'
# @echo off
# cd /d "%~dp0"
# set "PATH=%~dp0;%PATH%"
# set "PYTHONHOME=%~dp0python-portable"
# set "SIGROKDECODE_DIR=%~dp0decoders"
# if exist "..\..\install\share\libsigrokdecode\decoders" (
#     set "SIGROKDECODE_DIR=..\..\install\share\libsigrokdecode\decoders"
#     set "PYTHONHOME="
# )
# start "" "U3LogicAnalyzer.exe"
# RBEOF

    # Qt looks its own translations up under share/qt6/translations, so the
    # build tree needs its own copy: without it the standard buttons stay in
    # English there while the package shows them translated.
    QT_TRANSLATIONS_SRC="$QT_PREFIX/share/qt${QT_MAJOR}/translations"
    if [ -d "$QT_TRANSLATIONS_SRC" ]; then
        mkdir -p "$PVDIR/share/qt${QT_MAJOR}/translations"
        for lang in zh_CN de es; do
            cp "$QT_TRANSLATIONS_SRC"/*_${lang}.qm \
                "$PVDIR/share/qt${QT_MAJOR}/translations/" 2>/dev/null || true
        done
        echo "  Qt translations"
    fi

    # Firmware stays beside the executable: an FX2 asks for it on every scan,
    # and run_gui.bat points SIGROK_FIRMWARE_DIR at this copy.
    if [ -d "$PREFIX/share/sigrok-firmware" ]; then
        mkdir -p "$PVDIR/share/sigrok-firmware"
        cp "$PREFIX/share/sigrok-firmware"/*.fw \
            "$PVDIR/share/sigrok-firmware/" 2>/dev/null || true
        echo "  $(ls "$PVDIR/share/sigrok-firmware" | wc -l) firmware files"
    fi

    # Launcher carrying the environment the build itself does not know:
    # the Python runtime for the decoders, and the firmware directory. A
    # packaged build brings its own runtime, so it gets none.
    if [ $PACKAGE -eq 0 ]; then
        MINGW_WIN="$(cygpath -m /mingw64 2>/dev/null || echo 'D:/msys64/mingw64')"
        cat > "$PVDIR/run_gui.bat" << RBEOF
@echo off
rem Start the build with the Python runtime and firmware the drivers need.
cd /d "%~dp0"
set "PYTHONHOME=$MINGW_WIN"
set "PYTHONPATH="
set "SIGROK_FIRMWARE_DIR=%~dp0share\\sigrok-firmware"
start "" "%~dp0U3LogicAnalyzer.exe"
RBEOF
        echo "  run_gui.bat"
    else
        rm -f "$PVDIR/run_gui.bat" 2>/dev/null || true
    fi

    echo "  $(ls "$PVDIR"/*.dll 2>/dev/null | wc -l) DLLs"
    if [ $DEBUG -eq 0 ]; then
        echo "  Stripping debug symbols..."
        strip "$PVDIR/U3LogicAnalyzer.exe" 2>/dev/null || echo "  (strip not available, skipping)"
        echo "  $(du -sh "$PVDIR/U3LogicAnalyzer.exe" | cut -f1) after strip"
    else
        echo "  Symbols kept (debug build)"
    fi
fi

echo ""
echo "===== BUILD COMPLETE ====="
echo "  U3LogicAnalyzer: $PV_BIN"
echo ""

# ============================================================================
# Package: create self-contained distribution
# ============================================================================
if [ $PACKAGE -eq 1 ]; then
    echo "===== Packaging to $DIST_DIR ====="
    rm -rf "$DIST_DIR"
    mkdir -p "$DIST_DIR"

    if [ $IS_WINDOWS -eq 1 ]; then
    	# 1. Copy exe
        cp "$BUILD_DIR/logicanalyzer_build/U3LogicAnalyzer.exe" "$DIST_DIR/"
        echo "  U3LogicAnalyzer.exe"

	# 2. Copy our DLLs
        for dll in libsigrok.dll libsigrokcxx.dll libsigrokdecode.dll; do
            cp "$PREFIX/bin/$dll" "$DIST_DIR/" 2>/dev/null || \
            cp "$BUILD_DIR/libsigrok_build/$dll" "$DIST_DIR/" 2>/dev/null || \
            cp "$BUILD_DIR/libsigrokdecode_build/$dll" "$DIST_DIR/" 2>/dev/null || true
            [ -f "$DIST_DIR/$dll" ] && echo "  $dll"
        done

	# 3. Copy system DLLs
        echo "  System DLLs..."
        for dll in $SYSTEM_DLLS "$PYDLL"; do
            cp "/mingw64/bin/$dll" "$DIST_DIR/" 2>/dev/null || true
        done
        # Override the system libusb: the MSYS2 one has no raw-IO API, and an
        # FX2 stops mid-capture without that policy (see driver/libusb/README.md).
        LIBUSB_VENDORED="$SCRIPT_DIR/driver/libusb/libusb-1.0.dll"
        if [ -f "$LIBUSB_VENDORED" ]; then
            cp -f "$LIBUSB_VENDORED" "$DIST_DIR/"
            echo "    libusb-1.0.dll (vendored)"
        fi

	# 4. Copy Python standard library (needed for protocol decoders)
        echo "  Python standard library..."
        if [ -d "$PYTHON_SRC" ]; then
            mkdir -p "$DIST_DIR/python-portable/lib/python${PYVER}"
            if [ -d "$PYTHON_SRC/lib-dynload" ]; then
                cp -r "$PYTHON_SRC/lib-dynload" "$DIST_DIR/python-portable/lib/python${PYVER}/"
            fi
            for dir in asyncio collections concurrent ctypes distutils email encodings \
                       html http importlib json lib logging multiprocessing \
                       pydoc_data re sqlite3 unittest urllib wsgi xml xmlrpc \
                       _collections_abc.py _compat_pickle.py _py_abc.py \
                       _sitebuiltins.py site.py string.py \
                       abc.py ast.py base64.py bisect.py calendar.py cgi.py \
                       codecs.py copy.py copyreg.py csv.py dataclasses.py \
                       datetime.py decimal.py dis.py enum.py fnmatch.py \
                       functools.py glob.py hashlib.py heapq.py inspect.py io.py \
                       keyword.py linecache.py locale.py math.py operator.py \
                       os.py pathlib.py pickle.py platform.py plistlib.py \
                       pprint.py queue.py random.py reprlib.py selectors.py \
                       shutil.py signal.py socket.py sre_compile.py \
                       sre_constants.py sre_parse.py stat.py string string.py \
                       struct.py subprocess.py sysconfig.py tempfile.py \
                       textwrap.py threading.py token.py tokenize.py \
                       _weakrefset.py sysconfig _pydecimal.py _py_warnings.py numbers.py contextlib.py contextvars.py \
                       types.py typing.py uuid.py warnings.py weakref.py; do
                cp -r "$PYTHON_SRC/$dir" "$DIST_DIR/python-portable/lib/python${PYVER}/" 2>/dev/null || true
            done
            echo "    $(find "$DIST_DIR/python-portable" -type f | wc -l) files"
        else
            echo "[error] Python standard library not found at $PYTHON_SRC" >&2
            echo "        The packaged protocol decoders would not work; aborting." >&2
            exit 1
        fi

	# 5. Copy protocol decoders
    	echo "  Protocol decoders..."
        DECODERS_SRC="$PREFIX/share/libsigrokdecode/decoders"
        if [ -d "$DECODERS_SRC" ]; then
            cp -r "$DECODERS_SRC" "$DIST_DIR/decoders"
            echo "    $(ls "$DIST_DIR/decoders" | wc -l) decoders"
        fi

	# Firmware: an FX2 based analyzer has no flash of its own, so the driver
	# downloads the firmware into it every time the board is scanned.
	FIRMWARE_SRC="$PREFIX/share/sigrok-firmware"
	if [ -d "$FIRMWARE_SRC" ]; then
		echo "  Firmware..."
		mkdir -p "$DIST_DIR/share"
		cp -r "$FIRMWARE_SRC" "$DIST_DIR/share/sigrok-firmware"
		echo "    $(ls "$DIST_DIR/share/sigrok-firmware" | wc -l) firmware files"
	fi

	# 6. Create run.bat
#         cat > "$DIST_DIR/run.bat" << 'EOF'
# @echo off
# cd /d "%~dp0"
# set "PATH=%~dp0;%PATH%"
# set "PYTHONHOME=%~dp0python-portable"
# set "SIGROKDECODE_DIR=%~dp0decoders"
# echo Starting U3LogicAnalyzer...
# U3LogicAnalyzer.exe
# if %ERRORLEVEL% neq 0 (
#     echo.
#     echo U3LogicAnalyzer exited with error code %ERRORLEVEL%
#     pause
# )
# EOF
#         echo "  run.bat"
#         echo ""
#         echo "To distribute: copy the entire dist/U3LogicAnalyzer/ folder."
#         echo "To run: double-click run.bat or U3LogicAnalyzer.exe"
    # 7. Copy driver directory. driver/libusb holds the vendored libusb the
    #    build uses - its DLL is shipped beside the executable (step 8), and
    #    the patch and notes in there are for whoever builds it, not for the
    #    package, so that one directory is left out.
    echo "  Driver directory..."
    mkdir -p "$DIST_DIR/driver"
    for item in "$SCRIPT_DIR/driver"/*; do
        [ "$(basename "$item")" = "libusb" ] && continue
        cp -r "$item" "$DIST_DIR/driver/"
    done

    # 8. Copy license file
    echo "  License file..."
    cp "$SCRIPT_DIR/LICENSE" "$DIST_DIR/"

    # 9. Run clean__pycache__.sh
    if [ $IS_WINDOWS -eq 1 ]; then
        echo "  Clean __pycache__..."
        "$SCRIPT_DIR/clean__pycache__.sh"
    fi

    # 10. Copy Qt translations
    echo "  Qt translations..."
    QT_TRANSLATIONS_SRC="$QT_PREFIX/share/qt${QT_MAJOR}/translations"
    LANGUAGES="zh_CN en de es ru"

    if [ -d "$QT_TRANSLATIONS_SRC" ]; then
        mkdir -p "$DIST_DIR/share/qt${QT_MAJOR}/translations"
        for lang in $LANGUAGES; do
            cp "$QT_TRANSLATIONS_SRC"/*_${lang}.qm "$DIST_DIR/share/qt${QT_MAJOR}/translations/" 2>/dev/null
        done
    fi

    elif [ $IS_MACOS -eq 1 ]; then
        APP="$DIST_DIR/U3LogicAnalyzer.app"
        MACOS_DIR="$APP/Contents/MacOS"
        RES_DIR="$APP/Contents/Resources"
        FRAMEWORKS_DIR="$APP/Contents/Frameworks"
        MAIN_BIN="$MACOS_DIR/U3LogicAnalyzer"
        mkdir -p "$MACOS_DIR" "$RES_DIR" "$FRAMEWORKS_DIR"

        # install_name_tool invalidates code signatures; always re-sign Mach-O files after.
        MACOS_BUNDLE_VISITED="$(mktemp)"
        macos_is_macho() {
            file "$1" 2>/dev/null | grep -q 'Mach-O'
        }
        macos_rpaths_for() {
            otool -l "$1" 2>/dev/null | awk '/cmd LC_RPATH/{found=1} found && /path / {print $2; found=0}'
        }
        macos_resolve_dep() {
            local binary="$1" dep="$2" base rpath resolved pyfw suffix
            case "$dep" in
                @executable_path/*|@loader_path/*)
                    echo "$dep"
                    return 0
                    ;;
                @rpath/*)
                    base="${dep#@rpath/}"
                    if [[ "$base" == Python.framework/* ]]; then
                        suffix="${base#Python.framework/}"
                        pyfw="$(brew --prefix python@3.12 2>/dev/null)/Frameworks/Python.framework/$suffix"
                        [ -f "$pyfw" ] && echo "$pyfw" && return 0
                    fi
                    for rpath in $(macos_rpaths_for "$binary"); do
                        resolved="$rpath/$base"
                        [ -f "$resolved" ] && echo "$resolved" && return 0
                    done
                    for rpath in /opt/homebrew/lib /opt/homebrew/opt/webp/lib \
                        /opt/homebrew/opt/libpng/lib "$PREFIX/lib"; do
                        resolved="$rpath/$(basename "$base")"
                        [ -f "$resolved" ] && echo "$resolved" && return 0
                        resolved="$rpath/$base"
                        [ -f "$resolved" ] && echo "$resolved" && return 0
                    done
                    return 1
                    ;;
            esac
            if [[ "$dep" == */Python.framework/* ]] && [ ! -f "$dep" ]; then
                suffix="${dep#*Python.framework/}"
                pyfw="$(brew --prefix python@3.12 2>/dev/null)/Frameworks/Python.framework/$suffix"
                [ -f "$pyfw" ] && echo "$pyfw" && return 0
            fi
            echo "$dep"
            return 0
        }
        macos_should_bundle_dep() {
            local dep="$1"
            case "$dep" in
                @executable_path/*|@loader_path/*|@rpath/*) return 1 ;;
                /usr/lib/*|/System/Library/*) return 1 ;;
            esac
            [[ "$dep" == "$APP/Contents/"* ]] && return 1
            return 0
        }
        macos_bundle_framework() {
            local dep="$1" binary="$2" ref="${3:-$1}"
            local fw_root fw_name fw_dest rel_path inner
            fw_root="${dep%%.framework/*}.framework"
            [ -d "$fw_root" ] || return 0
            grep -qxF "$fw_root" "$MACOS_BUNDLE_VISITED" 2>/dev/null && return 0
            echo "$fw_root" >> "$MACOS_BUNDLE_VISITED"

            fw_name="$(basename "$fw_root" .framework)"
            fw_dest="$FRAMEWORKS_DIR/$fw_name.framework"
            rm -rf "$fw_dest"
            cp -R "$fw_root" "$fw_dest"
            echo "    $fw_name.framework"
            rel_path="${dep#*${fw_name}.framework}"
            for inner in \
                "$fw_dest/Versions/5/$fw_name" \
                "$fw_dest/Versions/A/$fw_name" \
                "$fw_dest/Versions/Current/$fw_name"; do
                if [ -f "$inner" ]; then
                    install_name_tool -change "$ref" \
                        "@executable_path/../Frameworks/$fw_name.framework$rel_path" \
                        "$binary" 2>/dev/null || true
                    macos_fixup_binary "$inner"
                    break
                fi
            done
        }
        macos_bundle_dylib() {
            local dep="$1" binary="$2" ref="${3:-$1}"
            macos_should_bundle_dep "$dep" || return 0
            [ -f "$dep" ] || return 0
            grep -qxF "$dep" "$MACOS_BUNDLE_VISITED" 2>/dev/null && return 0
            echo "$dep" >> "$MACOS_BUNDLE_VISITED"

            local base dest new_path
            base="$(basename "$dep")"
            dest="$FRAMEWORKS_DIR/$base"
            cp -f "$dep" "$dest"
            chmod 755 "$dest"
            install_name_tool -id "@loader_path/$base" "$dest" 2>/dev/null || true
            echo "    $base"
            if [[ "$binary" == "$MAIN_BIN" ]]; then
                new_path="@executable_path/../Frameworks/$base"
            else
                new_path="@loader_path/$base"
            fi
            install_name_tool -change "$ref" "$new_path" "$binary" 2>/dev/null || true
            macos_fixup_binary "$dest"
        }
        macos_fixup_binary() {
            local binary="$1" dep resolved
            macos_is_macho "$binary" || return 0
            while read -r dep; do
                [ -n "$dep" ] || continue
                resolved="$(macos_resolve_dep "$binary" "$dep")" || continue
                macos_should_bundle_dep "$resolved" || continue
                [ -f "$resolved" ] || continue
                if [[ "$resolved" == *".framework/"* ]]; then
                    macos_bundle_framework "$resolved" "$binary" "$dep"
                else
                    macos_bundle_dylib "$resolved" "$binary" "$dep"
                fi
            done < <(otool -L "$binary" 2>/dev/null | tail -n +2 | awk '{print $1}')
        }
        macos_has_external_deps() {
            local f dep resolved
            for f in "$@"; do
                [ -f "$f" ] || continue
                while read -r dep; do
                    resolved="$(macos_resolve_dep "$f" "$dep")" || continue
                    macos_should_bundle_dep "$resolved" || continue
                    [ -f "$resolved" ] && return 0
                done < <(otool -L "$f" 2>/dev/null | tail -n +2 | awk '{print $1}')
            done
            return 1
        }
        macos_macdeployqt_libpaths() {
            local args=() dir
            for dir in /opt/homebrew/lib /opt/homebrew/opt/webp/lib \
                /opt/homebrew/opt/libpng/lib /opt/homebrew/opt/gettext/lib \
                "$PREFIX/lib"; do
                [ -d "$dir" ] && args+=(-libpath="$dir")
            done
            if [ -n "${PYTHON_PREFIX:-}" ] && [ -d "$PYTHON_PREFIX/Frameworks" ]; then
                args+=(-libpath="$PYTHON_PREFIX/Frameworks")
            fi
            printf '%s\0' "${args[@]}"
        }
        macos_bundle_all_deps() {
            local round=0
            while [ "$round" -lt 25 ]; do
                round=$((round + 1))
                : > "$MACOS_BUNDLE_VISITED"
                macos_fixup_binary "$MAIN_BIN"
                for lib in "$FRAMEWORKS_DIR"/*.dylib; do
                    [ -f "$lib" ] || continue
                    macos_fixup_binary "$lib"
                done
                for fw in "$FRAMEWORKS_DIR"/*.framework; do
                    [ -d "$fw" ] || continue
                    local fwname inner
                    fwname="$(basename "$fw" .framework)"
                    for inner in "$fw/Versions/"*"/$fwname"; do
                        [ -f "$inner" ] && macos_fixup_binary "$inner"
                    done
                done
                macos_has_external_deps "$MAIN_BIN" "$FRAMEWORKS_DIR"/*.dylib || break
            done
        }
        macos_set_minimum_system_version() {
            local plist="$APP/Contents/Info.plist"
            # macdeployqt may raise LSMinimumSystemVersion to the CI runner's macOS release.
            /usr/libexec/PlistBuddy -c "Delete :LSMinimumSystemVersion" "$plist" 2>/dev/null || true
            /usr/libexec/PlistBuddy -c "Add :LSMinimumSystemVersion string 11.0" "$plist"
            echo "  LSMinimumSystemVersion set to 11.0"
        }
        macos_normalize_python_framework() {
            local py_home="${PYTHON_PREFIX:-$(brew --prefix python@3.12 2>/dev/null)}"
            local py_src="$py_home/Frameworks/Python.framework"
            local py_fw="$FRAMEWORKS_DIR/Python.framework"
            local py_target="@executable_path/../Frameworks/Python.framework/Versions/Current/Python"
            local f old ver

            [ -d "$py_src" ] || return 0
            echo "  Normalizing Python.framework to python@3.12..."
            rm -rf "$py_fw"
            cp -R "$py_src" "$py_fw"
            for ver in "$py_fw/Versions/"*; do
                [ -e "$ver" ] || continue
                [ "$(basename "$ver")" = "3.12" ] && continue
                [ "$(basename "$ver")" = "Current" ] && continue
                rm -rf "$ver"
            done
            ln -sfh 3.12 "$py_fw/Versions/Current"
            find "$py_fw" -type l | while read -r link; do
                [ -e "$link" ] || rm -f "$link"
            done
            while IFS= read -r -d '' f; do
                macos_is_macho "$f" || continue
                while read -r old; do
                    [[ "$old" != *Python.framework* ]] && continue
                    [[ "$old" == "$py_target" ]] && continue
                    install_name_tool -change "$old" "$py_target" "$f" 2>/dev/null || true
                done < <(otool -L "$f" 2>/dev/null | tail -n +2 | awk '{print $1}')
            done < <(find "$APP/Contents" -type f -print0)
        }
        macos_sign_file() {
            codesign --force --sign - --timestamp=none "$1" || {
                echo "[error] codesign failed: $1"
                exit 1
            }
        }
        macos_sign_app() {
            local app="$1" f fw fwname inner
            echo "  Ad-hoc signing app bundle (inner libraries first)..."
            for f in "$app/Contents/Frameworks"/*.dylib; do
                [ -f "$f" ] && macos_sign_file "$f"
            done
            for fw in "$app/Contents/Frameworks"/*.framework; do
                [ -d "$fw" ] || continue
                fwname="$(basename "$fw" .framework)"
                for inner in \
                    "$fw/Versions/5/$fwname" \
                    "$fw/Versions/A/$fwname" \
                    "$fw/Versions/Current/$fwname"; do
                    if [ -f "$inner" ]; then
                        macos_sign_file "$inner"
                        break
                    fi
                done
                macos_sign_file "$fw"
            done
            while IFS= read -r -d '' f; do
                macos_sign_file "$f"
            done < <(find "$app/Contents/PlugIns" -name '*.dylib' -type f -print0 2>/dev/null)
            macos_sign_file "$MAIN_BIN"
            macos_sign_file "$app"
            if ! codesign --verify --deep "$app" 2>&1; then
                echo "[error] codesign verification failed"
                otool -L "$MAIN_BIN" | head -30 || true
                exit 1
            fi
            echo "  codesign verification OK"
        }

        cat > "$APP/Contents/Info.plist" << 'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>U3LogicAnalyzer</string>
    <key>CFBundleIdentifier</key>
    <string>com.q2h2.logicanalyzer</string>
    <key>CFBundleName</key>
    <string>U3LogicAnalyzer</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleVersion</key>
    <string>0.2</string>
    <key>CFBundleShortVersionString</key>
    <string>0.2</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
PLIST

        cp "$BUILD_DIR/logicanalyzer_build/U3LogicAnalyzer" "$MAIN_BIN"
        chmod +x "$MAIN_BIN"
        echo "  U3LogicAnalyzer.app/Contents/MacOS/U3LogicAnalyzer"

        # macdeployqt needs libsharpyuv (Qt imageformats) on the search path
        for sharpyuv in /opt/homebrew/lib/libsharpyuv.0.dylib \
            /opt/homebrew/opt/webp/lib/libsharpyuv.0.dylib; do
            if [ -f "$sharpyuv" ]; then
                cp -f "$sharpyuv" "$FRAMEWORKS_DIR/"
                install_name_tool -id "@loader_path/libsharpyuv.0.dylib" \
                    "$FRAMEWORKS_DIR/libsharpyuv.0.dylib" 2>/dev/null || true
                echo "  Pre-bundled libsharpyuv.0.dylib for macdeployqt"
                break
            fi
        done

        MACDEPLOYQT=""
        if command -v macdeployqt &>/dev/null; then
            MACDEPLOYQT="$(command -v macdeployqt)"
        elif [ -n "${QT_PREFIX:-}" ] && [ -x "$QT_PREFIX/bin/macdeployqt" ]; then
            MACDEPLOYQT="$QT_PREFIX/bin/macdeployqt"
        fi
        if [ -n "$MACDEPLOYQT" ]; then
            echo "  Running macdeployqt..."
            MACDEPLOYQT_ARGS=(-always-overwrite -codesign=-)
            while IFS= read -r -d '' arg; do
                MACDEPLOYQT_ARGS+=("$arg")
            done < <(macos_macdeployqt_libpaths)
            "$MACDEPLOYQT" "$APP" "${MACDEPLOYQT_ARGS[@]}"
        else
            echo "  (macdeployqt not found, Qt frameworks must be installed separately)"
        fi

        echo "  Bundling non-Qt libraries into Contents/Frameworks..."
        macos_bundle_all_deps
        rm -f "$MACOS_BUNDLE_VISITED"
        macos_normalize_python_framework
        macos_set_minimum_system_version

        DECODERS_SRC="$PREFIX/share/libsigrokdecode/decoders"
        if [ -d "$DECODERS_SRC" ]; then
            echo "  Protocol decoders..."
            cp -r "$DECODERS_SRC" "$RES_DIR/decoders"
            echo "    $(ls "$RES_DIR/decoders" | wc -l) decoders"
        fi

        FIRMWARE_SRC="$PREFIX/share/sigrok-firmware"
        if [ -d "$FIRMWARE_SRC" ]; then
            echo "  Firmware..."
            cp -r "$FIRMWARE_SRC" "$RES_DIR/sigrok-firmware"
            echo "    $(ls "$RES_DIR/sigrok-firmware" | wc -l) firmware files"
        fi

        macos_sign_app "$APP"

        cat > "$DIST_DIR/run.sh" << 'EOF'
#!/bin/bash
DIR="$(cd "$(dirname "$0")" && pwd)"
open "$DIR/U3LogicAnalyzer.app"
EOF
        chmod +x "$DIST_DIR/run.sh"
        echo "  run.sh"

        echo ""
        echo "To distribute: copy dist/U3LogicAnalyzer/ (U3LogicAnalyzer.app + run.sh)."
        echo "To run: open U3LogicAnalyzer.app or ./run.sh"
    else
        # Linux: bundle our sigrok libraries + decoders; Qt and other system
        # libraries are expected to be provided by the distribution.
        echo "  U3LogicAnalyzer"
        cp "$BUILD_DIR/logicanalyzer_build/U3LogicAnalyzer" "$DIST_DIR/"
        chmod +x "$DIST_DIR/U3LogicAnalyzer"

        echo "  sigrok libraries..."
        for lib in "$PREFIX/lib"/libsigrok.so* "$PREFIX/lib"/libsigrokcxx.so* \
                   "$PREFIX/lib"/libsigrokdecode.so*; do
            [ -e "$lib" ] || continue
            cp -L "$lib" "$DIST_DIR/"
        done
        ls "$DIST_DIR"/libsigrok* 2>/dev/null | while read -r f; do echo "  $(basename "$f")"; done

        echo "  Protocol decoders..."
        DECODERS_SRC="$PREFIX/share/libsigrokdecode/decoders"
        if [ -d "$DECODERS_SRC" ]; then
            mkdir -p "$DIST_DIR/share/libsigrokdecode"
            cp -r "$DECODERS_SRC" "$DIST_DIR/share/libsigrokdecode/decoders"
            echo "    $(ls "$DIST_DIR/share/libsigrokdecode/decoders" | wc -l) decoders"
        fi

        echo "  Firmware..."
        FIRMWARE_SRC="$PREFIX/share/sigrok-firmware"
        if [ -d "$FIRMWARE_SRC" ]; then
            cp -r "$FIRMWARE_SRC" "$DIST_DIR/share/sigrok-firmware"
            echo "    $(ls "$DIST_DIR/share/sigrok-firmware" | wc -l) firmware files"
        fi

        echo "  Qt translations..."
        QT_TRANSLATIONS_SRC="/usr/share/qt${QT_MAJOR}/translations"
        LANGUAGES="zh_CN en de es ru"
        if [ -d "$QT_TRANSLATIONS_SRC" ]; then
            mkdir -p "$DIST_DIR/share/qt${QT_MAJOR}/translations"
            for lang in $LANGUAGES; do
                cp "$QT_TRANSLATIONS_SRC"/*_${lang}.qm "$DIST_DIR/share/qt${QT_MAJOR}/translations/" 2>/dev/null || true
            done
        fi

        echo "  Application icon + desktop launcher..."
        ICON_SRC="$SCRIPT_DIR/pulseview/icons/pulseview.png"
        ICON_SVG_SRC="$SCRIPT_DIR/pulseview/icons/pulseview.svg"
        if [ -f "$ICON_SRC" ]; then
            mkdir -p "$DIST_DIR/share/icons"
            cp "$ICON_SRC" "$DIST_DIR/share/icons/U3LogicAnalyzer.png"
            [ -f "$ICON_SVG_SRC" ] && cp "$ICON_SVG_SRC" "$DIST_DIR/share/icons/U3LogicAnalyzer.svg"

            # Portable desktop file (not usable from the menu until installed
            # by install_desktop.sh, which rewrites the paths to absolute).
            cat > "$DIST_DIR/U3LogicAnalyzer.desktop" << EOF
[Desktop Entry]
Type=Application
Name=U3LogicAnalyzer
Comment=CH32H417 Logic Analyzer (WeAct Studio)
Exec=U3LogicAnalyzer
Icon=U3LogicAnalyzer
Terminal=false
Categories=Development;Electronics;
StartupWMClass=U3LogicAnalyzer
EOF

            # Helper that registers the icon + menu entry for the current user.
            cat > "$DIST_DIR/install_desktop.sh" << 'EOF'
#!/bin/bash
# Install (default) or remove (--remove) the menu entry and its icons.
DIR="$(cd "$(dirname "$0")" && pwd)"
HICOLOR_DIR="$HOME/.local/share/icons/hicolor"
# The shipped PNG is 200x200: 256x256 is the directory that matches it.
ICON_DIR="$HICOLOR_DIR/256x256/apps"
ICON_DIR_48="$HICOLOR_DIR/48x48/apps"
PIXMAP_DIR="$HOME/.local/share/pixmaps"
DESKTOP_DIR="$HOME/.local/share/applications"

if [ "$1" = "--remove" ] || [ "$1" = "--uninstall" ]; then
    rm -f "$DESKTOP_DIR/U3LogicAnalyzer.desktop"
    rm -f "$ICON_DIR/U3LogicAnalyzer.png"
    rm -f "$ICON_DIR_48/U3LogicAnalyzer.png"
    rm -f "$PIXMAP_DIR/U3LogicAnalyzer.png"
    rm -f "$HICOLOR_DIR/scalable/apps/U3LogicAnalyzer.svg"
    gtk-update-icon-cache -f -t "$HICOLOR_DIR" 2>/dev/null || true
    update-desktop-database "$DESKTOP_DIR" 2>/dev/null || true
    echo "Removed U3LogicAnalyzer menu entry and icons"
    exit 0
fi

mkdir -p "$ICON_DIR" "$ICON_DIR_48" "$PIXMAP_DIR" "$DESKTOP_DIR"
if [ -f "$DIR/share/icons/U3LogicAnalyzer.svg" ]; then
    SVG_DIR="$HOME/.local/share/icons/hicolor/scalable/apps"
    mkdir -p "$SVG_DIR"
    cp "$DIR/share/icons/U3LogicAnalyzer.svg" "$SVG_DIR/"
fi
cp "$DIR/share/icons/U3LogicAnalyzer.png" "$ICON_DIR/"
cp "$DIR/share/icons/U3LogicAnalyzer.png" "$ICON_DIR_48/"
cp "$DIR/share/icons/U3LogicAnalyzer.png" "$PIXMAP_DIR/"
sed "s|^Exec=.*|Exec=$DIR/U3LogicAnalyzer|; s|^Icon=.*|Icon=U3LogicAnalyzer|" \
    "$DIR/U3LogicAnalyzer.desktop" > "$DESKTOP_DIR/U3LogicAnalyzer.desktop"
gtk-update-icon-cache -f -t "$HOME/.local/share/icons/hicolor" 2>/dev/null || true
update-desktop-database "$DESKTOP_DIR" 2>/dev/null || true
echo "Installed menu entry: $DESKTOP_DIR/U3LogicAnalyzer.desktop"
EOF
            chmod +x "$DIST_DIR/install_desktop.sh"
            echo "    U3LogicAnalyzer.png, U3LogicAnalyzer.desktop, install_desktop.sh"
        else
            echo "  (icon source not found, skipping)"
        fi

        echo "  License file..."
        if [ -f "$SCRIPT_DIR/LICENSE" ]; then
            cp "$SCRIPT_DIR/LICENSE" "$DIST_DIR/"
        else
            echo "  (LICENSE not found, skipping)"
        fi

        echo ""
        echo "To distribute: copy dist/U3LogicAnalyzer/."
        echo "To run: ./U3LogicAnalyzer (Qt${QT_MAJOR} and other system libraries must be installed)."
    fi

    echo ""
    echo "===== PACKAGE COMPLETE ====="
    echo "  $DIST_DIR/"
fi

