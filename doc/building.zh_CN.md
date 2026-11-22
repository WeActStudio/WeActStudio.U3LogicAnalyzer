# 构建说明

[English](building.md) · [返回 README](../README_zh_CN.md)

本工程用 CMake 构建，三个顶层库（libsigrok / libsigrokcxx / libsigrokdecode）与主程序
U3LogicAnalyzer 都由 `build.sh` 一次编完。日常操作见 [使用说明](usage.zh_CN.md)。

## 环境要求

| 平台 | 状态 | 依赖安装 |
|---|---|---|
| Windows 10/11 + MSYS2 MINGW64 | 支持 | `./setup_env.sh` |
| Linux（Debian / Ubuntu） | 支持 | `./setup_env_linux.sh`（apt） |
| macOS 13 及以上 | 支持 | `./setup_env_macos.sh`（Homebrew，Qt 6） |

> `setup_env_macos.sh` 用 Homebrew 安装依赖，默认 Qt 6，可用 `--qt5` 切换。
> 已在 Apple Silicon 真机上完成采集验证。

## Windows（MSYS2 MINGW64）

必须用 **MSYS2 MINGW64** shell（开始菜单 → MSYS2 MINGW64），普通 cmd/PowerShell 编不了。

```bash
# 一键部署（默认 Qt 6）
./setup_env.sh

# 或者手动安装
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,qt6-static,python}
```

`setup_env.sh` 支持 `--qt5` / `--qt6`（默认 Qt 6，与 `build.sh` 一致）：

| 参数 | 静态 Qt 包 |
|---|---|
| `--qt6`（默认） | `mingw-w64-x86_64-qt6-static` |
| `--qt5` | `mingw-w64-x86_64-qt5-static` |

> `qtN-static` 在 Qt 发版重建期间会从 mingw64 仓库消失，所以脚本把它单独安装：
> 缺这一个不会中断其余环境的部署，装完会给出提示。

## Linux（Debian / Ubuntu）

```bash
./setup_env_linux.sh            # 默认 Qt 6
./setup_env_linux.sh --qt5      # 改用 Qt 5
sudo ./install_udev_rules.sh    # 免 root 打开设备（只需一次）
```

apt 装的是 `qt6-base-dev` / `qt6-base-dev-tools` / `qt6-svg-dev` / `qt6-tools-dev` /
`qt6-l10n-tools`（`--qt5` 时换成 `qtbase5-dev` 那一套），以及 glibmm、libusb、hidapi、libzip、
boost 的开发包。用系统 Qt，不编静态版。

## macOS

```bash
./setup_env_macos.sh            # Homebrew，默认 Qt 6
./setup_env_macos.sh --qt5      # 改用 Qt 5
```

Homebrew 会安装 `cmake`、`ninja`、`pkg-config`、`glib`、`glibmm@2.66`、`libsigc++@2`、
`libusb`、`hidapi`、`libzip`、`boost`、`qtbase`/`qtsvg`/`qttools`/`qttranslations`、
`python@3.12` 与 `gettext`。这里不使用 Homebrew 默认的 `glibmm`（2.88），因为 PulseView 依赖的
是 `glibmm-2.4` 系列接口。Qt 同样使用动态库，不编静态版。

最低系统版本由 `build.sh` 决定：默认 13.0，但会先读取所用 Qt 的
`QtCore.framework` 中 `LC_BUILD_VERSION minos`，**Qt 要求更高时以 Qt 为准**，再据此设置
`MACOSX_DEPLOYMENT_TARGET` 与 `Info.plist` 的 `LSMinimumSystemVersion`。

> **注意：** Homebrew 的 `qtbase` 目前为 6.12，`minos` 是 **14.4**。直接用它打包，生成的应用
> 最低系统版本会被抬到 14.4，macOS 13 的用户无法安装。要生成 13.0 的包，需改用按 13.0 编译的
> Qt（自行编译，或取自更旧的系统），并通过 `PV_QT_PREFIX` 指定：
>
> ```bash
> PV_QT_PREFIX=$HOME/Qt/6.11.1/macos ./build.sh
> ```
>
> 自行编译的 Qt 若基于更旧的 sysroot，可加 `PV_SYSROOT=<dir>`。打包后用以下两条命令核对：
>
> ```bash
> otool -l <app>/Contents/Frameworks/QtCore.framework/Versions/A/QtCore | grep -A3 LC_BUILD_VERSION
> /usr/libexec/PlistBuddy -c "Print :LSMinimumSystemVersion" <app>/Contents/Info.plist
> ```

## 构建

```bash
./build.sh                 # 增量编译（默认 Qt 6）
./build.sh --qt5           # 用 Qt 5 编译
./build.sh --clean         # 全量重编
./build.sh --package       # 编译 + 打包到 dist/U3LogicAnalyzer/
./build.sh --prefix <dir>  # 指定 install 前缀（默认 ./install）
```

产物：

