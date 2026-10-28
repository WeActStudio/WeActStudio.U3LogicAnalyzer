# CH32H417 逻辑分析仪 PC 软件

[English](README.md)

**U3LogicAnalyzer** 是 WeAct Studio CH32H417 逻辑分析仪的上位机软件，基于 sigrok PulseView，
自带 132 个协议解码器（I²C / SPI / UART 等）。硬件驱动裁剪到只剩本项目相关的两个，
另外保留 PulseView 的 `demo` 驱动与给 FX2 方案分析仪用的 `fx2lafw` 驱动，纯 CMake 一键构建。

![主界面](doc/images/zh_CN/main-window.png)

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

## 文档

| 文档 | 内容 |
|---|---|
| [使用说明](doc/usage.zh_CN.md) | 连接设备、设备选项、采集、看波形、协议解码、保存导出、多会话、固件升级、常见问题 |
| [构建说明](doc/building.zh_CN.md) | 依赖安装、Windows / Linux 构建、运行、打包、udev 规则、FX2 固件、添加驱动、构建问题 |

English: [Usage](doc/usage.md) · [Building](doc/building.md)

## 平台支持

| 平台 | 状态 | 依赖安装 |
|---|---|---|
| Windows 10/11 + MSYS2 MINGW64 | 支持 | `./setup_env.sh` |
| Linux（Debian / Ubuntu） | 支持 | `./setup_env_linux.sh`（apt） |
| macOS | **未适配** | 不提供（`setup_env_macos.sh` 保留但不再维护） |

## 设备访问与驱动后端

| | Windows | Linux | macOS |
|---|---|---|---|
| `wch-ch32h417`（CH375DLL） | 默认绑定 | 不参与构建 | 不参与构建 |
| `wch-ch32h417-libusb`（libusb） | 需先用 Zadig 改绑 WinUSB | 支持 | 未验证 |
| `fx2lafw`（libusb，FX2 方案设备） | 需先用 Zadig 改绑 WinUSB | 支持 | 未验证 |
| 固件升级（IAP） | 两个后端都行 | 仅 libusb 后端 | 未验证 |

Windows 上设备默认绑在 `CH375WDM.sys`（走 `wch-ch32h417` 驱动），想用 libusb 后端需先用
Zadig 改绑 WinUSB。非 Windows 平台只有 libusb 后端，其中 Linux 已实机验证，macOS 未适配。

## 目录结构

```
.
├── doc/                         # 使用说明与构建说明（中英双语，含截图）
├── setup_env.sh                 # Windows 环境部署
├── setup_env_linux.sh           # Linux（Debian/Ubuntu）环境部署
├── setup_env_macos.sh           # macOS 环境部署（保留，未适配、不再维护）
├── build.sh                     # 构建 + 便携包
├── package_installer.sh         # 安装包（NSIS / deb+tar.gz+AppImage）
├── install_udev_rules.sh        # Linux udev 规则安装
├── start_pv.sh                  # 带环境变量启动（-l 5）
├── install/                     # libsigrok / libsigrokcxx / libsigrokdecode
├── libsigrok/src/hardware/
│   ├── wch-ch32h417/            # CH375 后端（仅 Windows）
│   ├── wch-ch32h417-libusb/     # libusb 后端（全平台）
│   ├── fx2lafw/                 # FX2 方案分析仪
│   └── demo/
├── libsigrokdecode/decoders/    # 132 个协议解码器
├── pulseview/                   # 主程序（CMakeLists 中 OUTPUT_NAME=U3LogicAnalyzer）
└── build_cmake/logicanalyzer_build/
```

## 来源

本软件是在 **Q2H2** 的项目上修改而来。CH375 驱动（`libsigrok/src/hardware/wch-ch32h417/`）、
`demo` 驱动以及 libsigrok / libsigrokdecode 中未改动的源文件仍保留 `Copyright (C) 2026 Q2H2`
声明，本项目未对其署名作任何更改；新增的 libusb 驱动（`wch-ch32h417-libusb/`）版权归
WeAct Studio；`fx2lafw` 驱动（`libsigrok/src/hardware/fx2lafw/`）取自上游 sigrok，
保留其作者 Bert Vermeulen 与 Joel Holdsworth 的署名。

## 许可

GPL-3.0-or-later，见 [LICENSE](LICENSE)。
