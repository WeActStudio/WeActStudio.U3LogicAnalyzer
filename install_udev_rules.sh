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
 udev rules installed
============================================

Rules cover:
  1a86:5537  runtime interfaces (command / logic data / ADC data)
  1a86:fe17  HID interface used for the IAP firmware upgrade (incl. hidraw)

Verify with:

  lsusb | grep 1a86          # the device should be listed (missing? check passthrough)
  ls -l /dev/bus/usb/*/*     # its node should be root:plugdev and writable
  udevadm verify /etc/udev/rules.d/99-weact-ch32h417.rules

If the device was already plugged in, unplug and replug it to apply the rules.
EOF
