/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2026 WeAct Studio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "agent_prompts.hpp"

#include <QClipboard>
#include <QDialogButtonBox>
#include <QFont>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

#include <pv/globalsettings.hpp>

namespace pv {
namespace dialogs {

const AgentPromptEntry AgentPrompts::EntriesZh[] = {
	{
		"找到命令行工具",
		"U3LogicAnalyzer 自带命令行工具 u3logicanalyzer-cli，Windows 默认在 "
		"C:\\Program Files\\WeAct Studio\\U3LogicAnalyzer\\，macOS 在 "
		"/Applications/U3LogicAnalyzer.app/Contents/MacOS/，Linux 在 "
		"/opt/U3LogicAnalyzer/。该目录一般不在 PATH 中，用完整路径调用；路径含空格，"
		"PowerShell 下要加调用运算符 &。先运行 --version 确认可用，再运行 --scan："
		"驱动标识（如 wch-ch32h417，CH375DLL 驱动；另有 wch-ch32h417-libusb 走 libusb）"
		"用于 -d，完整设备串（如 wch-ch32h417:conn=ch375/6-4）用于多台设备时指定其中一台。"
		"fx2lafw 方案的板子显示为 fx2lafw。扫描结果里的 demo 是内置虚拟设备，不是硬件。"
	},
	{
		"扫描设备",
		"用 u3logicanalyzer-cli --scan 扫描当前连接的逻辑分析仪，把型号、驱动标识、"
		"连接串（conn= 之后的部分）与通道数整理成表格。接的是 CH32H417 还是 fx2lafw "
		"要说清楚；结果中的 demo 是内置虚拟设备，不要算作真实硬件。"
	},
	{
		"采集波形并查看结果",
		"用 u3logicanalyzer-cli 以 4 MHz 采样率采集 2000 个采样点，保存为 test.sr："
		"-o test.sr（扩展名 .sr 会自动按 srzip 保存，不必再写 -O），然后用 "
		"U3LogicAnalyzer.exe test.sr 打开。确认显示看两点：底部标签栏出现该文件、"
		"左侧列出 D0–D15。2000 点在 4 MHz 下只有 0.5 ms，属链路自检，无信号时波形为"
		"平直线。最后回报采样率、采样点数、文件路径与大小。"
	},
	{
		"解码波形并整理结果",
		"把 test.sr 按 UART 解码：-P uart:rx=D0:baudrate=115200 -A uart=rx-data。"
		"注意 2000 点在 4 MHz 下只有 0.5 ms，而 115200 下一个字节约 87 µs，窗口太短会"
		"解不出内容；要真实数据先用 4 MHz 采集 200 ms（80 万点）存为 uart.sr 再解码。"
		"把结果按时间顺序整理成表格，列出起始时间、字段与数值，说明所用解码器与参数；"
		"没有解出数据时如实说明是无信号还是窗口过短，不要编造。"
	},
	{
		"接入 MCP 服务（可选）",
		"把 u3logicanalyzer-mcp-server 加进 MCP 客户端配置（如 "
		"%USERPROFILE%\\.workbuddy\\mcp.json 的 mcpServers）。command 填服务器可执行"
		"文件的完整路径：Windows 为 "
		"C:\\Program Files\\WeAct Studio\\U3LogicAnalyzer\\u3logicanalyzer-mcp-server.exe，"
		"macOS 为 /Applications/U3LogicAnalyzer.app/Contents/MacOS/u3logicanalyzer-mcp-server，"
		"Linux 为 /opt/U3LogicAnalyzer/u3logicanalyzer-mcp-server；同目录会自动找到"
		"命令行工具，不必另设。U3LOGICANALYZER_WORKING_DIR 指向用户目录下可写的文件夹，"
		"例如 C:\\Users\\<用户名>\\AppData\\Local\\U3LogicAnalyzer\\workdir，先创建该"
		"文件夹；不要指向安装目录，Program Files 与 /opt 下普通用户没有写权限。"
		"U3LOGICANALYZER_TIMEOUT_SECONDS 设为 600。JSON 里的反斜杠要写成双反斜杠，"
		"环境变量不会被展开，必须写绝对路径。配置完成后重启客户端并信任该连接器，"
		"再扫描设备确认能看到板子。"
	},
};

const AgentPromptEntry AgentPrompts::EntriesEn[] = {
	{
		"Find the command line tool",
		"U3LogicAnalyzer ships a command line tool called u3logicanalyzer-cli: "
		"on Windows it lives in C:\\Program Files\\WeAct Studio\\U3LogicAnalyzer\\, "
		"on macOS in /Applications/U3LogicAnalyzer.app/Contents/MacOS/, on Linux "
		"in /opt/U3LogicAnalyzer/. That folder is usually not on PATH, so call it "
		"by full path; the path contains spaces, and PowerShell needs the & "
		"operator. Run --version first, then --scan: the driver identifier (for "
		"example wch-ch32h417 on the CH375DLL backend, or wch-ch32h417-libusb on "
		"libusb) is what -d takes, while the full device string (for example "
		"wch-ch32h417:conn=ch375/6-4) picks one board when several are attached. "
		"An fx2lafw board shows up as fx2lafw. The demo entry in the scan result "
		"is a built-in virtual device, not hardware."
	},
	{
		"Scan the devices",
		"Use u3logicanalyzer-cli --scan to list the logic analyzers that are "
		"connected, and give me a table of model, driver identifier, connection "
		"string (the part after conn=) and channel count. Say whether it is a "
		"CH32H417 or an fx2lafw board; the demo entry is a built-in virtual "
		"device and does not count as hardware."
	},
	{
		"Capture a waveform and show it",
		"Capture 2000 samples at 4 MHz with u3logicanalyzer-cli into test.sr: "
		"-o test.sr, where the .sr extension already selects srzip so -O is not "
		"needed. Then open it with U3LogicAnalyzer.exe test.sr. To confirm it is "
		"shown, check that the file appears in the tab bar at the bottom and that "
		"D0-D15 are listed on the left. 2000 samples at 4 MHz span only 0.5 ms, a "
		"link check rather than a signal capture, so flat lines are expected when "
		"nothing is connected. Report the sample rate, sample count, file path and "
		"size."
	},
	{
		"Decode the waveform and tabulate it",
		"Decode test.sr as UART: -P uart:rx=D0:baudrate=115200 -A uart=rx-data. "
		"Note that 2000 samples at 4 MHz span 0.5 ms, while one byte at 115200 "
		"takes about 87 us, so a window that short decodes to nothing; for real "
		"data capture 200 ms (800000 samples) at 4 MHz into uart.sr first. Present "
		"the frames as a table ordered by time with start time, field and value, "
		"and name the decoder and options you used. If nothing decodes, say "
		"whether that is because there is no signal or because the window is too "
		"short; do not invent frames."
	},
	{
		"Connect it over MCP (optional)",
		"Add u3logicanalyzer-mcp-server to the MCP client configuration (the "
		"mcpServers object, for example in %USERPROFILE%\\.workbuddy\\mcp.json). "
		"command takes the full path of the server executable: "
		"C:\\Program Files\\WeAct Studio\\U3LogicAnalyzer\\u3logicanalyzer-mcp-server.exe "
		"on Windows, "
		"/Applications/U3LogicAnalyzer.app/Contents/MacOS/u3logicanalyzer-mcp-server "
		"on macOS, /opt/U3LogicAnalyzer/u3logicanalyzer-mcp-server on Linux. The "
		"command line tool next to it is found automatically. Point "
		"U3LOGICANALYZER_WORKING_DIR at a writable folder under the user profile, "
		"for example C:\\Users\\<user>\\AppData\\Local\\U3LogicAnalyzer\\workdir, "
		"and create it first; do not use the installation directory, which regular "
		"users cannot write to under Program Files or /opt. Set "
		"U3LOGICANALYZER_TIMEOUT_SECONDS to 600. In JSON the backslashes are "
		"doubled and environment variables are not expanded, so give absolute "
		"paths. Then restart the client, trust the connector, and scan the devices "
		"to confirm the board shows up."
	},
};

AgentPrompts::AgentPrompts(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Agent Prompts"));
	resize(760, 640);
	setMinimumSize(560, 420);

	GlobalSettings settings;
	const bool chinese =
		settings.value(GlobalSettings::Key_General_Language).toString() == "zh_CN";
	const AgentPromptEntry *entries = chinese ? EntriesZh : EntriesEn;

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *hint = new QLabel(
		tr("Copy a prompt and paste it into your AI assistant."), this);
	hint->setWordWrap(true);
	layout->addWidget(hint);

	QScrollArea *scroll = new QScrollArea(this);
	scroll->setWidgetResizable(true);
	scroll->setFrameShape(QFrame::NoFrame);

	QWidget *content = new QWidget(scroll);
	QVBoxLayout *cards = new QVBoxLayout(content);
	cards->setContentsMargins(0, 0, 0, 0);
	cards->setSpacing(10);

	for (int i = 0; i < EntryCount; i++)
		cards->addWidget(create_card(entries[i], content));

	cards->addStretch(1);
	scroll->setWidget(content);
	layout->addWidget(scroll, 1);

	QDialogButtonBox *buttons =
		new QDialogButtonBox(QDialogButtonBox::Close, this);
	/* Qt's own label comes from the Qt translations, which this package does
	 * not ship; the l10n files of the application always do. */
	buttons->button(QDialogButtonBox::Close)->setText(tr("Close"));
	connect(buttons, &QDialogButtonBox::rejected, this, &AgentPrompts::reject);
	layout->addWidget(buttons);
}

QWidget *AgentPrompts::create_card(const AgentPromptEntry &entry, QWidget *parent)
{
	/* A styled panel rather than a stylesheet: the theme is applied through
	 * palettes, and a widget carrying a stylesheet is skipped by it. */
	QFrame *card = new QFrame(parent);
	card->setFrameShape(QFrame::StyledPanel);

	QVBoxLayout *card_layout = new QVBoxLayout(card);
	card_layout->setContentsMargins(10, 8, 10, 10);
	card_layout->setSpacing(6);

	QHBoxLayout *header = new QHBoxLayout();

	QLabel *title = new QLabel(QString::fromUtf8(entry.title), card);
	QFont title_font = title->font();
	title_font.setBold(true);
	title->setFont(title_font);
	header->addWidget(title);
	header->addStretch(1);

	QPushButton *copy = new QPushButton(tr("Copy"), card);
	copy->setAutoDefault(false);
	header->addWidget(copy);

	card_layout->addLayout(header);

	/* A label rather than a text box: the prompt is meant to be read and
	 * copied whole, and a label always asks for the height its wrapped text
	 * needs, so no box ever hides its tail behind a scrollbar. */
	QLabel *body = new QLabel(QString::fromUtf8(entry.body), card);
	body->setWordWrap(true);
	body->setTextFormat(Qt::PlainText);
	body->setTextInteractionFlags(Qt::TextSelectableByMouse);
	card_layout->addWidget(body);

	/* The label goes back to "Copy" on its own; starting the timer again on
	 * every click keeps it reading "Copied" while the button is still used. */
	QTimer *restore = new QTimer(copy);
	restore->setSingleShot(true);
	restore->setInterval(1500);
	connect(restore, &QTimer::timeout, copy, [copy]() {
		copy->setText(tr("Copy"));
	});
	connect(copy, &QPushButton::clicked, restore, [body, copy, restore]() {
		QGuiApplication::clipboard()->setText(body->text());
		copy->setText(tr("Copied"));
		restore->start();
	});

	return card;
}

} // namespace dialogs
} // namespace pv
