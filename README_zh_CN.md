# CH32H417 LogicAnalyzer PC软件构建

[English](README.md)

该工程仅支持项目对应的 **WeAct Studio CH32H417 逻辑分析仪**，主程序名为 **U3LogicAnalyzer**
（基于 sigrok PulseView）。自带 132 个协议解码器（I²C / SPI / UART 等），
sigrok 的硬件驱动已裁剪到只剩本项目相关的两个（PulseView 自带的 `demo` 驱动保留，
另外保留了给 FX2 方案逻辑分析仪用的 `fx2lafw` 驱动），纯 CMake 一键构建。

> **来源**：本软件是在 **Q2H2** 的项目上修改而来。CH375 驱动
> （`libsigrok/src/hardware/wch-ch32h417/`）、`demo` 驱动以及 libsigrok /
> libsigrokdecode 中未改动的源文件仍保留 `Copyright (C) 2026 Q2H2` 声明，
> 本项目未对其署名作任何更改；新增的 libusb 驱动
> （`wch-ch32h417-libusb/`）版权归 WeAct Studio；`fx2lafw` 驱动
> （`libsigrok/src/hardware/fx2lafw/`）取自上游 sigrok，保留其作者
> Bert Vermeulen 与 Joel Holdsworth 的署名。

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

```

`start_pv.sh` 会自己设好 `PYTHONHOME` / `SIGROKDECODE_DIR` / `LD_LIBRARY_PATH`
并启动 `build_cmake/logicanalyzer_build/U3LogicAnalyzer`。

## 环境要求

| 平台 | 状态 | 依赖安装 |
|---|---|---|
| Windows 10/11 + MSYS2 MINGW64 | ✅ 支持 | `./setup_env.sh` |
| Linux（Debian / Ubuntu） | ✅ 支持 | `./setup_env_linux.sh`（apt） |
| macOS | ❌ **未适配** | 不提供（`setup_env_macos.sh` 保留但已不再维护） |

> **macOS 尚未适配**：`setup_env_macos.sh` 与 `build.sh` 里的 macOS 分支是早期留下的，
> 从未在 macOS 上实机编译或运行验证过。libusb 后端在代码层面不依赖任何 Windows 专有库，
> 理论上可以工作，但目前**不提供 macOS 构建支持**，遇到依赖或编译问题请自行解决。

**Windows（MSYS2 MINGW64）**

```bash
# 一键部署
./setup_env.sh

# 或手动安装:
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
    mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
    mingw-w64-x86_64-{boost,qt6-static,python}
