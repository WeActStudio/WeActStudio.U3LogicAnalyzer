#!/bin/bash
# ============================================================================
# setup_env_linux.sh - one-shot setup of the CH32H417 LogicAnalyzer build and
# runtime environment on Debian/Ubuntu Linux
# Run this in a normal shell: ./setup_env_linux.sh
# ============================================================================
set -e

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
elif command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    echo "[error] Not root and sudo not available; run as root or install sudo"
    exit 1
fi

echo ">>> Installing build dependencies (build-essential cmake ninja-build pkg-config)..."
echo ">>> and libraries (glibmm libusb hidapi libzip boost qt5 python3)..."
$SUDO apt update
$SUDO apt install -y \
    build-essential cmake ninja-build pkg-config \
    libglib2.0-dev libglibmm-2.4-dev libsigc++-2.0-dev \
    libusb-1.0-0-dev libzip-dev libhidapi-dev \
    libboost-filesystem-dev libboost-serialization-dev libboost-system-dev \
    qtbase5-dev qttools5-dev qttools5-dev-tools libqt5svg5-dev \
    python3-dev gdb

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

cat <<EOF

============================================
 环境部署完成！
============================================

运行时环境变量（每次打开新终端需设置）：

  export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders

（Linux 使用系统 Python，无需设置 PYTHONHOME）

永久生效（写入 ~/.bashrc）：

  echo 'export SIGROKDECODE_DIR=${SCRIPT_DIR}/install/share/libsigrokdecode/decoders' >> ~/.bashrc

现在可以运行:

  ./build.sh         # build
  ./start_pv.sh      # run (uses build_cmake/logicanalyzer_build/LogicAnalyzer)
EOF