| 路径 | 内容 |
|---|---|
| `build_cmake/logicanalyzer_build/U3LogicAnalyzer`（Windows 上 `.exe`） | 主程序 |
| `install/` | libsigrok + libsigrokcxx + libsigrokdecode 与 132 个解码器 |

> 构建目录可用 `U3_BUILD_DIR` 改到别的盘，例如 `U3_BUILD_DIR=/i/la_build ./build.sh`。
> debug 配置会用 `${U3_BUILD_DIR}_debug`。
>
> 切换 Qt 大版本时先整个删掉构建目录：CMake 缓存的 Qt 路径不会自己更新。

## 运行

构建产物依赖运行期环境变量，直接 `./start_pv.sh` 最省事。手动运行：

```bash
# Windows（MSYS2 MINGW64）
export PYTHONHOME=/mingw64
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer.exe

# Linux（用系统 Python，不需要 PYTHONHOME）
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
export LD_LIBRARY_PATH=install/lib:$LD_LIBRARY_PATH
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer
```

macOS 上直接 `./start_pv.sh`：脚本在同一分支里设置 `SIGROKDECODE_DIR` 与 Python 路径。程序要在
已登录的图形会话里运行，纯 SSH 会话（没有登录的桌面）起不了窗口。若提示找不到 `libsigrok*.dylib`，
把 `install/lib` 加进 `DYLD_LIBRARY_PATH`。

> 写进 `~/.bashrc` 可永久生效。已安装的 deb / 归档 / AppImage 不需要这些变量。

## 打包

### Windows

```bash
./build.sh --package       # 便携版：dist/U3LogicAnalyzer/
./package_installer.sh     # 安装包：dist/U3LogicAnalyzer_Setup.exe
```

便携版含主程序、全部依赖 DLL、`libsigrok*.dll`、`decoders/`、便携 Python 与驱动目录，拷走即用。
安装包（NSIS）带开始菜单项和桌面快捷方式，可从控制面板卸载。

### Linux

```bash
./package_installer.sh              # 构建 + 打包（Qt 6）
./package_installer.sh --qt5        # 用 Qt 5 构建
./package_installer.sh --skip-build # 跳过构建，直接打包现有 dist/
./package_installer.sh --no-clean   # 复用已有构建目录
```

| 产物 | 说明 |
|---|---|
| `U3LogicAnalyzer-<ver>-linux-<arch>.tar.gz` | 归档，内含 `install.sh` / `uninstall.sh` |
| `U3LogicAnalyzer-<ver>-linux-<arch>.deb` | `sudo apt install ./<file>.deb`，卸载 `sudo apt remove u3logicanalyzer` |
| `U3LogicAnalyzer-<ver>-<arch>.AppImage` | 免安装运行，附同名 `.run.sh` |

Qt 6 构建的文件名带 `-qt6` 后缀，两种 Qt 版本的产物不会互相覆盖。归档与 deb 会把程序装到
`/opt/U3LogicAnalyzer`（root）或 `~/.local/opt/U3LogicAnalyzer`（普通用户），并装上菜单项、
hicolor 图标树与 AppStream 元数据 —— 没有元数据时软件中心读不到信息，只显示占位图标。

> 本地安装的 deb 来自文件而非软件源，软件中心不会把它列进已装应用，也无法从那里卸载，
> 请用 `apt` / `dpkg`（或 Synaptic）。

**AppImage 注意事项**

- Ubuntu 24.04 起不再预装 `libfuse2`，直接运行会报 `dlopen(): error loading libfuse.so.2`。
  改用 `<file>.run.sh`（推荐，免 root 免 FUSE）、`--appimage-extract-and-run`，或装 `libfuse2`。
- 打包前把 `uruntime-<arch>` 放进 `dist/`，可产出完全不需要 FUSE 的 AppImage。
- 首次运行时 `AppRun` 会把启动器与图标注册进 `~/.local/share`，桌面环境才能按 app id
  显示名称和图标。设 `U3LA_NO_INTEGRATION=1` 可跳过。

### macOS

```bash
./package_installer.sh              # 构建 + 打包（Qt 6）
./package_installer.sh --no-clean   # 复用已有构建目录
```

产物为 `dist/U3LogicAnalyzer-<版本>-git-<提交号>-macos-<架构>.dmg`，其中包含 `.app` 与指向
`/Applications` 的链接，安装方式为拖入该文件夹。

- 卷名与文件名均带版本号与提交号。同名 dmg 会使 Finder 沿用上一次的窗口尺寸
  （`FXRecentFolders` 优先于 `.DS_Store` 中的 `WindowBounds`），加后缀可避免这一沿用。
- 安装界面的背景图、图标位置与窗口尺寸由 `packaging/macos/dmg-settings.py` 定义，背景图由
  `packaging/macos/make-dmg-background.py` 生成，两者均在仓库内。
