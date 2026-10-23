#!/bin/bash
# ============================================================================
# setup_env.sh - one-shot setup of the CH32H417 U3LogicAnalyzer build and runtime environment
# Run this inside an MSYS2 MINGW64 shell
#
# Usage:
#   ./setup_env.sh          # Qt 6 (default)
#   ./setup_env.sh --qt5    # Qt 5
#   ./setup_env.sh --qt6    # Qt 6
# ============================================================================
set -e

QT_MAJOR=6
for arg in "$@"; do
    case "$arg" in
        --qt5) QT_MAJOR=5 ;;
        --qt6) QT_MAJOR=6 ;;
        -h|--help) sed -n '2,9p' "$0"; exit 0 ;;
        *) echo "[error] unknown option: $arg" >&2; exit 1 ;;
    esac
done

if [[ "$MSYSTEM" != "MINGW64" ]]; then
    echo "[error] Please run this script in an MSYS2 MINGW64 shell (Start menu -> MSYS2 MINGW64)"
    exit 1
fi

echo ">>> Installing the build environment (Qt ${QT_MAJOR})..."

pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,python}

# qtN-static drops out of the mingw64 repo while a Qt release is rebuilt
# (6.12 did), so it is asked for on its own: the toolchain above is still
# worth having when only this one is missing.
QT_PACKAGE="mingw-w64-x86_64-qt${QT_MAJOR}-static"
if ! pacman -S --needed --noconfirm "${QT_PACKAGE}"; then
    echo "[warning] ${QT_PACKAGE} is not in the repo right now."
    echo "          Install it from a cached package, or wait until it is back."
fi

BUILD_ARGS=""
[ "${QT_MAJOR}" = "5" ] && BUILD_ARGS=" --qt5"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

cat <<EOF

============================================
 Environment setup complete! (Qt ${QT_MAJOR})
============================================

Runtime environment variables (set them in every new shell):

  export PYTHONHOME=/mingw64
  export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders

To make them permanent (append to ~/.bashrc):

  echo 'export PYTHONHOME=/mingw64' >> ~/.bashrc
  echo 'export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders' >> ~/.bashrc

You can now run:

  ./build.sh${BUILD_ARGS}         # build
  cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer.exe   # run
EOF
