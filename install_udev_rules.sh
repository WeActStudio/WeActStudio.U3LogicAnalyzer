#!/bin/bash
# ============================================================================
# install_udev_rules.sh - install the udev rules so that the logic analyzers
# can be opened without root:
#
#   99-weact-ch32h417.rules   WeAct Studio CH32H417
#   99-fx2lafw.rules          FX2 based boards (sigrok fx2lafw)
#
#   ./install_udev_rules.sh          # install
#   ./install_udev_rules.sh --remove # remove again
# ============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RULES_FILES="99-weact-ch32h417.rules 99-fx2lafw.rules"
RULES_DIR="/etc/udev/rules.d"

case "${1:-}" in
    --remove|--uninstall)
        if [ "$(id -u)" -eq 0 ]; then
            SUDO=""
        else
            SUDO="sudo"
        fi
        removed=0
        for rules in $RULES_FILES; do
            if [ -f "${RULES_DIR}/${rules}" ]; then
                $SUDO rm -f "${RULES_DIR}/${rules}"
                echo "Removed ${RULES_DIR}/${rules}"
                removed=1
            fi
        done
        if [ "$removed" -eq 1 ]; then
            $SUDO udevadm control --reload-rules 2>/dev/null || true
            $SUDO udevadm trigger --subsystem-match=usb 2>/dev/null || true
        else
            echo "No rules installed in ${RULES_DIR}; nothing to do"
        fi
        exit 0
        ;;
    "") ;;
    *)
        echo "Usage: $0 [--remove]"
        exit 1
        ;;
esac

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
elif command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    echo "[error] Not root and sudo not available; run this script as root"
    exit 1
fi

for rules in $RULES_FILES; do
    SRC="${SCRIPT_DIR}/${rules}"
    if [ ! -f "$SRC" ]; then
        echo "[error] ${SRC} not found"
        exit 1
    fi
    echo ">>> Installing ${SRC} -> ${RULES_DIR}/"
    $SUDO cp "$SRC" "${RULES_DIR}/"
    $SUDO chmod 0644 "${RULES_DIR}/${rules}"
done

$SUDO udevadm control --reload-rules 2>/dev/null || true
$SUDO udevadm trigger --subsystem-match=usb 2>/dev/null || true

cat <<'EOF'

============================================
 udev rules installed
============================================

Rules cover:
  1a86:5537  CH32H417 runtime interfaces (command / logic data / ADC data)
  1a86:fe17  CH32H417 HID interface used for the IAP firmware upgrade (incl. hidraw)
  1d50:608c  FX2 based logic analyzer, 8 channel
  1d50:608d  FX2 based logic analyzer, 16 channel

Verify with:

  lsusb | grep -E '1a86|1d50'   # devices should be listed (missing? check passthrough)
  ls -l /dev/bus/usb/*/*        # their nodes should be root:plugdev and writable
  udevadm verify /etc/udev/rules.d/99-weact-ch32h417.rules
  udevadm verify /etc/udev/rules.d/99-fx2lafw.rules

If a device was already plugged in, unplug and replug it to apply the rules.
EOF
