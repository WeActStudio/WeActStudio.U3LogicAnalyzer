# u3logicanalyzer-cli

扫描逻辑分析仪、采集波形、导出会话文件与协议解码的命令行工具，随 U3LogicAnalyzer 一起分发，
供脚本与 AI 代理在没有图形界面的环境下驱动设备。

本工具由 sigrok-cli 0.8.0 改写而来，中间经历 CH32H417-sigrok-cli 这一版；各源文件头部的版权行
按改写顺序列出原作者与 WeAct Studio，`session.c` 另含 OpenChipLab 一行。本工具跟随本仓库的
libsigrok 与 libsigrokdecode 编译，`sigrok-cli` 的版本号不再沿用，`--version` 报告的是产品版本
（取自 `pulseview/CMakeLists.txt` 的 `PV_VERSION_STRING`）。

## 构建

由仓库根目录的 `build.sh` 编译，产物直接复制到 `dist/U3LogicAnalyzer/`（macOS 为应用包的
`Contents/MacOS/`），没有单独的安装规则。

## 使用

`u3logicanalyzer-cli --help` 列出全部选项；常见调用方式与参数表见仓库根目录的
`doc/usage.md`（中文为 `doc/usage.zh_CN.md`），其中也说明了与 MCP 服务的配合方式。

## 许可与出处

GPL-3.0-or-later，与上游一致。`AUTHORS` 与 `COPYING` 为上游原件，未作改动；许可证全文同时在
仓库根目录的 `LICENSE` 中。上游的 `NEWS` 未随本目录保留，其中记录的是 sigrok-cli 0.7.0 及更早
的版本历史，与本工具的版本号无关。
