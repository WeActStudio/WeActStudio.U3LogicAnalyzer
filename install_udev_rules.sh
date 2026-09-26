#!/bin/bash
# ============================================================================
# install_udev_rules.sh - install 99-weact-ch32h417.rules so that the
# WeAct CH32H417 Logic Analyzer can be opened without root.
#
#   ./install_udev_rules.sh          # install
#   ./install_udev_rules.sh --remove # remove again
# ============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC="${SCRIPT_DIR}/99-weact-ch32h417.rules"
DST="/etc/udev/rules.d/99-weact-ch32h417.rules"

case "${1:-}" in
    --remove|--uninstall)
        if [ "$(id -u)" -eq 0 ]; then
            SUDO=""
        else
            SUDO="sudo"
        fi
        if [ -f "$DST" ]; then
            $SUDO rm -f "$DST"
            $SUDO udevadm control --reload-rules 2>/dev/null || true
            $SUDO udevadm trigger --subsystem-match=usb 2>/dev/null || true
            echo "Removed ${DST}"
        else
            echo "${DST} is not installed; nothing to do"
        fi
        exit 0
        ;;
    "") ;;
    *)
        echo "Usage: $0 [--remove]"
        exit 1
        ;;
esac

if [ ! -f "$SRC" ]; then
    echo "[error] ${SRC} not found"
    exit 1
fi

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
elif command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    echo "[error] Not root and sudo not available; run this script as root"
    exit 1
fi

echo ">>> Installing ${SRC} -> ${DST}"
$SUDO cp "$SRC" "$DST"
$SUDO chmod 0644 "$DST"
$SUDO udevadm control --reload-rules 2>/dev/null || true
$SUDO udevadm trigger --subsystem-match=usb 2>/dev/null || true

cat <<'EOF'

============================================
 udev 规则安装完成
============================================

规则覆盖：
  1a86:5537  运行态接口（命令 / 逻辑数据 / ADC 数据）
  1a86:fe17  IAP 固件升级用的 HID 接口（含 hidraw 节点）

验证：

  lsusb | grep 1a86          # 设备应可见（未接? 检查 VMware 透传）
  ls -l /dev/bus/usb/*/*     # 对应节点应为 root:plugdev 且可写
  udevadm verify /etc/udev/rules.d/99-weact-ch32h417.rules

若安装时设备已经插着，拔插一次以触发规则。
EOF
