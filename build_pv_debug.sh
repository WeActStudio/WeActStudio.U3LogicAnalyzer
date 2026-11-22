set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${U3_BUILD_DIR:-${SCRIPT_DIR}/build_cmake}"
PREFIX="${SCRIPT_DIR}/install"
DIST_DIR="${SCRIPT_DIR}/dist/U3LogicAnalyzer"
CLEAN=0
PACKAGE=0
IS_WINDOWS=0
IS_MACOS=0
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Qt version: 6 (default) or 5, same arguments as build.sh.
QT_MAJOR=6
for arg in "$@"; do
    case "$arg" in
        --qt5) QT_MAJOR=5 ;;
        --qt6) QT_MAJOR=6 ;;
    esac
done
# Homebrew splits Qt 6 into modules; the meta formula "qt" also pulls
# qtwebengine, which this application never uses.
QT_FORMULA="qtbase"
if [ "$QT_MAJOR" = "5" ]; then
    QT_FORMULA="qt@5"
fi

# Platform detection
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ -n "$MSYSTEM" ]]; then
    IS_WINDOWS=1
    export PATH="/mingw64/bin:/usr/bin:$PATH"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    IS_MACOS=1
    if command -v brew &>/dev/null; then
        # "brew --prefix" answers for a formula that is not installed.
        QT_PREFIX="$(brew --prefix "$QT_FORMULA" 2>/dev/null || true)"
        if [ ! -d "$QT_PREFIX" ]; then
            QT_PREFIX="$(brew --prefix qt 2>/dev/null || true)"
            [ -d "$QT_PREFIX" ] || QT_PREFIX=""
        fi
        PYTHON_PREFIX="$(brew --prefix python@3.12 2>/dev/null || brew --prefix python3 2>/dev/null || true)"
        if [ -n "$QT_PREFIX" ]; then
            export PATH="$QT_PREFIX/bin:$PATH"
        fi
        if [ -n "$PYTHON_PREFIX" ] && [ -d "$PYTHON_PREFIX/Frameworks/Python.framework/Versions/3.12" ]; then
            export PKG_CONFIG_PATH="$PYTHON_PREFIX/Frameworks/Python.framework/Versions/3.12/lib/pkgconfig:${PKG_CONFIG_PATH}"
        fi
        # gettext is keg-only, so -L for libintl has to be added by hand.
        for formula in boost python@3.12 glib glibmm@2.66 libsigc++@2 libusb hidapi libzip gettext; do
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

if ls "${SCRIPT_DIR}/pulseview/l10n/"*.ts >/dev/null 2>&1; then
    echo "  Generating translations..."
    if [ $IS_WINDOWS -eq 1 ]; then
        export PATH="/mingw64/qt${QT_MAJOR}-static/bin:$PATH"
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
    -DCMAKE_BUILD_TYPE=Debug \
    -DQT_VERSION_MAJOR="$QT_MAJOR" \
    -DSTATIC_PKGDEPS_LIBS=OFF \
    -DENABLE_DECODE=ON \
    -DENABLE_FLOW=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_SIGNALS=OFF \
    -DDISABLE_WERROR=ON $MINGW_EXTRA
cmake --build . -j"$JOBS"