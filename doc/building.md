# Building

[中文](building.zh_CN.md) · [Back to README](../README.md)

The three libraries (libsigrok, libsigrokcxx, libsigrokdecode) and the U3LogicAnalyzer
application are all built by CMake through one script. Using the application is covered in
[Usage](usage.md).

## Requirements

| Platform | State | Dependencies |
|---|---|---|
| Windows 10/11 + MSYS2 MINGW64 | supported | `./setup_env.sh` |
| Linux (Debian / Ubuntu) | supported | `./setup_env_linux.sh` (apt) |
| macOS | **not ported** | none (`setup_env_macos.sh` is kept but no longer maintained) |

> `setup_env_macos.sh` and the macOS branch of `build.sh` are left over from an earlier attempt and
> were never compiled or run on macOS. The libusb back end needs nothing Windows specific, so it
> could work in principle, but macOS is not supported for now.

## Windows (MSYS2 MINGW64)

The **MSYS2 MINGW64** shell is required (Start menu → MSYS2 MINGW64); cmd and PowerShell cannot
build this.

```bash
# one-shot setup (Qt 6 by default)
./setup_env.sh

# or by hand
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,qt6-static,python}
```

`setup_env.sh` takes `--qt5` / `--qt6` (Qt 6 by default, matching `build.sh`):

| Option | Static Qt package |
|---|---|
| `--qt6` (default) | `mingw-w64-x86_64-qt6-static` |
| `--qt5` | `mingw-w64-x86_64-qt5-static` |

> `qtN-static` drops out of the mingw64 repository while a Qt release is rebuilt, so the script
> installs it on its own: missing that one package does not abort the rest of the setup, and a
> note is printed instead.

## Linux (Debian / Ubuntu)

```bash
./setup_env_linux.sh            # Qt 6 by default
./setup_env_linux.sh --qt5      # Qt 5 instead
sudo ./install_udev_rules.sh    # open the device without root (once)
```

apt installs `qt6-base-dev` / `qt6-base-dev-tools` / `qt6-svg-dev` / `qt6-tools-dev` /
`qt6-l10n-tools` (`qtbase5-dev` and friends with `--qt5`), together with the development packages
of glibmm, libusb, hidapi, libzip and boost. The system Qt is used; no static build here.

## Building

```bash
./build.sh                 # incremental build (Qt 6 by default)
./build.sh --qt5           # build against Qt 5
./build.sh --clean         # throw the build directory away first
./build.sh --package       # build and fill dist/U3LogicAnalyzer/
./build.sh --prefix <dir>  # install prefix (default: ./install)
```

Results:

| Path | What it is |
|---|---|
| `build_cmake/logicanalyzer_build/U3LogicAnalyzer` (`.exe` on Windows) | the application |
| `install/` | libsigrok + libsigrokcxx + libsigrokdecode and the 132 decoders |

> `U3_BUILD_DIR` moves the build tree to another disk, e.g. `U3_BUILD_DIR=/i/la_build ./build.sh`;
> a debug build uses `${U3_BUILD_DIR}_debug`.
>
> Delete the build directory when switching between Qt majors: the cached Qt paths do not follow.

## Running

The build result needs a few environment variables, so `./start_pv.sh` is the easy way. By hand:

```bash
# Windows (MSYS2 MINGW64)
export PYTHONHOME=/mingw64
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer.exe

# Linux (system Python, no PYTHONHOME)
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
export LD_LIBRARY_PATH=install/lib:$LD_LIBRARY_PATH
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer
```

> Put these into `~/.bashrc` to keep them. An installed deb, archive or AppImage does not need any
> of them.

## Packaging

### Windows

```bash
./build.sh --package       # portable: dist/U3LogicAnalyzer/
./package_installer.sh     # installer: dist/U3LogicAnalyzer_Setup.exe
```

The portable directory holds the executable, every DLL it needs, `libsigrok*.dll`, `decoders/`,
a portable Python and the driver directory, and can be copied anywhere. The NSIS installer adds a
start menu entry and a desktop shortcut and can be removed from the control panel.

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

A Qt 6 build carries a `-qt6` tag in the file names, so the packages of the two Qt versions do not
overwrite each other. The archive and the deb install the application into `/opt/U3LogicAnalyzer`
(as root) or `~/.local/opt/U3LogicAnalyzer` (as a normal user), together with the menu entry, a
hicolor icon tree and AppStream metadata - without that metadata a software centre reads nothing
about the app and shows a placeholder tile.

