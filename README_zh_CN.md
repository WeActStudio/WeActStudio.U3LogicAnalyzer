# CH32H417 LogicAnalyzer PC软件构建

[English](README.md)

该工程仅支持项目对应的 **WeAct Studio CH32H417 逻辑分析仪**，主程序名为 **U3LogicAnalyzer**
（基于 sigrok PulseView）。自带 132 个协议解码器（I²C / SPI / UART 等），
sigrok 的硬件驱动已裁剪到只剩本项目相关的两个，纯 CMake 一键构建。

> **来源**：本软件是在 **Q2H2** 的项目上修改而来。CH375 驱动
> （`libsigrok/src/hardware/wch-ch32h417/`）、`demo` 驱动以及 libsigrok /
> libsigrokdecode 中未改动的源文件仍保留 `Copyright (C) 2026 Q2H2` 声明，
> 本项目未对其署名作任何更改；新增的 libusb 驱动
> （`wch-ch32h417-libusb/`）版权归 WeAct Studio。

## 快速开始

```bash
# Windows（MSYS2 MINGW64）
./setup_env.sh
./build.sh
./start_pv.sh

# Linux（Debian / Ubuntu）
./setup_env_linux.sh
sudo ./install_udev_rules.sh      # 免 root 打开设备（只需一次）
./build.sh
./start_pv.sh

# macOS（Homebrew）
./setup_env_macos.sh
./build.sh
./start_pv.sh
```

`start_pv.sh` 会自己设好 `PYTHONHOME` / `SIGROKDECODE_DIR` / `LD_LIBRARY_PATH`
并启动 `build_cmake/logicanalyzer_build/U3LogicAnalyzer`。

## 环境要求

| 平台 | 依赖安装 |
|---|---|
| Windows 10/11 + MSYS2 MINGW64 | `./setup_env.sh` |
| Linux（Debian / Ubuntu） | `./setup_env_linux.sh`（apt） |
| macOS | `./setup_env_macos.sh`（Homebrew） |

**Windows（MSYS2 MINGW64）**

```bash
# 一键部署
./setup_env.sh

# 或手动安装:
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,qt5-static,python}
```

> 在 Windows 上必须用 **MSYS2 MINGW64** shell（开始菜单 → MSYS2 MINGW64）。
> macOS / Linux 用普通 shell 即可，`build.sh` 会自动识别平台。
> Qt 版本由 `--qt5` / `--qt6` 选择，Windows 上对应 `qt5-static` / `qt6-static`，
> Linux 上用系统自带的 Qt。

## 设备访问与驱动后端

| | Windows | Linux | macOS |
|---|---|---|---|
| `wch-ch32h417`（CH375DLL） | ✅ 默认绑定 | ❌ 不参与构建 | ❌ 不参与构建 |
| `wch-ch32h417-libusb`（libusb） | ✅ 需先用 Zadig 改绑 WinUSB | ✅ | ✅ |
| 热插拔 | CH375 回调 + libusb 轮询 | libusb 轮询 | libusb 轮询 |
| 固件升级（IAP） | 两个后端都行 | 仅 libusb 后端 | 仅 libusb 后端 |

Windows 上设备默认绑在 `CH375WDM.sys`（走 `wch-ch32h417` 驱动），
想用 libusb 后端需先用 Zadig 改绑 WinUSB。非 Windows 平台只有 libusb 后端，
CH375 驱动的源文件会在 CMake 配置阶段被移除（`libsigrok/CMakeLists.txt` 中
`if(NOT WIN32)` 的 `list(REMOVE_ITEM ...)`）。

**Linux 免 root 访问**：默认 `/dev/bus/usb/*` 归 root 所有，libusb 打开设备会报
`LIBUSB_ERROR_ACCESS`（`lsusb` 能看到设备，但软件里扫描不到）。

```bash
sudo ./install_udev_rules.sh            # 安装
sudo ./install_udev_rules.sh --remove   # 移除
```

规则文件是 `99-weact-ch32h417.rules`，覆盖两个 USB ID：

- `1a86:5537` — 运行时接口（命令 / 逻辑数据 / ADC 数据）
- `1a86:fe17` — IAP 固件升级用的 HID 接口（同时给 hidraw 节点授权）

装完需重新插拔设备。用 deb 安装时规则会随包一起装到 `/etc/udev/rules.d/`，无需手动执行。

## 构建

```bash
./build.sh                 # 增量编译（默认 Qt 5）
./build.sh --qt6           # 用 Qt 6 编译
./build.sh --clean         # 全量重编
./build.sh --package       # 编译 + 打包到 dist/U3LogicAnalyzer/
./build.sh --prefix <dir>  # 指定 install 前缀（默认 ./install）
```

产物：

- `build_cmake/logicanalyzer_build/U3LogicAnalyzer`（Windows 上 `.exe`）— 主程序
- `install/` — libsigrok + libsigrokcxx + libsigrokdecode + 132 个解码器

## 运行

构建产物依赖运行期环境变量，直接用 `./start_pv.sh` 最省事。手动运行：