- 需安装 `dmgbuild`：`pip install 'dmgbuild==1.6.1'`。更新的版本会调用 `diskutil image --plist`，
  macOS 13 不支持该选项，打包会直接失败。未安装 dmgbuild 时脚本退回 `hdiutil`，仍可生成 dmg，
  但没有背景图与图标布局。
- 应用使用 ad-hoc 签名，未做公证，首次启动需右键选择「打开」。
- 打包前不要在 `dist/` 中运行应用，也不要手工改动包内文件：两者都会破坏代码签名的封装完整性，
  打包时报 `a sealed resource is missing`。脚本在打包前会执行一次
  `codesign --verify --deep --strict`，失败时提示重新构建。

## udev 规则（Linux 免 root）

默认 `/dev/bus/usb/*` 归 root，libusb 打开设备会报 `LIBUSB_ERROR_ACCESS`：`lsusb` 能看到，
软件里扫描不到。

```bash
sudo ./install_udev_rules.sh            # 安装
sudo ./install_udev_rules.sh --remove   # 移除
```

| 规则文件 | 覆盖的设备 |
|---|---|
| `99-weact-ch32h417.rules` | `1a86:5537`（运行时接口）与 `1a86:fe17`（IAP 升级用的 HID 接口） |
| `99-fx2lafw.rules` | `1d50:608c` / `1d50:608d`（FX2 方案的 8 / 16 通道板） |

装完重新插拔设备。用 deb 安装时规则会随包装到 `/etc/udev/rules.d/`，无需手动执行。

## FX2 固件

`fx2lafw` 是给跑开源 fx2lafw 固件的 Cypress FX2 方案分析仪用的驱动（Saleae Logic 及其克隆、
USBee AX/DX、sigrok fx2grok 等）。FX2 自身没有 flash，固件由驱动下载进它的 RAM，断电即失，
所以固件文件每次都要在。

固件在 `firmware/` 目录（`fx2lafw-*.fw`），构建时安装到 `share/sigrok-firmware/`；
程序也会把 `SIGROK_FIRMWARE_DIR` 指向自己的副本，环境变量里的值优先。

## 添加自定义驱动

1. 在 `libsigrok/src/hardware/` 下新建 `your-device/`
2. 实现 `api.c`（必须）、`protocol.c`、`protocol.h`
3. 在 `libsigrok/CMakeLists.txt` → `LIBSIGROK_SOURCES` 添加源文件
4. 在 `libsigrok/config.h.cmake` 添加 `#define HAVE_HW_YOUR_DEVICE 1`
5. 在 `libsigrok/CMakeLists.txt` 添加 `set(HAVE_HW_YOUR_DEVICE 1)`
6. `./build.sh` 重新构建

> 两个现有驱动共用设备协议但不共用编译单元：所有全局符号都带 `ch32h417_libusb_` 之类的前缀，
> 避免同一份代码被编两次时重复定义。新增源文件后要重跑 CMake 配置（源文件列表是 GLOB）。

## 常见构建问题

**`collect2: ld returned 1`，提示无法写入 exe**
- 上个实例还在跑，占着 exe。先关掉程序（Windows：`taskkill /F /IM U3LogicAnalyzer.exe`）再编。

**改了 libsigrok 但程序行为没变**
- libsigrok 装到 `install/`，主程序链接的是那里的库。重新 install，并把
  `install/bin/libsigrok*.dll` 拷进 `build_cmake/logicanalyzer_build/`（Windows）。

**切换 Qt5/Qt6 后编译一堆莫名其妙的错**
- 构建目录里的 CMake 缓存还指着旧 Qt。删掉 `build_cmake/`（或 `$U3_BUILD_DIR`）重来。

**升级 MSYS2 依赖后增量构建只链接不重编**
- pacman 按包内时间戳落盘，新头文件可能比既有 `.obj` 还"旧"，ninja 判定无需重编。
  把升级过的头文件时间刷到当前（`glib.h`、`glibconfig.h`、`zip.h`、`boost/version.hpp`、
  `Python.h`）后重跑，或直接 `--clean`。

**Python 报 `Failed to import encodings module`**
- `PYTHONHOME` 没设或设错。Windows 上必须是 `/mingw64`，且运行时不要带外部的 `PYTHONPATH`。

**macOS 打出来的 dmg 装到 macOS 13 上提示系统版本过低**
- 所用 Qt 的 `minos` 高于 13.0，`build.sh` 会跟着它抬高 `LSMinimumSystemVersion`。换一份按
  13.0 编译的 Qt 并用 `PV_QT_PREFIX` 指定，见上文 macOS 一节。

**macOS 报 `a sealed resource is missing`**
- `.app` 的签名封装被破坏。常见原因是打包前在 `dist/` 中运行过应用（Python 会往 framework 里
  写入 `__pycache__`），或手工改动过包内文件。重新执行 `./build.sh --package` 生成干净的应用
  再打包。

**`dmgbuild` 报 `diskutil: unrecognized option --plist`**
- 装的 dmgbuild 版本太新。固定到 `pip install 'dmgbuild==1.6.1'`。
