# Vendored libusb for Windows

`libusb-1.0.dll` here replaces the one MSYS2 ships, for two reasons:

- the MSYS2 build predates `libusb_endpoint_set_raw_io()`, which is the only way
  to get the WinUSB `RAW_IO` pipe policy;
- without that policy an FX2 based analyzer stops sending in the middle of a
  capture as soon as the host falls behind, and every later transfer times out.

## Where it comes from

The unmodified official binary of libusb v1.0.30, the `VS2025/MS64-hotplug`
build, which has the Windows hot-plug support compiled in. Taken from
`Doc/libusb-1.0.30.7z` of this project. Licence LGPL-2.1-or-later, same as
upstream. Nothing in it is patched.

The front end loads this same DLL for every libusb call it makes
(`pulseview/pv/usbapi.cpp` holds the entry points): the bus watcher, and the
HID enumeration behind the firmware-update action. Hot-plug events are what
tell a page that a board came or went, and the MSYS2 build has none compiled
in. Going through the DLL also keeps one libusb in the process - the one
libsigrok.dll links against - so a context made here is the one the drivers
use.

## RAW_IO belongs to the driver

The library stays stock: `fx2lafw/api.c` resolves `libusb_endpoint_set_raw_io()`
at run time and turns the policy on for the acquisition endpoint. The lookup is
by name on purpose - the library this links against does not have to export it,
and an older DLL then simply captures without the policy instead of failing.

`build.sh` copies this file over the system DLL, both into the build directory
and into the packaged distribution.

## Measured effect (sigrok FX2 LA 8ch, USB 2.0 high speed)

| sample rate | without RAW_IO | with the driver turning it on |
|---|---|---|
| 16 MHz | stops after 1.44 M samples | full 10 M samples |
| 24 MHz | stops after 240,640 samples | full 48 M samples (2 s) |
| 48 MHz | 9216 samples | impossible - 48 MB/s exceeds USB 2.0 high speed |