> A deb installed from a file does not come from a software source, so a software centre does not
> list it as installed and cannot remove it either. Use `apt` / `dpkg` (or Synaptic).

**Notes on the AppImage**

- Ubuntu 24.04 and later ship no `libfuse2`, so starting it directly fails with
  `dlopen(): error loading libfuse.so.2`. Use `<file>.run.sh` (recommended: no root, no FUSE),
  `--appimage-extract-and-run`, or install `libfuse2`.
- Dropping `uruntime-<arch>` into `dist/` before packaging produces an AppImage that needs no FUSE.
- On the first run `AppRun` registers the launcher and the icons under `~/.local/share`, which is
  what makes the shell show the application by name and icon. Set `U3LA_NO_INTEGRATION=1` to skip it.

## udev rules (Linux, without root)

By default the `/dev/bus/usb/*` nodes belong to root, so libusb fails with `LIBUSB_ERROR_ACCESS`:
`lsusb` lists the device while the application cannot find it.

```bash
sudo ./install_udev_rules.sh            # install
sudo ./install_udev_rules.sh --remove   # remove again
```

| Rule file | Devices |
|---|---|
| `99-weact-ch32h417.rules` | `1a86:5537` (runtime interfaces) and `1a86:fe17` (the HID interface used by the IAP update) |
| `99-fx2lafw.rules` | `1d50:608c` / `1d50:608d`, the FX2 based 8- and 16-channel boards |

Unplug and replug the device afterwards. Installing the deb puts the rules into
`/etc/udev/rules.d/` as part of the package.

## FX2 firmware

`fx2lafw` drives the analyzers built on a Cypress FX2 that run the open source fx2lafw firmware
(Saleae Logic and its clones, the USBee AX/DX family, the sigrok fx2grok boards). An FX2 has no
flash of its own: the firmware is downloaded into its RAM by the driver and is gone as soon as the
board loses power, which is why the files have to be there every time.

The files are in `firmware/` (`fx2lafw-*.fw`) and the build installs them to
`share/sigrok-firmware/`. The application also points `SIGROK_FIRMWARE_DIR` at its own copy; a
value coming from the environment wins.

## Adding a driver of your own

1. Create `your-device/` under `libsigrok/src/hardware/`
2. Write `api.c` (required), `protocol.c`, `protocol.h`
3. Add the sources to `LIBSIGROK_SOURCES` in `libsigrok/CMakeLists.txt`
4. Add `#define HAVE_HW_YOUR_DEVICE 1` to `libsigrok/config.h.cmake`
5. Add `set(HAVE_HW_YOUR_DEVICE 1)` to `libsigrok/CMakeLists.txt`
6. Run `./build.sh` again

> The two drivers that are there share the device protocol but no translation unit: every global
> symbol carries a prefix such as `ch32h417_libusb_`, so that building both cannot end up with
> duplicate definitions. Re-run the CMake configure after adding sources (the file list is a GLOB).

## Build problems

**`collect2: ld returned 1`, the exe cannot be written**
- A previous instance is still running and holds it. Close the application
  (Windows: `taskkill /F /IM U3LogicAnalyzer.exe`) and build again.

**libsigrok changed but the application behaves the same**
- libsigrok installs into `install/` and the application links against that copy. Install again and
  copy `install/bin/libsigrok*.dll` into `build_cmake/logicanalyzer_build/` (Windows).

**A pile of odd errors after switching Qt major**
- The CMake cache still points at the old Qt. Delete `build_cmake/` (or `$U3_BUILD_DIR`) and start over.

**An incremental build after a dependency upgrade only links, it does not recompile**
- pacman stores headers with the timestamps from the package, so a new header can be "older" than
  the existing `.obj` and ninja sees no reason to rebuild. Touch the upgraded headers
  (`glib.h`, `glibconfig.h`, `zip.h`, `boost/version.hpp`, `Python.h`) or use `--clean`.

**Python fails with `Failed to import encodings module`**
- `PYTHONHOME` is missing or wrong. On Windows it must be `/mingw64`, and no outside `PYTHONPATH`
  should be set.
