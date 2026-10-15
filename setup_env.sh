#!/bin/bash
# ============================================================================
# setup_env.sh - one-shot setup of the CH32H417 U3LogicAnalyzer build and runtime environment
# Run this inside an MSYS2 MINGW64 shell
# ============================================================================
set -e

if [[ "$MSYSTEM" != "MINGW64" ]]; then
    echo "[error] Please run this script in an MSYS2 MINGW64 shell (Start menu -> MSYS2 MINGW64)"
    exit 1
fi

echo ">>> Installing the build environment..."

pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,qt6-static,python}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

cat <<EOF

============================================
 Environment setup complete!
============================================

Runtime environment variables (set them in every new shell):

  export PYTHONHOME=/mingw64
  export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders

To make them permanent (append to ~/.bashrc):

  echo 'export PYTHONHOME=/mingw64' >> ~/.bashrc
  echo 'export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders' >> ~/.bashrc

You can now run:

  ./build.sh         # build
  cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer.exe   # run
EOF
