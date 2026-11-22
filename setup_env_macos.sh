#!/bin/bash
# ============================================================================
# setup_env_macos.sh - CH32H417 U3LogicAnalyzer macOS build environment
# Requires Homebrew: https://brew.sh
#
# Usage:
#   ./setup_env_macos.sh          # Qt 6 (default)
#   ./setup_env_macos.sh --qt5    # Qt 5
#   ./setup_env_macos.sh --qt6    # Qt 6
# ============================================================================
set -e

QT_MAJOR=6
for arg in "$@"; do
    case "$arg" in
        --qt5) QT_MAJOR=5 ;;
        --qt6) QT_MAJOR=6 ;;
        -h|--help) sed -n '2,10p' "$0"; exit 0 ;;
        *) echo "[error] unknown option: $arg" >&2; exit 1 ;;
    esac
done

# Homebrew splits Qt 6 into modules. The meta formula "qt" would also pull
# qtwebengine - well over a gigabyte nothing here uses.
QT_FORMULA="qtbase"
QT_PACKAGES="qtbase qtsvg qttools qttranslations"
BUILD_ARGS=""
if [ "$QT_MAJOR" = "5" ]; then
    QT_FORMULA="qt@5"
    QT_PACKAGES="qt@5"
    BUILD_ARGS=" --qt5"
fi

if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "[error] This script is for macOS only"
    exit 1
fi

if ! command -v brew &>/dev/null; then
    echo "[error] Homebrew is required. Install from https://brew.sh"
    exit 1
fi

echo ">>> Installing build dependencies via Homebrew..."

brew update
# PulseView requires glibmm-2.4 / sigc++-2.0 (not Homebrew's default glibmm 2.88)
brew install \
    cmake ninja pkg-config \
    glib glibmm@2.66 libsigc++@2 libusb hidapi libzip \
    boost $QT_PACKAGES python@3.12 gettext

QT_PREFIX="$(brew --prefix "$QT_FORMULA")"
if [ ! -d "$QT_PREFIX" ]; then
    QT_PREFIX="$(brew --prefix qt)"
fi
echo "Using Qt ${QT_MAJOR} at ${QT_PREFIX}"
PYTHON_PREFIX="$(brew --prefix python@3.12)"
GLIBMM_PREFIX="$(brew --prefix glibmm@2.66)"
SIGC_PREFIX="$(brew --prefix libsigc++@2)"
GLIB_PREFIX="$(brew --prefix glib)"
PY_PKGCONFIG="$PYTHON_PREFIX/Frameworks/Python.framework/Versions/3.12/lib/pkgconfig"

export PKG_CONFIG_PATH="$PY_PKGCONFIG:$GLIBMM_PREFIX/lib/pkgconfig:$SIGC_PREFIX/lib/pkgconfig:$GLIB_PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH}"
if ! pkg-config --exists 'python3-embed >= 3.12'; then
    echo "[error] python3-embed (3.12) not found after install"
    echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH"
    pkg-config --list-all | grep -i python || true
    exit 1
fi
echo "Using python3-embed $(pkg-config --modversion python3-embed)"
if ! pkg-config --exists 'glibmm-2.4 >= 2.32.0'; then
    echo "[error] glibmm-2.4 not found after install"
    echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH"
    pkg-config --list-all | grep -i glib || true
    exit 1
fi

cat <<EOF

============================================
 Environment setup complete! (Qt ${QT_MAJOR})
============================================

Then build (./build.sh sets Homebrew pkg-config paths automatically):

  ./build.sh --package${BUILD_ARGS}
  open dist/U3LogicAnalyzer/U3LogicAnalyzer.app
EOF
