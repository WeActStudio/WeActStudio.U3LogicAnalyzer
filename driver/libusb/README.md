# Vendored libusb for Windows

`libusb-1.0.dll` here replaces the one MSYS2 ships. The stock library cannot
stream an FX2 based analyzer above 12 MHz: it only enables the WinUSB
`RAW_IO` pipe policy when a transfer is larger than the pipe's raw-I/O limit,
and without that policy the device stops sending in the middle of an
acquisition (every remaining transfer then times out).

`build.sh` copies this file over the system DLL, both into the build directory
and into the packaged distribution.

## Where it comes from

Exactly the library the sigrok project builds for its own Windows releases:

- source: https://github.com/uwehermann/libusb/archive/event-abstraction-v4.zip
- sha256: `58fee7f3f05fda209d14c55763df36ab86028bd9ab82c9bb74f1d5ab3208bcfd`
- version: `1.0.20-rc3-event-abstraction-v4` (the branch the sigrok wiki calls
  "libusb (special branch)"; it is what provides pollable events on Windows)

Both facts come from `sigrokproject/sigrok-build`, file
`docker/mxe_fixes.patch`: its `src/libusb1.mk` hunk repoints the MXE package at
that archive (with that checksum), and `src/libusb1-1-fixes.patch` adds the
`RAW_IO` change. sigrok ships the result under LGPL-2.1-or-later.

## The required patch

`raw_io.patch` turns on the WinUSB `RAW_IO` pipe policy unconditionally in
`winusbx_configure_endpoints()`. It lands in `libusb/os/windows_usb.c` at the
same offset the sigrok patch uses.

## Rebuilding

    ./build-libusb.sh [source-tree] [output-dir]

Unpack the archive next to this file first:

    curl -L -o /tmp/libusb-event-abstraction-v4.zip \
      https://github.com/uwehermann/libusb/archive/event-abstraction-v4.zip
    mkdir -p src && unzip -d src /tmp/libusb-event-abstraction-v4.zip

The script then applies `raw_io.patch` to the pristine tree, compiles it
directly (autotools is not usable: a process costs about half a second on
this machine, so `configure` never finishes), writes the DLL into
`build_cmake/libusb_build/` and copies it over the one here. The source tree
is not kept in the repository - fetch it again when needed.

## Measured effect (sigrok FX2 LA 8ch, USB 2.0 high speed)

| sample rate | stock libusb 1.0.28 | this library |
|---|---|---|
| 12 MHz | works | works |
| 16 MHz | stops after 1.44 M samples | **full 10 M samples** |
| 24 MHz | stops after 0.96 M samples | **full 48 M samples (2 s)** |
| 48 MHz | 9216 samples | 5632 samples - still impossible, 48 MB/s exceeds what USB 2.0 high speed can carry |
