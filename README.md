# CH32H417 Logic Analyzer - PC software

[中文文档](README_zh_CN.md)

This tree builds **U3LogicAnalyzer**, the host application for the
WeAct Studio CH32H417 logic analyzer. It is based on sigrok PulseView, ships 132 protocol
decoders (I²C, SPI, UART, ...) and keeps the hardware drivers down to the two
that belong to this device (PulseView's `demo` driver is kept as well, along
with the `fx2lafw` driver for FX2 based analyzers). Everything is built by CMake
through one script.

> **Origin**: this software is a modification of the project of **Q2H2**. The
> CH375 driver (`libsigrok/src/hardware/wch-ch32h417/`), the `demo` driver and
> the parts of libsigrok / libsigrokdecode that were left alone still carry
> their `Copyright (C) 2026 Q2H2` notice, which this project did not touch.
> The libusb driver added here (`wch-ch32h417-libusb/`) is copyright
> WeAct Studio. The `fx2lafw` driver (`libsigrok/src/hardware/fx2lafw/`) is
> upstream sigrok code and keeps the notice of its authors, Bert Vermeulen and
> Joel Holdsworth.

## Quick start

```bash
# Windows (MSYS2 MINGW64)
./setup_env.sh
./build.sh
./start_pv.sh

# Linux (Debian / Ubuntu)
./setup_env_linux.sh
sudo ./install_udev_rules.sh      # open the device without root (once)
./build.sh
./start_pv.sh

```

`start_pv.sh` sets `PYTHONHOME`, `SIGROKDECODE_DIR` and `LD_LIBRARY_PATH` for
you and starts `build_cmake/logicanalyzer_build/U3LogicAnalyzer`.

## Requirements

| Platform | State | How the dependencies are installed |
|---|---|---|
| Windows 10/11 + MSYS2 MINGW64 | supported | `./setup_env.sh` |
| Linux (Debian / Ubuntu) | supported | `./setup_env_linux.sh` (apt) |
| macOS | **not ported** | none (`setup_env_macos.sh` is kept but no longer maintained) |

> **macOS is not ported.** `setup_env_macos.sh` and the macOS branch of
> `build.sh` are left over from an earlier attempt and were never compiled or
> run on macOS. The libusb back end needs nothing Windows specific, so it
> could work in principle, but macOS is **not supported** for now and any
> dependency or build problem there is up to you to sort out.

**Windows (MSYS2 MINGW64)**

```bash
# one-shot setup
./setup_env.sh

# or by hand:
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,qt6-static,python}
```

> On Windows the **MSYS2 MINGW64** shell is required (Start menu -> MSYS2 MINGW64).
> A plain shell is fine on Linux; `build.sh` detects the platform itself
> (macOS is not ported).
> Qt 6 is the default and `--qt5` falls back to Qt 5: on Windows that
> means `qt6-static` / `qt5-static`, on Linux the Qt of the system is used.

## Device access and driver back ends

| | Windows | Linux | macOS |
|---|---|---|---|
| Overall | supported | supported | not ported |
| `wch-ch32h417` (CH375DLL) | yes, the default binding | not built | not built |
| `wch-ch32h417-libusb` (libusb) | yes, after rebinding to WinUSB with Zadig | yes | unverified |
| `fx2lafw` (libusb, FX2 based analyzers) | yes, after rebinding to WinUSB with Zadig | yes | unverified |
| Hot plug | CH375 callback + libusb polling | libusb polling | unverified |
| Firmware upgrade (IAP) | either back end | libusb only | unverified |

On Windows the device is bound to `CH375WDM.sys` by default, which is what the
`wch-ch32h417` driver talks to; using the libusb back end there needs a WinUSB
rebind with Zadig first. Off Windows only the libusb back end exists - the
sources of the CH375 driver are dropped while CMake configures
(`list(REMOVE_ITEM ...)` under `if(NOT WIN32)` in `libsigrok/CMakeLists.txt`).
Of the two, Linux has been built and run; macOS has not been ported.

**fx2lafw** is the driver for the analyzers built on a Cypress FX2 that run the
open source fx2lafw firmware: Saleae Logic and its clones, the USBee AX/DX
family, the sigrok fx2grok boards. It is a libusb back end like the one above,
so on Windows the board has to be bound to WinUSB with Zadig as well.

The FX2 is a USB 2.0 high speed part, and the sample rates the driver offers (up
to 24 MHz with 8 channels, 12 MHz with 16) are the ones its bulk endpoint keeps
up with on a high speed link. The driver reports the link the board ended up on
when it opens it, so a board that only runs at full speed - a slow hub, or a
cable that carries just the four wires - says so instead of quietly returning
samples that are not there.

