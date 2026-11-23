# MCP Server

[English](mcp-server.md) · [返回 README](../README_zh_CN.md)

`u3logicanalyzer-mcp-server` 是一个基于 stdio 的 MCP 服务器，把逻辑分析仪的采集与解码能力
提供给支持 Model Context Protocol 的 AI 客户端。它本身不链接 libsigrok，而是调用随包分发的
`u3logicanalyzer-cli`，因此与图形界面共用同一份驱动、解码器和固件。

协议与工具集参照 KenosInc/sigrok-mcp-server（MIT 许可）实现，本项目用 C 重写并补充了三个工具，
详见下文「与上游的差异」。

## 位置

| 平台 | 路径 |
|---|---|
| Windows | `C:\Program Files\WeAct Studio\U3LogicAnalyzer\u3logicanalyzer-mcp-server.exe` |
| Linux（deb / tar.gz） | `/opt/U3LogicAnalyzer/u3logicanalyzer-mcp-server` |
| Linux（AppImage） | `usr/bin/u3logicanalyzer-mcp-server`（挂载目录内） |
| macOS | `/Applications/U3LogicAnalyzer.app/Contents/MacOS/u3logicanalyzer-mcp-server` |

服务器启动时按自身所在目录查找 `u3logicanalyzer-cli`，解码器与固件由该程序自己解析，
不需要设置任何环境变量。

## 客户端配置

面向最终用户的逐步说明（各平台的 `mcp.json` 完整示例、启用步骤、常见问题）见
[使用说明的「Agent 配置」一节](usage.zh_CN.md#agent-配置)，或安装目录下同一章的 PDF。
下面只列出与本服务器实现相关的部分。

在客户端的 MCP 配置文件里加入一项，把路径换成实际的安装位置。Windows 的配置文件为
`C:\Users\<用户名>\.workbuddy\mcp.json`：

```json
{
  "mcpServers": {
    "u3logicanalyzer": {
      "command": "C:\\Program Files\\WeAct Studio\\U3LogicAnalyzer\\u3logicanalyzer-mcp-server.exe",
      "args": [],
      "env": {
        "U3LOGICANALYZER_TIMEOUT_SECONDS": "600",
        "U3LOGICANALYZER_WORKING_DIR": "C:\\Users\\<用户名>\\AppData\\Local\\U3LogicAnalyzer\\workdir"
      },
      "disabled": false
    }
  }
}
```

macOS 与 Linux 只需替换 `command` 为上一节的路径。保存后重启客户端，在「连接器」页面的
「自定义连接器」里点「信任」才会启用。

### 环境变量

| 变量 | 默认值 | 说明 |
|---|---|---|
| `U3LOGICANALYZER_CLI_PATH` | 同目录下的 `u3logicanalyzer-cli` | 该程序的路径，只在需要指向别处时设置 |
| `U3LOGICANALYZER_TIMEOUT_SECONDS` | `30` | 单次调用超时，长时间采集需要调大 |
| `U3LOGICANALYZER_WORKING_DIR` | 继承当前目录 | 采集文件的写入目录，未指定 `output_file` 时生成的文件落在这里。目录须事先存在且当前用户可写，不要指向安装目录 |

## 工具

| 工具 | 说明 |
|---|---|
| `scan_devices` | 扫描已连接设备，可指定 driver 与 conn 缩小范围 |
| `show_driver_details` | 驱动的功能、连接参数、通道组、采样率与触发选项 |
| `capture_data` | 采集并保存到文件，`samples` 与 `time` 二选一 |
| `decode_protocol` | 对已保存的文件运行协议解码器 |
| `show_decoder_details` | 解码器的说明、选项、通道与注解类别 |
| `list_supported_hardware` | 支持的全部硬件驱动 |
| `list_supported_decoders` | 全部协议解码器，可用 `filter` 按 id 或说明过滤 |
| `list_input_formats` / `list_output_formats` | 可读 / 可写的文件格式 |
| `show_version` | 命令行工具、libsigrok、libsigrokdecode 的版本 |
| `check_firmware_status` | 固件的搜索目录与文件数量，包含本安装包自带的目录 |
| `capture_and_decode` | 采集与解码合成一次调用 |
| `set_device_options` | 只下发设备选项而不采集，用于校验配置 |

## 典型流程

```
scan_devices
  → show_driver_details  driver=wch-ch32h417-libusb
  → capture_and_decode  driver=wch-ch32h417-libusb  config=samplerate=4m
                        channels=D0  samples=1000000
                        protocol_decoders=uart:rx=D0:baudrate=115200
```

一次往返即可拿到「已存文件 + 解码结果」，文件保留在磁盘上，后续可以换解码器再分析，
不必重新采集。

### 设备选项的写法

`capture_data`、`capture_and_decode` 与 `set_device_options` 的 `config` 参数对应命令行的
`--config`：

- 多个选项之间用冒号分隔，例如 `samplerate=4m:voltage_threshold=1.2`。写成逗号同样可用，
  服务器会自行转换。
- 区间类型的选项要求写成 `下限-上限`。`voltage_threshold` 属于此类，只写一个数值会被判为
  非法；服务器会把单个数值补成 `v-v`，因此 `voltage_threshold=1.2` 与 `1.2-1.2` 等价。
- 该阈值是引脚上的电压，本机可设范围 0.60 V 至 1.50 V，超出时错误信息会给出实际范围。
- 采样率必须是设备支持的档位，先用 `show_driver_details` 查看。CH32H417 为 4 MHz 至
  200 MHz 共 25 档。

## 与上游的差异

- 上游的 `serial_query`（串口发送文本命令）与 `get_device_profile`（SCPI 仪器资料库）
  面向台式仪器，与逻辑分析仪无关，本实现未包含。
- 新增 `capture_and_decode`：采集与解码一次完成，减少一次往返。
- 新增 `set_device_options`：只下发选项，用来确认参数被设备接受。
- `list_supported_decoders` 增加 `filter` 参数；`check_firmware_status` 额外报告安装包自带的
  固件目录。

## 故障排查

1. **提示 `u3logicanalyzer-cli could not be started`**：`U3LOGICANALYZER_CLI_PATH` 指错了，
   或该程序不在服务器同目录。
2. **提示超时**：调大 `U3LOGICANALYZER_TIMEOUT_SECONDS`。长时间采集（例如按秒计）默认的 30 秒不够。
3. **`show_decoder_details` 报 `decoder sources not found`**：服务器按自身所在目录查找
   `decoders`，报这个错说明安装目录里没有该文件夹，重装即可。只有把解码器挪到别处时才
   需要设置 `SIGROKDECODE_DIR`。
4. **采集报 `invalid argument`**：采样率不在设备支持的列表内，先用 `show_driver_details`
   查看可选值。CH32H417 的采样率区间为 4 MHz 至 200 MHz。
