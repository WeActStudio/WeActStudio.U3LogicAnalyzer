#!/bin/bash
# ============================================================================
# setup_env_linux.sh - one-shot setup of the CH32H417 U3LogicAnalyzer build and
# runtime environment on Debian/Ubuntu Linux
# Run this in a normal shell: ./setup_env_linux.sh
#
# Usage:
#   ./setup_env_linux.sh          # Qt 6 (default)
#   ./setup_env_linux.sh --qt5    # Qt 5
#   ./setup_env_linux.sh --qt6    # Qt 6
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

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
elif command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    echo "[error] Not root and sudo not available; run as root or install sudo"
    exit 1
fi

if [ "${QT_MAJOR}" = "5" ]; then
    QT_PACKAGES="qtbase5-dev qttools5-dev qttools5-dev-tools libqt5svg5-dev"
    BUILD_ARGS=" --qt5"
else
    QT_PACKAGES="qt6-base-dev qt6-base-dev-tools qt6-svg-dev \
    qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools"
    BUILD_ARGS=""
fi

echo ">>> Installing build dependencies (build-essential cmake ninja-build pkg-config)..."
echo ">>> and libraries (glibmm libusb hidapi libzip boost qt${QT_MAJOR} python3)..."
$SUDO apt update
$SUDO apt install -y \
    build-essential cmake ninja-build pkg-config \
    libglib2.0-dev libglibmm-2.4-dev libsigc++-2.0-dev \
    libusb-1.0-0-dev libzip-dev libhidapi-dev \
    libboost-filesystem-dev libboost-serialization-dev libboost-system-dev \
    ${QT_PACKAGES} \
    python3-dev gdb

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

cat <<EOF

============================================
 Environment setup complete! (Qt ${QT_MAJOR})
============================================

Runtime environment variables (set them in every new shell):

  export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders

(Linux uses the system Python, PYTHONHOME is not needed)

To make them permanent (append to ~/.bashrc):

  echo 'export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders' >> ~/.bashrc

You can now run:

  ./build.sh${BUILD_ARGS}         # build
  ./start_pv.sh      # run (uses build_cmake/logicanalyzer_build/U3LogicAnalyzer)
EOF