An FX2 has no flash of its own, so the firmware is not kept in the board: it is
downloaded into its RAM by the driver, at the latest when the board is scanned,
and the board then re-enumerates with the `fx2lafw` ID that the driver waits for
and opens. It is gone again as soon as the board loses power, which is why the
firmware files have to be there every time - an FX2 based analyzer cannot be
used at all without them.

The files are in the `firmware/` directory of this repository - the
`fx2lafw-*.fw` of the sigrok-firmware-fx2lafw package - and the build installs
them to `share/sigrok-firmware/` of the install tree. That is one of the
directories libsigrok looks in, and the application points `SIGROK_FIRMWARE_DIR`
at its own copy as well, the way it does for the decoders; a value coming from
the environment wins. A board that is already running the firmware - one that a
previous scan programmed, in the same power cycle - is recognised as
`sigrok fx2lafw` and opened without another download.

**Using the device without root on Linux**: by default the `/dev/bus/usb/*`
nodes belong to root, so libusb fails with `LIBUSB_ERROR_ACCESS` - `lsusb`
lists the device while the application cannot find it.

```bash
sudo ./install_udev_rules.sh            # install
sudo ./install_udev_rules.sh --remove   # remove again
```

The rules come in two files, one per analyzer family:

- `99-weact-ch32h417.rules` - `1a86:5537` (the runtime interfaces: commands,
  logic data, ADC data) and `1a86:fe17` (the HID interface used by the IAP
  firmware upgrade, its hidraw node included)
- `99-fx2lafw.rules` - `1d50:608c` and `1d50:608d`, the FX2 based 8- and
  16-channel boards

Unplug and replug the device afterwards. Installing the deb puts the rules into
`/etc/udev/rules.d/` as part of the package, so nothing has to be done by hand.

## Building

```bash
./build.sh                 # incremental build (Qt 6 by default)
./build.sh --qt5           # build against Qt 5
./build.sh --clean         # throw the build directory away first
./build.sh --package       # build and fill dist/U3LogicAnalyzer/
./build.sh --prefix <dir>  # install prefix (default: ./install)
```

Results:

- `build_cmake/logicanalyzer_build/U3LogicAnalyzer` (`.exe` on Windows) - the application
- `install/` - libsigrok + libsigrokcxx + libsigrokdecode and the 132 decoders

## Running

The build result needs a few environment variables, so `./start_pv.sh` is the
easy way. By hand:

```bash
# Windows (MSYS2 MINGW64)
export PYTHONHOME=/mingw64
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer.exe

# Linux (system Python, no PYTHONHOME; macOS is not ported)
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
export LD_LIBRARY_PATH=install/lib:$LD_LIBRARY_PATH
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer
```

> Put these into `~/.bashrc` to keep them. An installed deb, archive or
> AppImage does not need any of them.

## Using the application

### Opening a saved capture

A capture saved as a sigrok session file (`.sr`) opens from the file menu, or
can be handed to the application on the command line:

```bash
./U3LogicAnalyzer <file>.sr            # Windows: U3LogicAnalyzer.exe <file>.sr
./U3LogicAnalyzer -i <file>.sr         # the same through the option
```

What the file was captured with is what gets shown. A file holds samples only
for the channels it enables, so those are the ones given a signal, and the
channel popup lists them alone - no empty rows for channels the file has
nothing for. A capture shorter than the window opens fitted to it; for a
longer one, zoom out or use zoom-to-fit, and the view stops at the end of the
data. That limit is taken from the file itself and not from the sample count
of the tool bar, which belongs to the hardware.

### 8 or 16 logic channels

The device captures either 8 or 16 logic channels
(`SR_CONF_NUM_LOGIC_CHANNELS`). In the narrow 8 channel mode one sample is a
single byte instead of two, so the same capture takes half the memory - or
twice the samples for the memory a 16 channel capture would need. The number
is chosen in the tool bar and is sent to the device before a capture starts;
the sample rate it offers follows from it.

> A narrow capture is stored as one too: the probe count written into a `.sr`
> file follows the width of the samples, so an 8 channel file is not read
> back as though it held sixteen channels.

### Long captures and memory

A capture is held in memory in full, so a long one at a high sample rate can
use up everything the machine has. The application looks at what is left
while the data comes in and stops the capture with a message once less than
512 MB is available, rather than letting the system swap and the window stop
responding. What was captured is kept, the capture just ends early - fewer
channels, a lower sample rate or a shorter run is what fits it into memory.

## Layout