```bash
# Windows（MSYS2 MINGW64）
export PYTHONHOME=/mingw64
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer.exe

# Linux / macOS（用系统 Python，不需要 PYTHONHOME）
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
export LD_LIBRARY_PATH=install/lib:$LD_LIBRARY_PATH
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer
```

> 写入 `~/.bashrc` 可永久生效。已安装的 deb / 归档 / AppImage 不需要这些变量。

## 项目结构

```
.
├── setup_env.sh                 # Windows 环境部署
├── setup_env_linux.sh           # Linux（Debian/Ubuntu）环境部署
├── setup_env_macos.sh           # macOS 环境部署
├── build.sh                     # 构建 + 便携包
├── package_installer.sh         # 安装包（NSIS / deb+tar.gz+AppImage）
├── install_udev_rules.sh        # Linux udev 规则安装
├── 99-weact-ch32h417.rules      # 设备免 root 访问规则
├── start_pv.sh                  # 带环境变量启动（-l 5）
├── install/                     # libsigrok / libsigrokcxx / libsigrokdecode
├── libsigrok/
│   └── src/hardware/
│       ├── wch-ch32h417/        # CH375 后端（仅 Windows）
│       ├── wch-ch32h417-libusb/ # libusb 后端（全平台）
│       └── demo/
├── libsigrokdecode/decoders/    # 132 个协议解码器
├── pulseview/                   # 主程序（CMakeLists 中 OUTPUT_NAME=U3LogicAnalyzer）
└── build_cmake/logicanalyzer_build/
```

## 打包

### Windows

```bash
./build.sh --package       # 便携版：dist/U3LogicAnalyzer/
./package_installer.sh     # 安装包：dist/U3LogicAnalyzer_Setup.exe
```

便携版内含主程序、全部依赖 DLL、`libsigrok*.dll`、`decoders/`、便携 Python 与驱动目录，
拷走即用。安装包（NSIS）带开始菜单 + 桌面快捷方式，控制面板可卸载。

### Linux

```bash
./package_installer.sh              # 构建 + 打包（Qt 5）
./package_installer.sh --qt6        # 同上，用 Qt 6 构建
./package_installer.sh --skip-build # 跳过构建，直接打包现有 dist/
./package_installer.sh --no-clean   # 复用已有构建目录
```

| 产物 | 说明 |
|---|---|
| `U3LogicAnalyzer-<ver>-linux-<arch>.tar.gz` | 归档，内含 `install.sh` / `uninstall.sh` |
| `U3LogicAnalyzer-<ver>-linux-<arch>.deb` | `sudo apt install ./<file>.deb`，卸载 `sudo apt remove u3logicanalyzer` |
| `U3LogicAnalyzer-<ver>-<arch>.AppImage` | 免安装运行，附同名 `.run.sh` |

Qt 6 构建的文件名带 `-qt6` 后缀，两种 Qt 版本的产物不会互相覆盖。
归档和 deb 会把程序装到 `/opt/U3LogicAnalyzer`（root）或
`~/.local/opt/U3LogicAnalyzer`（普通用户），并装上菜单项
`U3LogicAnalyzer.desktop`、hicolor 图标树与 AppStream 元数据
（`U3LogicAnalyzer.metainfo.xml`）—— 没有元数据的话软件中心读不到信息，
只会显示占位图标。

> 本地安装的 deb 来自文件而非软件源，因此软件中心不会把它列进已装应用、
> 也无法从那里卸载，请用 `apt` / `dpkg`（或 Synaptic）。

**AppImage 注意事项**

- Ubuntu 24.04 及以后不再预装 `libfuse2`，直接双击会报
  `dlopen(): error loading libfuse.so.2`。改用 `<file>.run.sh`（推荐，免 root 免 FUSE）、
  `--appimage-extract-and-run`，或 `sudo apt install libfuse2`。
- 把 `uruntime-<arch>` 放进 `dist/` 再打包，可产出完全不需要 FUSE 的 AppImage。
- 首次运行时 `AppRun` 会把菜单项和图标注册进 `~/.local/share`，这样桌面环境
  才能按 app id 显示正确的名称和图标，而不是退回窗口标题 + 占位图标
  （shell 只监视会话启动时就存在的数据目录，光导出 `XDG_DATA_DIRS` 不够）。
  设 `U3LA_NO_INTEGRATION=1` 可跳过，每一步都是 best effort。

## 添加自定义驱动

1. 在 `libsigrok/src/hardware/` 下新建 `your-device/`
2. 实现 `api.c`（必须）、`protocol.c`、`protocol.h`
3. 在 `libsigrok/CMakeLists.txt` → `LIBSIGROK_SOURCES` 添加源文件
4. 在 `libsigrok/config.h.cmake` 添加 `#define HAVE_HW_YOUR_DEVICE 1`
5. 在 `libsigrok/CMakeLists.txt` 添加 `set(HAVE_HW_YOUR_DEVICE 1)`
6. `./build.sh` 重新构建

> 两个现有驱动共用设备协议但**不共用编译单元**：所有全局符号都带
> `ch32h417_libusb_` 之类的前缀，避免同一份代码被编两次时重复定义。
