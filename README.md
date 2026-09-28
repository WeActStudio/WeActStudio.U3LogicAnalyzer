# CH32H417 Logic Analyzer - PC software

[中文文档](README_zh_CN.md)

This tree builds **U3LogicAnalyzer**, the host application for the
WeAct Studio CH32H417 logic analyzer. It is based on sigrok PulseView, ships 132 protocol
decoders (I²C, SPI, UART, ...) and keeps only the two hardware drivers that
belong to this device. Everything is built by CMake through one script.

> **Origin**: this software is a modification of the project of **Q2H2**. The
> CH375 driver (`libsigrok/src/hardware/wch-ch32h417/`), the `demo` driver and
> the parts of libsigrok / libsigrokdecode that were left alone still carry
> their `Copyright (C) 2026 Q2H2` notice, which this project did not touch.
> The libusb driver added here (`wch-ch32h417-libusb/`) is copyright
> WeAct Studio.

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

# macOS (Homebrew)
./setup_env_macos.sh
./build.sh
./start_pv.sh
```

`start_pv.sh` sets `PYTHONHOME`, `SIGROKDECODE_DIR` and `LD_LIBRARY_PATH` for
you and starts `build_cmake/logicanalyzer_build/U3LogicAnalyzer`.

## Requirements

| Platform | How the dependencies are installed |
|---|---|
| Windows 10/11 + MSYS2 MINGW64 | `./setup_env.sh` |
| Linux (Debian / Ubuntu) | `./setup_env_linux.sh` (apt) |
| macOS | `./setup_env_macos.sh` (Homebrew) |

**Windows (MSYS2 MINGW64)**

```bash
# one-shot setup
./setup_env.sh

# or by hand:
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,qt5-static,python}
```

> On Windows the **MSYS2 MINGW64** shell is required (Start menu -> MSYS2 MINGW64).
> A plain shell is fine on macOS and Linux; `build.sh` detects the platform itself.
> The Qt version is picked with `--qt5` / `--qt6`: on Windows that means
> `qt5-static` / `qt6-static`, on Linux the Qt of the system is used.

## Device access and driver back ends

| | Windows | Linux | macOS |
|---|---|---|---|
| `wch-ch32h417` (CH375DLL) | yes, the default binding | not built | not built |
| `wch-ch32h417-libusb` (libusb) | yes, after rebinding to WinUSB with Zadig | yes | yes |
| Hot plug | CH375 callback + libusb polling | libusb polling | libusb polling |
| Firmware upgrade (IAP) | either back end | libusb only | libusb only |

On Windows the device is bound to `CH375WDM.sys` by default, which is what the
`wch-ch32h417` driver talks to; using the libusb back end there needs a WinUSB
rebind with Zadig first. Off Windows only the libusb back end exists - the
sources of the CH375 driver are dropped while CMake configures
(`list(REMOVE_ITEM ...)` under `if(NOT WIN32)` in `libsigrok/CMakeLists.txt`).

**Using the device without root on Linux**: by default the `/dev/bus/usb/*`
nodes belong to root, so libusb fails with `LIBUSB_ERROR_ACCESS` - `lsusb`
lists the device while the application cannot find it.

```bash
sudo ./install_udev_rules.sh            # install
sudo ./install_udev_rules.sh --remove   # remove again
```

The rules file is `99-weact-ch32h417.rules` and covers two USB IDs:

- `1a86:5537` - the runtime interfaces (commands, logic data, ADC data)
- `1a86:fe17` - the HID interface used by the IAP firmware upgrade (its hidraw
  node is covered as well)

Unplug and replug the device afterwards. Installing the deb puts the rules into
`/etc/udev/rules.d/` as part of the package, so nothing has to be done by hand.

## Building

```bash
./build.sh                 # incremental build (Qt 5 by default)
./build.sh --qt6           # build against Qt 6
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

# Linux / macOS (system Python, no PYTHONHOME)
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
export LD_LIBRARY_PATH=install/lib:$LD_LIBRARY_PATH
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer
```

> Put these into `~/.bashrc` to keep them. An installed deb, archive or
> AppImage does not need any of them.

## Layout

```
.
├── setup_env.sh                 # environment setup, Windows
├── setup_env_linux.sh           # environment setup, Debian/Ubuntu
├── setup_env_macos.sh           # environment setup, macOS
├── build.sh                     # build + portable package
├── package_installer.sh         # installer (NSIS / deb + tar.gz + AppImage)
├── install_udev_rules.sh        # installs the udev rules on Linux
├── 99-weact-ch32h417.rules      # rules for using the device without root
├── start_pv.sh                  # start with the environment set (-l 5)
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
./package_installer.sh              # build and package (Qt 5)
./package_installer.sh --qt6        # the same, built against Qt 6
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