```

> 在 Windows 上必须用 **MSYS2 MINGW64** shell（开始菜单 → MSYS2 MINGW64）。
> Linux 用普通 shell 即可，`build.sh` 会自动识别平台（macOS 未适配）。
> 默认 Qt 6，`--qt5` 可改用 Qt 5；Windows 上对应 `qt6-static` / `qt5-static`，
> Linux 上用系统自带的 Qt。

## 设备访问与驱动后端

| | Windows | Linux | macOS |
|---|---|---|---|
| 整体支持 | ✅ | ✅ | ❌ 未适配 |
| `wch-ch32h417`（CH375DLL） | ✅ 默认绑定 | ❌ 不参与构建 | ❌ 不参与构建 |
| `wch-ch32h417-libusb`（libusb） | ✅ 需先用 Zadig 改绑 WinUSB | ✅ | 未验证 |
| `fx2lafw`（libusb，FX2 方案设备） | ✅ 需先用 Zadig 改绑 WinUSB | ✅ | 未验证 |
| 热插拔 | CH375 回调 + libusb 轮询 | libusb 轮询 | 未验证 |
| 固件升级（IAP） | 两个后端都行 | 仅 libusb 后端 | 未验证 |

Windows 上设备默认绑在 `CH375WDM.sys`（走 `wch-ch32h417` 驱动），
想用 libusb 后端需先用 Zadig 改绑 WinUSB。非 Windows 平台只有 libusb 后端，
CH375 驱动的源文件会在 CMake 配置阶段被移除（`libsigrok/CMakeLists.txt` 中
`if(NOT WIN32)` 的 `list(REMOVE_ITEM ...)`）。其中 Linux 已实机验证，macOS 未适配。

**fx2lafw** 是给「跑开源 fx2lafw 固件的 Cypress FX2 方案」逻辑分析仪用的驱动：
Saleae Logic 及其克隆、USBee AX/DX 系列、sigrok fx2grok 等。它和上面的 libusb
后端同类，Windows 上同样要先用 Zadig 把板子改绑到 WinUSB。

FX2 是 USB 2.0 高速（High Speed）器件，驱动给出的采样率（8 通道最高 24 MHz、
16 通道最高 12 MHz）就是它的批量端点在高速链路下能跟上的速率。驱动打开设备时会
打印实际协商到的链路速率：若只跑到全速（Full Speed，比如经过慢速 HUB，或线缆只通了
四根线），它能用的就只有较低的采样率 —— 会明确报出来，而不是悄悄返回并不存在的采样。

FX2 自身没有 flash，固件并不存留在板子上：它由驱动下载进 FX2 的 RAM（最晚在扫描
设备时完成），板子随即重新枚举成 `fx2lafw` 的 ID，驱动等着它出现然后打开。**一断电
就没了**，所以固件文件每次都要在 —— 没有这些文件，FX2 方案的逻辑分析仪根本用不起来。

这些文件就在本仓库的 `firmware/` 目录里（即 sigrok-firmware-fx2lafw 软件包里的
`fx2lafw-*.fw`），构建时会安装到安装树的 `share/sigrok-firmware/` —— 这是 libsigrok
会查找的位置之一；程序自己也会把 `SIGROK_FIRMWARE_DIR` 指向它（和解码器目录一个做法），
环境变量里的值优先。若板子已经跑着固件（同一次上电里刚被扫描烧录过），会以
`sigrok fx2lafw` 的身份被直接打开，不会再下载一次。

**Linux 免 root 访问**：默认 `/dev/bus/usb/*` 归 root 所有，libusb 打开设备会报
`LIBUSB_ERROR_ACCESS`（`lsusb` 能看到设备，但软件里扫描不到）。

```bash
sudo ./install_udev_rules.sh            # 安装
sudo ./install_udev_rules.sh --remove   # 移除
```

规则文件有两个，每种分析仪一个：

- `99-weact-ch32h417.rules` — `1a86:5537`（运行时接口：命令 / 逻辑数据 / ADC 数据）
  与 `1a86:fe17`（IAP 固件升级用的 HID 接口，含 hidraw 节点）
- `99-fx2lafw.rules` — `1d50:608c` 与 `1d50:608d`，FX2 方案的 8 / 16 通道板

装完需重新插拔设备。用 deb 安装时规则会随包一起装到 `/etc/udev/rules.d/`，无需手动执行。

## 构建

```bash
./build.sh                 # 增量编译（默认 Qt 6）
./build.sh --qt5           # 用 Qt 5 编译
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

# Linux（用系统 Python，不需要 PYTHONHOME；macOS 未适配）
export SIGROKDECODE_DIR=install/share/libsigrokdecode/decoders
export LD_LIBRARY_PATH=install/lib:$LD_LIBRARY_PATH
cd build_cmake/logicanalyzer_build && ./U3LogicAnalyzer
```

> 写入 `~/.bashrc` 可永久生效。已安装的 deb / 归档 / AppImage 不需要这些变量。

## 使用说明

### 打开已保存的采集文件

`.sr`（sigrok session）文件可从菜单打开，也可以在命令行直接交给程序：

```bash
./U3LogicAnalyzer <file>.sr            # Windows：U3LogicAnalyzer.exe <file>.sr
./U3LogicAnalyzer -i <file>.sr         # 等价的选项写法
```

显示的内容以文件本身为准：文件里只有被启用通道的数据，所以只为这些通道建立信号，
通道弹窗里也只列它们 —— 不会给文件里没有数据的通道留空行。比窗口更短的采集打开即
铺满窗口；更长的采集用滚轮缩小或点「适应窗口」，缩到头就是数据的末尾 —— 这个上限
取自文件本身的数据，而不是工具栏上的采样数（后者属于硬件）。

### 8 / 16 通道

设备可以按 8 或 16 路逻辑通道采集（`SR_CONF_NUM_LOGIC_CHANNELS`）。8 通道窄模式下
一个采样只占 1 字节而不是 2 字节，同样的采集量内存减半 —— 或者说同样的内存能存
2 倍采样点。通道数在工具栏选择，开始采集前下发给设备，可选的采样率也随之变化。

> 窄模式采集存下来也是窄的：`.sr` 的探针数按采样的实际宽度写入，
> 所以 8 通道文件不会被当成 16 通道读回。

### 长采集与内存

采集数据在内存里完整保存，长时间高采样率有可能耗尽机器内存。程序在收数过程中会
查看剩余可用内存，一旦低于 512 MB 就停止采集并给出提示，而不是等系统开始换页、
窗口卡死。已采到的数据会保留，只是采集提前结束 —— 想装进内存就用更少的通道、
更低的采样率或更短的时长。

## 项目结构

```
.
├── setup_env.sh                 # Windows 环境部署
├── setup_env_linux.sh           # Linux（Debian/Ubuntu）环境部署
├── setup_env_macos.sh           # macOS 环境部署（保留，未适配、不再维护）
├── build.sh                     # 构建 + 便携包
├── build_pv.sh                  # 只构建，start_pv.sh 走的就是它
├── build_pv_debug.sh            # 同上，debug 配置
├── package_installer.sh         # 安装包（NSIS / deb+tar.gz+AppImage）
├── install_udev_rules.sh        # Linux udev 规则安装
├── 99-weact-ch32h417.rules      # 设备免 root 访问规则
├── 99-fx2lafw.rules             # FX2 板免 root 访问规则
├── start_pv.sh                  # 带环境变量启动（-l 5）
├── start_pv_debug.sh            # 启动 debug 构建（-l 5）
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
./package_installer.sh              # 构建 + 打包（Qt 6）
./package_installer.sh --qt5        # 同上，用 Qt 5 构建
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