```
.
├── setup_env.sh                 # environment setup, Windows
├── setup_env_linux.sh           # environment setup, Debian/Ubuntu
├── setup_env_macos.sh           # environment setup, macOS (kept, not ported)
├── build.sh                     # build + portable package
├── build_pv.sh                  # build only, the way start_pv.sh runs it
├── build_pv_debug.sh            # the same with a debug configuration
├── package_installer.sh         # installer (NSIS / deb + tar.gz + AppImage)
├── install_udev_rules.sh        # installs the udev rules on Linux
├── 99-weact-ch32h417.rules      # udev rules for the CH32H417
├── 99-fx2lafw.rules             # udev rules for the FX2 based boards
├── start_pv.sh                  # start with the environment set (-l 5)
├── start_pv_debug.sh            # start the debug build (-l 5)
├── install/                     # libsigrok / libsigrokcxx / libsigrokdecode
├── libsigrok/
│   └── src/hardware/
│       ├── wch-ch32h417/        # CH375 back end (Windows only)
│       ├── wch-ch32h417-libusb/ # libusb back end (all platforms)
│       └── demo/
├── libsigrokdecode/decoders/    # the 132 protocol decoders
├── pulseview/                   # application (OUTPUT_NAME=U3LogicAnalyzer)
└── build_cmake/logicanalyzer_build/
```

## Packaging

### Windows

```bash
./build.sh --package       # portable: dist/U3LogicAnalyzer/
./package_installer.sh     # installer: dist/U3LogicAnalyzer_Setup.exe
```

The portable directory holds the executable, every DLL it needs,
`libsigrok*.dll`, `decoders/`, a portable Python and the driver directory, and
can be copied anywhere. The NSIS installer adds a start menu entry and a
desktop shortcut and can be removed from the control panel.

### Linux

```bash
./package_installer.sh              # build and package (Qt 6)
./package_installer.sh --qt5        # the same, built against Qt 5
./package_installer.sh --skip-build # package the existing dist/ as it is
./package_installer.sh --no-clean   # reuse the existing build directory
```

| Result | What it is |
|---|---|
| `U3LogicAnalyzer-<ver>-linux-<arch>.tar.gz` | archive with `install.sh` / `uninstall.sh` |
| `U3LogicAnalyzer-<ver>-linux-<arch>.deb` | `sudo apt install ./<file>.deb`, removed with `sudo apt remove u3logicanalyzer` |
| `U3LogicAnalyzer-<ver>-<arch>.AppImage` | runs without installing, comes with a `.run.sh` next to it |

A Qt 6 build carries a `-qt6` tag in the file names, so the packages of the two
Qt versions do not overwrite each other. The archive and the deb install the
application into `/opt/U3LogicAnalyzer` (as root) or
`~/.local/opt/U3LogicAnalyzer` (as a normal user), together with the menu entry
`U3LogicAnalyzer.desktop`, a hicolor icon tree and AppStream metadata
(`U3LogicAnalyzer.metainfo.xml`) - without that metadata a software centre
reads nothing about the app and shows a placeholder tile.

> A deb installed from a file does not come from a software source, so a
> software centre does not list it as installed and cannot remove it either.
> Use `apt` / `dpkg` (or Synaptic) instead.

**Notes on the AppImage**

- Ubuntu 24.04 and later ship no `libfuse2`, so starting it directly fails with
  `dlopen(): error loading libfuse.so.2`. Use `<file>.run.sh` (recommended: no
  root, no FUSE), `--appimage-extract-and-run`, or install `libfuse2`.
- Dropping `uruntime-<arch>` into `dist/` before packaging produces an AppImage
  that needs no FUSE at all.
- On the first run `AppRun` registers the launcher and the icons under
  `~/.local/share`. That is what makes the shell show the application by name
  and icon instead of falling back to the window title and a placeholder: the
  shell only watches the data directories it knew about when the session
  started, so exporting `XDG_DATA_DIRS` is not enough. Set
  `U3LA_NO_INTEGRATION=1` to skip it. Every step is best effort.

## Adding a driver of your own

1. Create `your-device/` under `libsigrok/src/hardware/`
2. Write `api.c` (required), `protocol.c`, `protocol.h`
3. Add the sources to `LIBSIGROK_SOURCES` in `libsigrok/CMakeLists.txt`
4. Add `#define HAVE_HW_YOUR_DEVICE 1` to `libsigrok/config.h.cmake`
5. Add `set(HAVE_HW_YOUR_DEVICE 1)` to `libsigrok/CMakeLists.txt`
6. Run `./build.sh` again

> The two drivers that are there share the device protocol but no translation
> unit: every global symbol carries a prefix such as `ch32h417_libusb_`, so
> that building both cannot end up with duplicate definitions.
