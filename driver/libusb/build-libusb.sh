#!/bin/sh
#
# Rebuild the vendored Windows libusb (see README.md).
#
#   ./build-libusb.sh [source-tree] [output-dir]
#
# The source tree defaults to $LIBUSB_SRC, then to src/ next to this script.
# It is the unpacked archive named in README.md; raw_io.patch from this
# directory is applied automatically if it is not there yet.
#
# The built DLL lands in the output directory (default:
# <repo>/build_cmake/libusb_build) and is copied over the one in this directory.
#
set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
REPO_DIR=$(cd "$SCRIPT_DIR/../.." && pwd)

SRC=${1:-${LIBUSB_SRC:-$SCRIPT_DIR/src/libusb-event-abstraction-v4}}
OUT=${2:-$REPO_DIR/build_cmake/libusb_build}

if [ ! -f "$SRC/libusb/core.c" ]; then
    echo "libusb source tree not found: $SRC" >&2
    echo "" >&2
    echo "Fetch and unpack it first (must match the sha256 in README.md):" >&2
    echo "  curl -L -o /tmp/libusb-event-abstraction-v4.zip \\" >&2
    echo "    https://github.com/uwehermann/libusb/archive/event-abstraction-v4.zip" >&2
    echo "  mkdir -p '$SCRIPT_DIR/src' && unzip -d '$SCRIPT_DIR/src' /tmp/libusb-event-abstraction-v4.zip" >&2
    echo "Or pass the directory as the first argument / set LIBUSB_SRC." >&2
    exit 1
fi

# Autotools is not usable here (a process costs about half a second on this
# machine, so configure never finishes), hence the hand written config.h and
# the direct compile below.
export PATH="/d/msys64/mingw64/bin:$PATH"
mkdir -p "$OUT"
cd "$OUT"

cat > config.h << 'CFG'
#ifndef LIBUSB_CONFIG_H
#define LIBUSB_CONFIG_H
#define DEFAULT_VISIBILITY /**/
#define ENABLE_LOGGING 1
#define POLL_NFDS_TYPE unsigned int
#define OS_WINDOWS 1
#define HAVE_SIGNAL_H 1
#define HAVE_SYS_TYPES_H 1
#endif
CFG

if grep -q "failed to enable RAW_IO" "$SRC/libusb/os/windows_usb.c"; then
    echo "raw_io.patch already applied"
else
    echo "Applying raw_io.patch..."
    patch -p1 -d "$SRC" --forward < "$SCRIPT_DIR/raw_io.patch"
fi

gcc -O2 -w -std=gnu11 -c -DNDEBUG -D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS \
    -DHAVE_GETTIMEOFDAY=1 -DHAVE_SYS_TIME_H=1 -I. -I"$SRC/libusb" -I"$SRC/libusb/os" \
    "$SRC/libusb/core.c" "$SRC/libusb/descriptor.c" "$SRC/libusb/hotplug.c" \
    "$SRC/libusb/io.c" "$SRC/libusb/strerror.c" "$SRC/libusb/sync.c" \
    "$SRC/libusb/os/events_windows.c" "$SRC/libusb/os/threads_windows.c" \
    "$SRC/libusb/os/windows_usb.c"

gcc -shared -o libusb-1.0.dll ./*.o "$SRC/libusb/libusb-1.0.def" \
    -Wl,--out-implib,libusb-1.0.dll.a

cp -f libusb-1.0.dll "$SCRIPT_DIR/libusb-1.0.dll"
echo "Built $OUT/libusb-1.0.dll"
echo "Copied it to $SCRIPT_DIR/libusb-1.0.dll"
