<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN" sourcelanguage="en">
<context>
    <name>Channels</name>
</context>
<context>
    <name>QApplication</name>
    <message>
        <location filename="../pv/devicemanager.cpp" line="278"/>
        <source>Error when scanning device driver &apos;%1&apos;: %2</source>
        <translation>扫描设备驱动程序时出错 &apos;%1&apos;: %2</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="72"/>
        <source>Querying config key %1 is not allowed</source>
        <translation>不允许查询 %1 配置</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="81"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>查询配置 &apos;%1&apos;: %2</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="95"/>
        <source>Unknown type supplied when attempting to query %1</source>
        <translation>查询 %1 时 返回未知类型</translation>
    </message>
</context>
<context>
    <name>QHexView</name>
    <message>
        <location filename="../pv/views/decoder_binary/QHexView.cpp" line="347"/>
        <source>No data available</source>
        <translation>无可用数据</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>Stack trace of previous crash:</source>
        <translation type="vanished">跟踪上次崩溃日志：</translation>
    </message>
    <message>
        <source>Don&apos;t show this message again</source>
        <translation type="vanished">不再显示此消息</translation>
    </message>
    <message>
        <source>When %1 last crashed, it created a stack trace.
A human-readable form has been saved to disk and was written to the log. You may access it from the settings dialog.</source>
        <translation type="vanished">上次崩溃时 %1 ，创建了日志文件。
可在 设置\Logging 中查看。</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="67"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="98"/>
        <source>Scanning for devices that driver %1 can access...</source>
        <translation>正在扫描驱动程序 %1 可以访问的设备...</translation>
    </message>
</context>
<context>
    <name>SubWindow</name>
</context>
<context>
    <name>View</name>
</context>
<context>
    <name>pv</name>
</context>
<context>
    <name>pv::MainWindow</name>
    <message>
        <source>Current available memory size is less than 4GB and will affect usage effect. 
Do you want to continue to use?</source>
        <translation>当前可用内存不足4GB，将可能会影响使用，
是否继续使用?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="336"/>
        <source>Decoder Selector</source>
        <translation>协议解码器</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="402"/>
        <source>Session %1</source>
        <translation>会话 %1</translation>
    </message>
    <message>
        <source>device detached!
Do you want to save the session?</source>
        <translation>设备拔出!
是否保存当前会话?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="927"/>
        <source>Start/Stop Acquisition</source>
        <translation>开始/停止 采集</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1002"/>
        <source>Reload</source>
        <translation>重复采集</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="939"/>
        <source>Settings</source>
        <translation>设置</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1002"/>
        <location filename="../pv/mainwindow.cpp" line="1010"/>
        <source>Run</source>
        <translation>开始</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1016"/>
        <source>Stop</source>
        <translation>停止</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="930"/>
        <location filename="../pv/mainwindow.cpp" line="931"/>
        <source>Repeat Acquisition</source>
        <translation>重复采样</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="108"/>
        <location filename="../pv/mainwindow.cpp" line="1082"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <source>Warning</source>
        <translation>警告</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1066"/>
        <source>Untitled</source>
        <translation>未命名</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Unsaved Capture Data&lt;/h3&gt;&lt;p&gt;Data will be permanently lost on exit.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;有未保存的采集数据&lt;/h3&gt;&lt;p&gt;退出后数据将永久丢失。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1081"/>
        <source>Discard &amp;&amp; Exit</source>
        <translation>放弃并退出</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1117"/>
        <source>Capture Failed</source>
        <translation>采集失败</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1493"/>
        <source>Device stopped sending data</source>
        <translation>设备已停止发送数据</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1494"/>
        <source>The device stopped sending samples before the capture was due to end, usually because the USB link cannot sustain the configured %1.&#10;&#10;The acquisition was stopped. The data captured before that may be incomplete or wrong. Try a lower sample rate, fewer channels, or another USB port.</source>
        <translation>设备在采集结束前就不再上传数据，通常是因为 USB 链路无法承受当前设置的 %1。&#10;&#10;采集已停止。在此之前采集到的数据可能不完整或不正确。请降低采样率、减少通道数，或换一个 USB 端口。</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1338"/>
        <location filename="../pv/mainwindow.cpp" line="1364"/>
        <source>Confirm</source>
        <translation>确认</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1339"/>
        <location filename="../pv/mainwindow.cpp" line="1365"/>
        <source>This session contains unsaved data. Close anyway?</source>
        <translation>此会话包含未保存的数据。确定关闭？</translation>
    </message>
    <message>
        <source>No udev access rules were found for the CH32H417. Without them the device (VID 1a86 / PID 5537) cannot be opened by an unprivileged user, so the logic analyzer will not be detected.

Enter your administrator password to install:
/etc/udev/rules.d/99-weact-ch32h417.rules</source>
        <translation>未检测到 CH32H417 的 udev 权限规则。缺少该规则时普通用户无法访问设备（VID 1a86 / PID 5537），软件将扫描不到逻辑分析仪。

请输入管理员密码以安装规则：
/etc/udev/rules.d/99-weact-ch32h417.rules</translation>
    </message>
    <message>
        <source>Administrator privileges required</source>
        <translation>需要管理员权限</translation>
    </message>
    <message>
        <source>Access rules installed</source>
        <translation>权限规则已安装</translation>
    </message>
    <message>
        <source>The udev rules have been installed.

If the device is already plugged in, unplug and replug it before scanning again.</source>
        <translation>udev 规则安装完成。

若设备此时已经插上，请拔插一次后再扫描。</translation>
    </message>
    <message>
        <source>Installation failed</source>
        <translation>安装失败</translation>
    </message>
    <message>
        <source>Could not install the udev rules:

%1

Please try again, or cancel to skip.</source>
        <translation>无法安装 udev 规则：

%1

请重试，或选择取消跳过。</translation>
    </message>
    <message>
        <source>Skipped installing access rules</source>
        <translation>已跳过权限规则安装</translation>
    </message>
    <message>
        <source>The udev rules are not installed, so the device may not be detected.

You can run this application as root later, or copy 99-weact-ch32h417.rules from the repository to /etc/udev/rules.d/ and replug the device.</source>
        <translation>未安装 udev 规则，软件可能扫描不到设备。

可之后以管理员身份运行本软件，或把仓库中的 99-weact-ch32h417.rules 复制到 /etc/udev/rules.d/ 后拔插设备。</translation>
    </message>
</context>
<context>
    <name>pv::Session</name>
    <message>
        <location filename="../pv/session.cpp" line="430"/>
        <source>Can&apos;t restore generated signal of unknown type %1 (%2)</source>
        <translation>无法还原生成的未知类型的信号 %1 (%2)</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="808"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="809"/>
        <source>Unexpected input format: %1</source>
        <translation>不支持的输入格式： %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="844"/>
        <source>Failed to load %1</source>
        <translation>无法加载 %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="883"/>
        <source>No active device set, can&apos;t start acquisition.</source>
        <translation>未设置活动设备，无法采集。</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="897"/>
        <source>No channels enabled.</source>
        <translation>未启用任何通道。</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="619"/>
        <source>Out of memory, acquisition stopped.</source>
        <translation>内存不足，采集停止。</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="624"/>
        <source>this feature is currently not supported.</source>
        <translation>此功能暂不支持。</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1735"/>
        <source>Can&apos;t handle more than 64 logic channels.</source>
        <translation>无法处理超过64个逻辑通道。</translation>
    </message>
</context>
<context>
    <name>pv::StoreSession</name>
    <message>
        <location filename="../pv/storesession.cpp" line="116"/>
        <source>Can&apos;t save logic channel without data.</source>
        <translation>无法保存，逻辑通道内没有数据。</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="132"/>
        <source>Can&apos;t save analog channel without data.</source>
        <translation>无法保存，模拟通道内没有数据。</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="144"/>
        <source>No channels enabled.</source>
        <translation>未启用任何通道。</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="169"/>
        <source>Can&apos;t save range without sample data.</source>
        <translation>无法保存，光标范围内没有数据。</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="190"/>
        <location filename="../pv/storesession.cpp" line="297"/>
        <location filename="../pv/storesession.cpp" line="312"/>
        <source>Error while saving: </source>
        <translation>保存时出错： </translation>
    </message>
</context>
<context>
    <name>pv::binding::Device</name>
    <message>
        <location filename="../pv/binding/device.cpp" line="84"/>
        <source>Note for device developers: Ignoring device configuration capability &apos;%1&apos; as it is missing GET and/or SET</source>
        <translation>设备开发人员注意：缺少 GET/SET，忽略该设备配置功能 &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../pv/binding/device.cpp" line="109"/>
        <source>No Limit</source>
        <translation>没有限制</translation>
    </message>
</context>
<context>
    <name>pv::data::DecodeSignal</name>
    <message>
        <location filename="../pv/data/decodesignal copy.cpp" line="239"/>
        <location filename="../pv/data/decodesignal.cpp" line="237"/>
        <source>No decoders</source>
        <translation>没有解码器</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal copy.cpp" line="247"/>
        <location filename="../pv/data/decodesignal.cpp" line="244"/>
        <source>There are no channels assigned to this decoder</source>
        <translation>解码器没有选择通道</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal copy.cpp" line="263"/>
        <location filename="../pv/data/decodesignal.cpp" line="258"/>
        <source>One or more required channels have not been specified</source>
        <translation>解码器缺少一个或多个通道</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal copy.cpp" line="281"/>
        <location filename="../pv/data/decodesignal.cpp" line="274"/>
        <source>No input data</source>
        <translation>无数据输入</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal copy.cpp" line="1527"/>
        <location filename="../pv/data/decodesignal.cpp" line="1518"/>
        <source>Decoder reported an error</source>
        <translation>解码器解码错误</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal copy.cpp" line="1685"/>
        <location filename="../pv/data/decodesignal.cpp" line="1671"/>
        <source>Failed to create decoder instance</source>
        <translation>无法创建的解码器实例</translation>
    </message>
</context>
<context>
    <name>pv::data::MathSignal</name>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="107"/>
        <source>Math%1</source>
        <translation>数学%1</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="306"/>
        <source>No expression defined, nothing to do</source>
        <translation>未定义表达式，不执行任何操作</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="345"/>
        <source>%1 at line %2, column %3: %4</source>
        <translation>%1 行 %2, 列 %3: %4</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="364"/>
        <location filename="../pv/data/mathsignal.cpp" line="536"/>
        <source>&quot;%1&quot; isn&apos;t a valid analog signal</source>
        <translation>&quot;%1&quot; 不是有效的模拟信号</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="374"/>
        <location filename="../pv/data/mathsignal.cpp" line="611"/>
        <source>No data will be generated as %1 must be enabled</source>
        <translation>不会生成数据，因为必须启用 %1</translation>
    </message>
</context>
<context>
    <name>pv::data::SignalBase</name>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="521"/>
        <source>Signal average</source>
        <translation>平均信号电平</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="522"/>
        <source>0.9V (for 1.8V CMOS)</source>
        <translation>0.9V（适用于 1.8V CMOS）</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="523"/>
        <source>1.8V (for 3.3V CMOS)</source>
        <translation>1.8V（适用于 3.3V CMOS）</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="524"/>
        <source>2.5V (for 5.0V CMOS)</source>
        <translation>2.5V（适用于 5.0V CMOS）</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="525"/>
        <source>1.5V (for TTL)</source>
        <translation>1.5V（适用于 TTL）</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="530"/>
        <source>Signal average +/- 15%</source>
        <translation>平均信号电平 +/- 15%</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="531"/>
        <source>0.3V/1.2V (for 1.8V CMOS)</source>
        <translation>0.3V/1.2V（适用于 1.8V CMOS）</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="532"/>
        <source>0.7V/2.5V (for 3.3V CMOS)</source>
        <translation>0.7V/2.5V（适用于 3.3V CMOS）</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="533"/>
        <source>1.3V/3.7V (for 5.0V CMOS)</source>
        <translation>1.3V/3.7V（适用于 5.0V CMOS）</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="534"/>
        <source>0.8V/2.0V (for TTL)</source>
        <translation>0.8V/2.0V（适用于 TTL）</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::Connect</name>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="58"/>
        <source>&amp;Scan for devices using driver above</source>
        <translation>使用上述驱动程序扫描设备</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="63"/>
        <source>Connect to Device</source>
        <translation>连接到设备</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="75"/>
        <source>Step 1: Choose the driver</source>
        <translation>步骤1：选择驱动程序</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="79"/>
        <source>&amp;USB</source>
        <translation>USB(&amp;U)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="80"/>
        <source>Serial &amp;Port</source>
        <translation>串行和端口</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="81"/>
        <source>&amp;TCP/IP</source>
        <translation>TCP/IP(&amp;T)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="116"/>
        <source>Protocol:</source>
        <translation>协议：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="134"/>
        <source>Step 2: Choose the interface</source>
        <translation>步骤2：选择接口</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="140"/>
        <source>Step 3: Scan for devices</source>
        <translation>步骤3：扫描设备</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="146"/>
        <source>Step 4: Select the device</source>
        <translation>步骤4：选择设备</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::DecoderChannelDialog</name>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="47"/>
        <source>Decoder Configuration</source>
        <translation>解码器配置</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="84"/>
        <source>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</source>
        <translation>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="97"/>
        <source>Channel Mapping</source>
        <translation>通道映射</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="102"/>
        <source>Decoder Options</source>
        <translation>解码器选项</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="113"/>
        <source>Apply &amp;&amp; Decode</source>
        <translation>应用并解码</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="114"/>
        <source>Cancel</source>
        <translation type="unfinished">取消</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="159"/>
        <source>-- Not assigned --</source>
        <translation>-- 未分配 --</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="195"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2)%3</source>
        <translation>&lt;b&gt;%1&lt;/b&gt;（%2）%3</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::IAPDialog</name>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="290"/>
        <source>FW Update</source>
        <translation>固件升级</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="97"/>
        <source>FW File:</source>
        <translation>固件文件:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="99"/>
        <source>Select firmware file in BIN or HEX format</source>
        <translation>选择BIN或HEX格式固件文件</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="101"/>
        <source>Browse</source>
        <translation>浏览</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="110"/>
        <source>Ready</source>
        <translation>就绪</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="115"/>
        <source>Update Log:</source>
        <translation>更新日志:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="124"/>
        <source>Start</source>
        <translation>开始</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="126"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="185"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="152"/>
        <source>Select firmware file</source>
        <translation>选择固件文件</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="154"/>
        <source>Firmware File (*.bin *.hex);;All Files (*.*)</source>
        <translation>固件文件 (*.bin *.hex);;所有文件 (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="168"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="173"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="168"/>
        <source>Please select a firmware file</source>
        <translation>请选择固件文件</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="173"/>
        <source>Firmware file does not exist</source>
        <translation>固件文件不存在</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="187"/>
        <source>Start firmware update...</source>
        <translation>开始固件更新...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="188"/>
        <source>Device index: %1</source>
        <translation>设备索引: %1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="189"/>
        <source>IAP VID: 0x%1, PID: 0x%2</source>
        <translation>IAP VID：0x%1，PID：0x%2</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="211"/>
        <source>Canceling firmware update...</source>
        <translation>正在取消升级...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="233"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="239"/>
        <source>Update Success</source>
        <translation>升级成功</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="240"/>
        <source>Firmware Update Success!</source>
        <translation>固件升级成功！</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="241"/>
        <source>Success</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="241"/>
        <source>Firmware Update Success!
Device will restart.</source>
        <translation>固件升级成功！
设备将重新启动。</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="243"/>
        <source>Update Failed</source>
        <translation>升级失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="244"/>
        <source>Firmware Update Failed!</source>
        <translation>固件升级失败！</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="245"/>
        <source>Fail</source>
        <translation>失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="245"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>固件升级失败！
请检查设备连接和固件文件。</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::IAPWorker</name>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="294"/>
        <source>Could not initialise hidapi</source>
        <translation>hidapi初始化失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="302"/>
        <source>IAP device not found (VID=0x%1, PID=0x%2)</source>
        <translation>未找到IAP设备 (VID=0x%1, PID=0x%2)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="308"/>
        <source>IAP device found</source>
        <translation>找到IAP设备</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="415"/>
        <source>Sending the enter-IAP-mode command...</source>
        <translation>发送IAP模式进入命令...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="423"/>
        <source>Failed to send the IAP command</source>
        <translation>发送IAP命令失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="457"/>
        <source>Erasing flash... address: 0x%1</source>
        <translation>擦除FLASH...地址: 0x%1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="458"/>
        <source>Erasing flash...</source>
        <translation>擦除FLASH...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="469"/>
        <source>Failed to send the erase command</source>
        <translation>发送擦除命令失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="475"/>
        <source>Erase response timed out</source>
        <translation>擦除响应超时</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="484"/>
        <source>Erase succeeded</source>
        <translation>擦除成功</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="490"/>
        <source>Programming flash... data length: %1 bytes</source>
        <translation>编程FLASH...数据长度: %1字节</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="491"/>
        <source>Programming flash...</source>
        <translation>编程FLASH...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="410"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="438"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="452"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="502"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="581"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="791"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="804"/>
        <source>Upgrade cancelled by the user</source>
        <translation>用户取消升级</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="342"/>
        <source>Write cancelled</source>
        <translation>写入被取消</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="358"/>
        <source>HID write failed</source>
        <translation>HID写入失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="364"/>
        <source>HID write timed out (%1 ms)</source>
        <translation>HID写入超时 (%1ms)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="381"/>
        <source>Read cancelled</source>
        <translation>读取被取消</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="397"/>
        <source>HID read error</source>
        <translation>HID读取错误</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="401"/>
        <source>HID read timed out (%1 ms)</source>
        <translation>HID读取超时 (%1ms)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="427"/>
        <source>IAP command sent, closing the CH375 device...</source>
        <translation>IAP命令已发送，关闭CH375设备...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="431"/>
        <source>CH375 device closed, waiting for the mode switch...</source>
        <translation>CH375设备已关闭，等待设备切换模式...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="480"/>
        <source>Erase failed, error code: %1</source>
        <translation>擦除失败，错误码: %1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="516"/>
        <source>Failed to write data, offset: %1</source>
        <translation>写入数据失败，偏移: %1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="527"/>
        <source>Program response timed out, offset: %1</source>
        <translation>编程响应超时，偏移: %1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="537"/>
        <source>Programming failed, error code: %1, offset: %2</source>
        <translation>编程失败，错误码: %1，偏移: %2</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="551"/>
        <source>Programming flash... %1%</source>
        <translation>编程FLASH... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="562"/>
        <source>Programming complete</source>
        <translation>编程完成</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="568"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="569"/>
        <source>Verifying flash...</source>
        <translation>校验FLASH...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="599"/>
        <source>Failed to send the verify command, offset: %1</source>
        <translation>发送校验命令失败，偏移: %1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="610"/>
        <source>Verify response timed out, offset: %1</source>
        <translation>校验响应超时，偏移: %1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="620"/>
        <source>Verify failed, error code: %1, address: 0x%2</source>
        <translation>校验失败，错误码: %1，地址: 0x%2</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="637"/>
        <source>Verifying flash... %1%</source>
        <translation>校验FLASH... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="640"/>
        <source>Verify succeeded</source>
        <translation>校验成功</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="646"/>
        <source>Sending the finish-upgrade command...</source>
        <translation>发送升级结束命令...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="647"/>
        <source>Finishing the upgrade...</source>
        <translation>完成升级...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="654"/>
        <source>Failed to send the finish command</source>
        <translation>发送结束命令失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="658"/>
        <source>Finish-upgrade command sent</source>
        <translation>升级结束命令已发送</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="663"/>
        <source>Device confirmed the upgrade</source>
        <translation>设备确认升级完成</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="666"/>
        <source>Device restarted (no response is expected)</source>
        <translation>设备已重启(无响应为正常情况)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="676"/>
        <source>Could not open the firmware file: %1</source>
        <translation>无法打开固件文件: %1</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="684"/>
        <source>HEX file detected, converting...</source>
        <translation>检测到HEX格式文件，正在转换...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="686"/>
        <source>HEX conversion failed</source>
        <translation>HEX转换失败</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="689"/>
        <source>HEX conversion succeeded, start address: 0x%1, size: %2 bytes</source>
        <translation>HEX转换成功，起始地址: 0x%1, 数据大小: %2字节</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="695"/>
        <source>Reading the BIN file, size: %1 bytes</source>
        <translation>读取BIN文件，数据大小: %1字节</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="765"/>
        <source>Reading the firmware file...</source>
        <translation>读取固件文件...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="773"/>
        <source>Firmware data is empty</source>
        <translation>固件数据为空</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="778"/>
        <source>Entering IAP mode...</source>
        <translation>进入IAP模式...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="784"/>
        <source>Looking for the HID device...</source>
        <translation>查找HID设备...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="815"/>
        <source>Waiting for the HID device... (%1/10)</source>
        <translation>等待HID设备... (%1/10)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="820"/>
        <source>Could not find the IAP HID device</source>
        <translation>无法找到IAP HID设备</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="856"/>
        <source>Upgrade complete</source>
        <translation>升级完成</translation>
    </message>
<message>
        <location filename="lu_src/pulseview/pv/dialogs/iapdialog.cpp" line="335"/>
        <source>HID write too long (%1 &gt; %2), rejected</source>
        <translation>HID 写入数据过长（%1 &gt; %2），已拒绝</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/dialogs/iapdialog.cpp" line="457"/>
        <source>IAP command sent, closing the libusb device...</source>
        <translation>IAP 命令已发送，正在关闭 libusb 设备...</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/dialogs/iapdialog.cpp" line="461"/>
        <source>libusb device closed, waiting for the mode switch...</source>
        <translation>libusb 设备已关闭，等待设备切换模式...</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/dialogs/iapdialog.cpp" line="478"/>
        <source>On this platform the IAP command can only be sent through libusb</source>
        <translation>当前平台仅支持通过 libusb 发送 IAP 命令</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/dialogs/iapdialog.cpp" line="829"/>
        <source>Device already in HID/IAP mode, skipping the mode switch</source>
        <translation>设备已处于 HID/IAP 模式，跳过模式切换</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/dialogs/iapdialog.cpp" line="830"/>
        <source>Device already in IAP mode...</source>
        <translation>设备已处于 IAP 模式...</translation>
    </message></context>
<context>
    <name>pv::dialogs::Settings</name>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="137"/>
        <location filename="../pv/dialogs/settings.cpp" line="216"/>
        <source>General</source>
        <translation>常规</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="146"/>
        <source>Views</source>
        <translation>显示</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="156"/>
        <location filename="../pv/dialogs/settings.cpp" line="409"/>
        <source>Decoders</source>
        <translation>解码器</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="166"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="175"/>
        <source>Logging</source>
        <translation>日志</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="239"/>
        <source>User interface language</source>
        <translation>语言</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="250"/>
        <source>User interface theme</source>
        <translation>主题</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="291"/>
        <source>User interface style</source>
        <translation>界面风格</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="284"/>
        <source>Follow system</source>
        <translation>跟随系统</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="285"/>
        <source>Light</source>
        <translation>浅色</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="286"/>
        <source>Dark</source>
        <translation>深色</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="258"/>
        <source>System Default</source>
        <translation>默认风格</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="271"/>
        <source>Qt widget style</source>
        <translation>QT 软件界面风格</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="280"/>
        <source>Save session &amp;setup along with .sr file</source>
        <translation>将会话设置与 .sr文件一起保存</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="299"/>
        <source>Trace View</source>
        <translation>采集视图</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="307"/>
        <source>Use colored trace &amp;background</source>
        <translation>使用彩色通道背景 ( &amp;B )</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="311"/>
        <source>Constantly perform &amp;zoom-to-fit during acquisition</source>
        <translation>采集数据过程中不断调整和缩放以适应窗口 ( &amp;Z )</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="315"/>
        <source>Perform a zoom-to-&amp;fit when acquisition stops</source>
        <translation>采集停止时执行调整和缩放以适应窗口 ( &amp;A )</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="319"/>
        <source>Show time zero at the &amp;trigger</source>
        <translation>在触发器处显示时间零点 ( &amp;T )</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="323"/>
        <source>Always keep &amp;newest samples at the right edge during capture</source>
        <translation>采集过程中始终跟踪右侧最新数据 ( &amp;N )</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="327"/>
        <source>Allow &amp;vertical dragging in the view area</source>
        <translation>允许在视图区域中垂直拖动</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="331"/>
        <source>Show data &amp;sampling points</source>
        <translation>显示数据采样点 ( &amp;S )</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="335"/>
        <source>Fill &amp;high areas of logic signals</source>
        <translation>填充逻辑信号高电平的区域 ( &amp;H )</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="342"/>
        <source>Color to fill high areas of logic signals with</source>
        <translation>填充逻辑信号高电平区域的颜色</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="346"/>
        <source>Show analog minor grid in addition to div grid</source>
        <translation>模拟通道除了div网格之外，还显示更多垂直细分网格</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="350"/>
        <source>Highlight mouse cursor using a vertical marker line</source>
        <translation>用竖线突出显示鼠标光标位置</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="354"/>
        <location filename="../pv/dialogs/settings.cpp" line="380"/>
        <location filename="../pv/dialogs/settings.cpp" line="389"/>
        <source> pixels</source>
        <translation>像素</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="359"/>
        <source>Maximum distance from edges before markers snap to them</source>
        <translation>光标吸附到边沿前的最大距离</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="366"/>
        <source>Color to fill cursor area with</source>
        <translation>填充光标范围内的颜色</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="369"/>
        <source>None</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="370"/>
        <source>Background</source>
        <translation>背景</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="371"/>
        <source>Dots</source>
        <translation>点</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="376"/>
        <source>Conversion threshold display mode (analog traces only)</source>
        <translation>转换阈值显示模式（仅限模拟通道）</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="385"/>
        <source>Default analog trace div height</source>
        <translation>模拟通道垂直高度</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="394"/>
        <source>Default logic trace height</source>
        <translation>逻辑通道垂直高度</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="417"/>
        <source>Allow configuration of &amp;initial signal state</source>
        <translation>允许配置初始信号状态</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="421"/>
        <source>Always show all &amp;rows, even if no annotation is visible</source>
        <translation>始终显示所有解码，即使未解码</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="429"/>
        <source>Annotation export format</source>
        <translation>解码数据导出格式</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="430"/>
        <source>%s = sample range; %d: decoder name; %r: row name; %c: class name</source>
        <translation>%s = 采样范围；%d：解码器名称；%r：行名称；%c：类名</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="433"/>
        <source>%1: longest annotation text; %a: all annotation texts; %q: use quotation marks</source>
        <translation>%1：最长的注释文本；%a：全部注释文本；%q：使用引号</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="453"/>
        <source>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</source>
        <translation>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="454"/>
        <source>GNU GPL, version 3 or later</source>
        <translation>GNU GPL 第 3 版或更高版本</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="465"/>
        <source>Versions, libraries and features:</source>
        <translation>版本、库和功能：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="472"/>
        <source>Firmware search paths:</source>
        <translation>固件搜索路径：</translation>
    </message>
    <message>
        <source>Protocol decoder search paths:</source>
        <translation>协议解码器搜索路径：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="487"/>
        <source>Supported hardware drivers:</source>
        <translation>支持的硬件驱动程序：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="494"/>
        <source>Supported input formats:</source>
        <translation>支持的输入格式：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="501"/>
        <source>Supported output formats:</source>
        <translation>支持的输出格式：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="509"/>
        <source>Supported protocol decoders:</source>
        <translation>支持的协议解码器：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="517"/>
        <source>Available Translations:</source>
        <translation>可用翻译：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="565"/>
        <source>Log level:</source>
        <translation>日志级别：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="570"/>
        <source> lines</source>
        <translation> 行</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="579"/>
        <source>Length of background buffer:</source>
        <translation>日志缓存长度：</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="585"/>
        <source>&amp;Save to File</source>
        <translation>保存到文件</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="592"/>
        <source>&amp;Pop out</source>
        <translation>独立弹出日志窗口</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="838"/>
        <source>Save Log</source>
        <translation>保存日志</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="838"/>
        <source>Log Files (*.txt *.log);;All Files (*)</source>
        <translation>日志文件 (*.txt *.log);;所有文件 (*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="850"/>
        <source>Success</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="850"/>
        <source>Log saved to %1.</source>
        <translation>日志已保存到 %1。</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="860"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="860"/>
        <source>File %1 could not be written to.</source>
        <translation>文件 %1 无法保存。</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="874"/>
        <source>%1 Log</source>
        <translation>%1 日志</translation>
    </message>
<message>
        <location filename="lu_src/pulseview/pv/dialogs/settings.cpp" line="489"/>
        <source>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;(Note: Set environment variable SIGROKDECODE_DIR to add a custom directory)&lt;/td&gt;&lt;/tr&gt;</source>
        <translation>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;（提示：设置环境变量 SIGROKDECODE_DIR 可添加自定义目录）&lt;/td&gt;&lt;/tr&gt;</translation>
    </message></context>
<context>
    <name>pv::dialogs::StoreProgress</name>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Saving...</source>
        <translation>保存中...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="89"/>
        <source>Failed to save session.</source>
        <translation>未能保存会话。</translation>
    </message>
</context>
<context>
    <name>pv::popups::Channels</name>
    <message>
        <location filename="../pv/popups/channels.cpp" line="953"/>
        <source>The current mode does not support opening this channel.</source>
        <translation>当前模式不支持该通道</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="348"/>
        <source>Logic Analyzer</source>
        <translation>逻辑分析仪</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="349"/>
        <source>ADC</source>
        <translation>ADC</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="352"/>
        <source>Device Mode</source>
        <translation>设备模式</translation>
    </message>
    <message>
        <source>Model:</source>
        <translation>型号:</translation>
    </message>
    <message>
        <source>Pre-trigger</source>
        <translation>预触发</translation>
    </message>
    <message>
        <source>Options</source>
        <translation>选项</translation>
    </message>
    <message>
        <source>Channels</source>
        <translation>通道</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="384"/>
        <source>ADC Channel 0</source>
        <translation>ADC通道0</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="385"/>
        <source>ADC Channel 1</source>
        <translation>ADC通道1</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="389"/>
        <source>ADC Channel</source>
        <translation>ADC通道</translation>
    </message>
    <message>
        <source>Input Level</source>
        <translation>输入电平</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="379"/>
        <source>Threshold Level</source>
        <translation>阈值电平</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="379"/>
        <source>Buffer Board</source>
        <translation>缓冲板</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="280"/>
        <location filename="../pv/popups/channels.cpp" line="1087"/>
        <source>Use 8 channels (Max 40MHz)</source>
        <translation>使用8通道(最高 40MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="281"/>
        <location filename="../pv/popups/channels.cpp" line="1088"/>
        <source>Use 16 channels (Max 20MHz)</source>
        <translation>使用16通道(最高 20MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="297"/>
        <location filename="../pv/popups/channels.cpp" line="1102"/>
        <source>Use 2 channels (Max 500MHz)</source>
        <translation>使用2通道(最高 500MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="298"/>
        <location filename="../pv/popups/channels.cpp" line="1103"/>
        <source>Use 4 channels (Max 250MHz)</source>
        <translation>使用4通道(最高 250MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="299"/>
        <location filename="../pv/popups/channels.cpp" line="1104"/>
        <source>Use 8 channels (Max 125MHz)</source>
        <translation>使用8通道(最高 125MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="300"/>
        <location filename="../pv/popups/channels.cpp" line="1105"/>
        <source>Use 16 channels (Max 62.5MHz)</source>
        <translation>使用16通道(最高 62.5MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="333"/>
        <source>Trigger Threshold(0~7 Channel)</source>
        <translation>触发阈值(0~7 通道)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="341"/>
        <source>Trigger Threshold(8~15 Channel)</source>
        <translation>触发阈值(8~15 通道)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="284"/>
        <location filename="../pv/popups/channels.cpp" line="1090"/>
        <source>Use 2 channels (Max 200MHz)</source>
        <translation type="unfinished">使用2通道 (最高 200MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="285"/>
        <location filename="../pv/popups/channels.cpp" line="1091"/>
        <source>Use 4 channels (Max 200MHz)</source>
        <translation type="unfinished">使用4通道 (最高 200MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="286"/>
        <location filename="../pv/popups/channels.cpp" line="1092"/>
        <source>Use 8 channels (Max 200MHz)</source>
        <translation type="unfinished">使用8通道 (最高 200MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="287"/>
        <location filename="../pv/popups/channels.cpp" line="1093"/>
        <source>Use 16 channels (Max 200MHz)</source>
        <translation type="unfinished">使用16通道 (最高 200MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="291"/>
        <location filename="../pv/popups/channels.cpp" line="1096"/>
        <source>Use 2 channels (Max 1GHz)</source>
        <translation>使用2通道 (最高 1GHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="292"/>
        <location filename="../pv/popups/channels.cpp" line="1097"/>
        <source>Use 4 channels (Max 500MHz)</source>
        <translation>使用4通道 (最高 500MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="293"/>
        <location filename="../pv/popups/channels.cpp" line="1098"/>
        <source>Use 8 channels (Max 250MHz)</source>
        <translation>使用8通道 (最高 250MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="294"/>
        <location filename="../pv/popups/channels.cpp" line="1099"/>
        <source>Use 16 channels (Max 125MHz)</source>
        <translation>使用16通道 (最高 125MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="395"/>
        <source>8-bit</source>
        <translation>8 位</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="396"/>
        <source>10-bit</source>
        <translation>10 位</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="400"/>
        <source>ADC Precision</source>
        <translation>ADC精度</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="952"/>
        <source>Information</source>
        <translation>信息</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="954"/>
        <source>Confirm</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="278"/>
        <location filename="../pv/popups/channels.cpp" line="1085"/>
        <source>Use 2 channels (Max 40MHz)</source>
        <translation>使用2通道 (最高 40MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="279"/>
        <location filename="../pv/popups/channels.cpp" line="1086"/>
        <source>Use 4 channels (Max 40MHz)</source>
        <translation>使用4通道 (最高 40MHz)</translation>
    </message>
<message>
        <location filename="lu_src/pulseview/pv/popups/channels.cpp" line="189"/>
        <source>SN:</source>
        <translation>序列号：</translation>
    </message></context>
<context>
    <name>pv::prop::Bool</name>
    <message>
        <location filename="../pv/prop/bool.cpp" line="51"/>
        <location filename="../pv/prop/bool.cpp" line="82"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>查询配置 &apos;%1&apos;: %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Double</name>
    <message>
        <location filename="../pv/prop/double.cpp" line="65"/>
        <location filename="../pv/prop/double.cpp" line="96"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>查询配置 &apos;%1&apos;: %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Enum</name>
    <message>
        <location filename="../pv/prop/enum.cpp" line="113"/>
        <location filename="../pv/prop/enum.cpp" line="176"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>查询配置 &apos;%1&apos;: %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Int</name>
    <message>
        <location filename="../pv/prop/int.cpp" line="63"/>
        <location filename="../pv/prop/int.cpp" line="127"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>查询配置 &apos;%1&apos;: %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::String</name>
    <message>
        <location filename="../pv/prop/string.cpp" line="59"/>
        <location filename="../pv/prop/string.cpp" line="84"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>查询配置 &apos;%1&apos;: %2</translation>
    </message>
</context>
<context>
    <name>pv::subwindows::decoder_selector::DecoderCollectionModel</name>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="42"/>
        <source>Decoder</source>
        <translation>解码器</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="43"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="44"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="51"/>
        <source>All Decoders</source>
        <translation>所有解码器</translation>
    </message>
</context>
<context>
    <name>pv::subwindows::decoder_selector::SubWindow</name>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="51"/>
        <source>Select a decoder to see its description here.</source>
        <translation>选择解码器以在此处查看其说明。</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="253"/>
        <source>, %1</source>
        <translation>，%1</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="270"/>
        <source>&lt;p align=&apos;right&apos;&gt;Tags: %1&lt;/p&gt;</source>
        <translation>&lt;p align='right'&gt;标签：%1&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="309"/>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="378"/>
        <source>Protocol decoder &lt;b&gt;%1&lt;/b&gt; requires input type &lt;b&gt;%2&lt;/b&gt; which several decoders provide.&lt;br&gt;Choose which one to use:&lt;br&gt;</source>
        <translation>协议解码器 &lt;b&gt;%1&lt;/b&gt; 有不同的&lt;b&gt;%2&lt;/b&gt; 协议类型。&lt;br&gt;选择要使用的：&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="317"/>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="386"/>
        <source>Choose Decoder</source>
        <translation>选择解码器</translation>
    </message>
</context>
<context>
    <name>pv::toolbars</name>
</context>
<context>
    <name>pv::toolbars::MainBar</name>
    <message>
        <source>Warning</source>
        <translation>警告</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>设置</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="204"/>
        <source>&amp;Open...</source>
        <translation>打开... ( &amp;O )</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="338"/>
        <source>Open</source>
        <translation>打开</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="211"/>
        <source>Restore Session Setu&amp;p...</source>
        <translation>从文件打开会话设置...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="222"/>
        <source>Save &amp;As...</source>
        <translation>另存为... ( &amp;A )</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="228"/>
        <source>Save Selected &amp;Range As...</source>
        <translation>将所选范围另存为... ( &amp;R )</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="235"/>
        <source>Save Session Setu&amp;p...</source>
        <translation>保存会话设置...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="241"/>
        <source>&amp;Export</source>
        <translation>导出</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="247"/>
        <source>&amp;Import</source>
        <translation>导入</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="250"/>
        <source>&amp;Connect to Device...</source>
        <translation>连接到设备...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="265"/>
        <source>Binary Decoder Output View</source>
        <translation>二进制解码器</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="267"/>
        <source>Tabular Decoder Output View</source>
        <translation>表格解码器</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="323"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1861"/>
        <source>Start</source>
        <translation>开始</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="367"/>
        <source>Add protocol decoder</source>
        <translation>添加协议解码器</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="398"/>
        <source>Configure Channels</source>
        <translation>配置</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="402"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="412"/>
        <source>Option</source>
        <translation>配置</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="603"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1055"/>
        <source>Failed to get sample rate list:</source>
        <translation>无法获取采样率列表：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1684"/>
        <source>Failed to set CH32H417 mode on device switch:</source>
        <translation>设备切换时设置 CH32H417 模式失败：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1886"/>
        <source>sigrok Sessions (*.sr);;All Files (*)</source>
        <translation>sigrok 会话 (*.sr);;所有文件 (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1928"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1944"/>
        <source>PulseView Session Setups (*.pvs);;All Files (*)</source>
        <translation>PulseView 会话设置 (*.pvs);;所有文件 (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2017"/>
        <source>Total Sampling Count: %1</source>
        <translation>总采样数：%1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="198"/>
        <source>Decoding Result</source>
        <translation>解码结果</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="215"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="358"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="278"/>
        <source>Single</source>
        <translation>单次</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="279"/>
        <source>Repeat</source>
        <translation>重复</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="288"/>
        <source>Version Info</source>
        <translation>版本信息</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="290"/>
        <source>FW Update</source>
        <translation>固件升级</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="311"/>
        <source>Help</source>
        <translation>帮助</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="331"/>
        <source>Model</source>
        <translation>模式</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="370"/>
        <source>Decoder</source>
        <translation>解码器</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="759"/>
        <source>Failed to get sample limit list:</source>
        <translation>无法获取采样限制列表：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="962"/>
        <source>Failed to configure pattern mode:</source>
        <translation>配置工作模式失败：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="983"/>
        <source>Failed to configure ADC channel:</source>
        <translation>配置 ADC 通道失败：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1006"/>
        <source>Failed to configure ADC precision:</source>
        <translation>配置 ADC 精度失败：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1356"/>
        <source>Failed to configure samplerate:</source>
        <translation>无法配置采样率：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1482"/>
        <source>Failed to configure sample count:</source>
        <translation>无法配置采样计数：</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1522"/>
        <source>Missing Cursors</source>
        <translation>缺少光标</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1522"/>
        <source>You need to set the cursors before you can save the data enclosed by them to a session file (e.g. using the Show Cursors button).</source>
        <translation>您需要设置光标，然后才能将光标范围内的数据保存到会话文件中（例如，使用“显示光标”按钮）。</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1541"/>
        <source>Invalid Range</source>
        <translation>无效的范围</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1541"/>
        <source>The cursors don&apos;t define a valid range of samples.</source>
        <translation>光标没有定义有效的采样范围。</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1555"/>
        <source>%1 files </source>
        <translation>%1 个文件 </translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1560"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1611"/>
        <source>All Files</source>
        <translation>所有文件</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1564"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1928"/>
        <source>Save File</source>
        <translation>保存文件</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1577"/>
        <source>Export %1</source>
        <translation>导出 %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1609"/>
        <source>%1 files</source>
        <translation>%1 个文件</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1619"/>
        <source>Import File</source>
        <translation>导入文件</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1629"/>
        <source>Import %1</source>
        <translation>导入 %1</translation>
    </message>
    <message>
        <source>&lt;b&gt;Software Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;软件版本：&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <source>&lt;b&gt;Firmware Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;固件版本：&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2141"/>
        <source>The current device does not support firmware upgrade.
Please connect the CH32H417 device and try again.</source>
        <translation>当前设备不支持固件升级。
请连接CH32H417设备后重试。</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2150"/>
        <source>Select firmware file</source>
        <translation>选择固件文件</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2152"/>
        <source>Firmware File (*.bin *.BIN *.hex *.HEX);;All Files (*.*)</source>
        <translation>固件文件 (*.bin *.BIN *.hex *.HEX);;所有文件 (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2162"/>
        <source>Firmware update in progress, do not disconnect the USB connection</source>
        <translation>正在升级固件，请勿断开USB连接！</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2162"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2163"/>
        <source>Firmware Update</source>
        <translation>固件升级</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2218"/>
        <source>Information</source>
        <translation>提示信息</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2254"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2292"/>
        <source>Update Success</source>
        <translation>更新成功</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2293"/>
        <source>Firmware Update Success!
Device already restart.</source>
        <translation>固件升级成功！
设备已重新启动。</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2188"/>
        <source>Update Failed</source>
        <translation>升级失败</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2189"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>固件升级失败！
请检查设备连接和固件文件。</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2233"/>
        <source>Open Failed</source>
        <translation>打开失败</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2234"/>
        <source>The PDF file could not be opened. Make sure a PDF reader is installed.</source>
        <translation>无法打开PDF文件，请确认系统已安装PDF阅读器。</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1497"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2125"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2143"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2221"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2264"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2294"/>
        <source>Confirm</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1866"/>
        <source>Stop</source>
        <translation type="unfinished">停止</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1886"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1944"/>
        <source>Open File</source>
        <translation>打开文件</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2044"/>
        <source>Device Options</source>
        <translation>设备选项</translation>
    </message>
<message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="163"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1088"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1107"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1145"/>
        <source>Demo</source>
        <translation>演示</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="164"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1089"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1108"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1146"/>
        <source>Device Status: Demo</source>
        <translation>设备状态：演示</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="727"/>
        <source>Failed to get sample rate value:</source>
        <translation>获取采样率数值失败：</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="982"/>
        <source>Failed to configure pre-trigger:</source>
        <translation>配置预触发失败：</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1095"/>
        <source>File</source>
        <translation>文件</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1096"/>
        <source>Device Status: File</source>
        <translation>设备状态：文件</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1132"/>
        <source>USB2.0</source>
        <translation>USB2.0</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1133"/>
        <source>Device Status: USB 2.0</source>
        <translation>设备状态：USB 2.0</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1136"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1141"/>
        <source>USB3.0</source>
        <translation>USB3.0</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1137"/>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1142"/>
        <source>Device Status: USB 3.0</source>
        <translation>设备状态：USB 3.0</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="1641"/>
        <source>Error</source>
        <translation>错误</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="2342"/>
        <source>&lt;b&gt;FPGA Version:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;FPGA 版本：&lt;/b&gt;%1</translation>
    </message>
<message>
        <location filename="lu_src/pulseview/pv/toolbars/mainbar.cpp" line="2483"/>
        <source>PDF file not found:
%1

Make sure the doc directory and the user manual are present.</source>
        <translation>找不到 PDF 文件：
%1

请确认 doc 目录和用户手册文件是否存在。</translation>
    </message></context>
<context>
    <name>pv::views</name>
</context>
<context>
    <name>pv::views::decoder_binary::View</name>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="90"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="91"/>
        <source>Decoder:</source>
        <translation>解码器：</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="99"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="99"/>
        <source>Show data as</source>
        <translation>数据显示格式</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="107"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="107"/>
        <source>Hexdump</source>
        <translation>十六进制转储</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="121"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="121"/>
        <source>&amp;Save...</source>
        <translation>保存... ( &amp;S )</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="351"/>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="391"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="343"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="383"/>
        <source>Save Binary Data</source>
        <translation>保存二进制数据</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="351"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="343"/>
        <source>Binary Data Files (*.bin);;All Files (*)</source>
        <translation>二进制数据文件 (*.bin);;所有文件 (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="370"/>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="422"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="362"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="414"/>
        <source>File %1 could not be written to.</source>
        <translation>文件 %1 无法保存。</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="370"/>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="422"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="362"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="414"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view copy.cpp" line="391"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="383"/>
        <source>Hex Dumps (*.txt);;All Files (*)</source>
        <translation>十六进制转储文件 (*.txt);;所有文件 (*)</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::AnnotationCollectionModel</name>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="58"/>
        <source>Sample</source>
        <translation>起始采样点</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="59"/>
        <source>Time</source>
        <translation>时间</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="60"/>
        <source>Decoder</source>
        <translation>解码器</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="61"/>
        <source>Ann Row</source>
        <translation>类别</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="62"/>
        <source>Ann Class</source>
        <translation>子类别</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="63"/>
        <source>Value</source>
        <translation>值</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="66"/>
        <source>End Sample</source>
        <translation>结束采样点</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="85"/>
        <source>s</source>
        <translation>s</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="85"/>
        <source>sa</source>
        <translation>sa</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::View</name>
    <message>
        <location filename="../pv/views/tabular_decoder/view copy.cpp" line="177"/>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="183"/>
        <source>Decoder:</source>
        <translation>解码器：</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view copy.cpp" line="201"/>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="207"/>
        <source>Hide Hidden Rows/Classes</source>
        <translation>隐藏界面外的解码结果</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view copy.cpp" line="205"/>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="211"/>
        <source>&amp;Save...</source>
        <translation>保存... ( &amp;S )</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view copy.cpp" line="440"/>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="443"/>
        <source>Save Annotations as CSV</source>
        <translation>将解码数据另存为CSV</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view copy.cpp" line="440"/>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="443"/>
        <source>CSV Files (*.csv);;Text Files (*.txt);;All Files (*)</source>
        <translation>CSV文件 (*.csv);;文本文件 (*.txt);;所有文件 (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view copy.cpp" line="507"/>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="510"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view copy.cpp" line="507"/>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="510"/>
        <source>File %1 could not be written to.</source>
        <translation>文件 %1 无法保存。</translation>
    </message>
</context>
<context>
    <name>pv::views::trace</name>
</context>
<context>
    <name>pv::views::trace::AnalogSignal</name>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="889"/>
        <source>Number of pos vertical divs</source>
        <translation>正区间div垂直细分网格数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="897"/>
        <source>Number of neg vertical divs</source>
        <translation>负区间div垂直细分网格数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="902"/>
        <source> pixels</source>
        <translation> 像素</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="906"/>
        <source>Div height</source>
        <translation>Div高度</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="925"/>
        <source>V/div</source>
        <translation>V/格</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="929"/>
        <source>Vertical resolution</source>
        <translation>垂直分辨率</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="938"/>
        <source>Autoranging</source>
        <translation>自动量程</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="943"/>
        <source>none</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="945"/>
        <source>to logic via threshold</source>
        <translation>通过阈值转换为逻辑</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="947"/>
        <source>to logic via schmitt-trigger</source>
        <translation>通过施密特触发器转换为逻辑</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="953"/>
        <source>Conversion</source>
        <translation>转换</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="962"/>
        <source>Conversion threshold(s)</source>
        <translation>转换阈值</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="972"/>
        <source>analog</source>
        <translation>模拟</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="973"/>
        <source>converted</source>
        <translation>数字</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="974"/>
        <source>analog+converted</source>
        <translation>模拟+数字</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="979"/>
        <source>Show traces for</source>
        <translation>显示采集</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Cursor</name>
    <message>
        <location filename="../pv/views/trace/cursor.cpp" line="105"/>
        <source>Disable snapping</source>
        <translation>禁用捕捉</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::CursorPair</name>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="130"/>
        <source>Display interval</source>
        <translation>显示间隔</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="142"/>
        <source>Display frequency</source>
        <translation>显示频率</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="154"/>
        <source>Display samples</source>
        <translation>显示采样</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::DecodeTrace</name>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="462"/>
        <source>&lt;p&gt;&lt;i&gt;No decoders in the stack&lt;/i&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;i&gt;堆叠中没有解码器&lt;/i&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="473"/>
        <source>&lt;i&gt;* Required channels&lt;/i&gt;</source>
        <translation>&lt;i&gt;* 所需通道&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="477"/>
        <source>Stack Decoder</source>
        <translation>堆叠解码器</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="478"/>
        <source>Stack a higher-level decoder on top of this one</source>
        <translation>在这之上面堆叠一个更高级别的解码器</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="580"/>
        <source>Delete</source>
        <translation>删除</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="622"/>
        <source>Resume decoding</source>
        <translation>继续解码</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="629"/>
        <source>Pause decoding</source>
        <translation>暂停解码</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="637"/>
        <source>Copy annotation text to clipboard</source>
        <translation>将解码数据复制到剪贴板</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="646"/>
        <source>Export all annotations</source>
        <translation>导出所有解码数据</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="653"/>
        <source>Export all annotations for this row</source>
        <translation>导出此行的解码数据</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="662"/>
        <source>Export all annotations, starting here</source>
        <translation>导出从这以后所有的解码数据</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="669"/>
        <source>Export annotations for this row, starting here</source>
        <translation>导出从这以后此行的解码数据</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="678"/>
        <source>Export all annotations within cursor range</source>
        <translation>导出光标范围内所有的解码数据</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="685"/>
        <source>Export annotations for this row within cursor range</source>
        <translation>导出光标范围内此行的解码数据</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1177"/>
        <source>%1:
%2</source>
        <translation>%1：
%2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1221"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2) %3</source>
        <translation>&lt;b&gt;%1&lt;/b&gt;（%2）%3</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1290"/>
        <source>Export annotations</source>
        <translation>导出解码数据</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1290"/>
        <source>Text Files (*.txt);;All Files (*)</source>
        <translation>文本文件 (*.txt);;所有文件 (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1355"/>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1364"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1355"/>
        <source>File %1 could not be written to.</source>
        <translation>文件 %1 无法保存。</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1417"/>
        <source>Show this row</source>
        <translation>显示此行</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1428"/>
        <source>Show All</source>
        <translation>全部显示</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1436"/>
        <source>Hide All</source>
        <translation>全部隐藏</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Flag</name>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="146"/>
        <source>Text</source>
        <translation>文本</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="155"/>
        <source>Delete</source>
        <translation>删除</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="160"/>
        <source>Disable snapping</source>
        <translation>禁用捕捉</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Header</name>
    <message>
        <location filename="../pv/views/trace/header.cpp" line="138"/>
        <source>Group</source>
        <translation>组</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::LogicSignal</name>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="487"/>
        <source>No trigger</source>
        <translation>无触发</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="494"/>
        <source>Trigger on rising edge</source>
        <translation>上升沿触发</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="501"/>
        <source>Trigger on high level</source>
        <translation>高电平触发</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="508"/>
        <source>Trigger on falling edge</source>
        <translation>下降沿触发</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="515"/>
        <source>Trigger on low level</source>
        <translation>低电平触发</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="522"/>
        <source>Trigger on rising or falling edge</source>
        <translation>上升沿或下降沿触发</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="610"/>
        <source> pixels</source>
        <translation> 像素</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="614"/>
        <source>Trace height</source>
        <translation>通道高度</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="638"/>
        <source>Trigger</source>
        <translation>触发</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::MathEditDialog</name>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="90"/>
        <source>Math Expression Editor</source>
        <translation>Math 表达式编辑器</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="95"/>
        <source>Inputs:</source>
        <translation>输入：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="101"/>
        <source>Variables:</source>
        <translation>变量：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="103"/>
        <source>Basic operators:</source>
        <translation>基本运算符：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="110"/>
        <source>Assignments:</source>
        <translation>赋值：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="120"/>
        <source>General purpose functions:</source>
        <translation>通用函数：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="121"/>
        <source>abs(x)		Absolute value of x</source>
        <translation>abs(x)		x 的绝对值</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="122"/>
        <source>avg(x, y, ...)	Average of all input values</source>
        <translation>avg(x, y, ...)	Average of all input values</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="123"/>
        <source>ceil(x)		Smallest integer that is greater than or equal to x</source>
        <translation>ceil(x)		大于或等于 x 的最小整数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="124"/>
        <source>clamp(lb, x, ub)	Clamp x in range between lb and ub, where lb &lt; ub</source>
        <translation>clamp(lb, x, ub)	Clamp x in range between lb and ub, where lb &lt; ub</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="125"/>
        <source>equal(x, y)	Equality test between x and y using normalised epsilon</source>
        <translation>equal(x, y)	Equality test between x and y using normalised epsilon</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="126"/>
        <source>erf(x)		Error function of x</source>
        <translation>erf(x)		x 的误差函数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="127"/>
        <source>erfc(x)		Complimentary error function of x</source>
        <translation>erfc(x)		x 的互补误差函数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="128"/>
        <source>exp(x)		e to the power of x</source>
        <translation>exp(x)		e 的 x 次幂</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="129"/>
        <source>expm1(x)	e to the power of x minus 1, where x is very small.</source>
        <translation>expm1(x)	e 的 x 次幂减 1（适用于 x 极小的情况）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="130"/>
        <source>floor(x)		Largest integer that is less than or equal to x</source>
        <translation>floor(x)		小于或等于 x 的最大整数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="131"/>
        <source>frac(x)		Fractional portion of x</source>
        <translation>frac(x)		x 的小数部分</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="132"/>
        <source>hypot(x)		Hypotenuse of x and y (i.e. sqrt(x*x + y*y))</source>
        <translation>hypot(x)		x 与 y 的斜边（即 sqrt(x*x + y*y)）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="133"/>
        <source>iclamp(lb, x, ub)	Inverse-clamp x outside of the range lb and ub, where lb &lt; ub.
		If x is within the range it will snap to the closest bound</source>
        <translation>iclamp(lb, x, ub)	Inverse-clamp x outside of the range lb and ub, where lb &lt; ub.
		If x is within the range it will snap to the closest bound</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="134"/>
        <source>inrange(lb, x, ub)	In-range returns true when x is within the range lb and ub, where lb &lt; ub.</source>
        <translation>inrange(lb, x, ub)	In-range returns true when x is within the range lb and ub, where lb &lt; ub.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="135"/>
        <source>log(x)		Natural logarithm of x</source>
        <translation>log(x)		x 的自然对数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="136"/>
        <source>log10(x)		Base 10 logarithm of x</source>
        <translation>log10(x)		x 以 10 为底的对数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="140"/>
        <source>log1p(x)		Natural logarithm of 1 + x, where x is very small</source>
        <translation>log1p(x)		1 + x 的自然对数（适用于 x 极小的情况）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="141"/>
        <source>log2(x)		Base 2 logarithm of x</source>
        <translation>log2(x)		x 以 2 为底的对数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="142"/>
        <source>logn(x)		Base N logarithm of x, where n is a positive integer</source>
        <translation>logn(x)		x 以 N 为底的对数（n 为正整数）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="143"/>
        <source>max(x, y, ...)	Largest value of all the inputs</source>
        <translation>max(x, y, ...)	Largest value of all the inputs</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="144"/>
        <source>min(x, y, ...)	Smallest value of all the inputs</source>
        <translation>min(x, y, ...)	Smallest value of all the inputs</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="145"/>
        <source>mul(x, y, ...)	Product of all the inputs</source>
        <translation>mul(x, y, ...)	Product of all the inputs</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="146"/>
        <source>ncdf(x)		Normal cumulative distribution function</source>
        <translation>ncdf(x)		正态累积分布函数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="147"/>
        <source>nequal(x, y)	Not-equal test between x and y using normalised epsilon</source>
        <translation>nequal(x, y)	Not-equal test between x and y using normalised epsilon</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="148"/>
        <source>pow(x, y)	x to the power of y</source>
        <translation>pow(x, y)	x to the power of y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="149"/>
        <source>root(x, n)	Nth-Root of x, where n is a positive integer</source>
        <translation>root(x, n)	Nth-Root of x, where n is a positive integer</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="150"/>
        <source>round(x)		Round x to the nearest integer</source>
        <translation>round(x)		将 x 四舍五入到最近的整数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="151"/>
        <source>roundn(x, n)	Round x to n decimal places, where n &gt; 0 and is an integer</source>
        <translation>roundn(x, n)	Round x to n decimal places, where n &gt; 0 and is an integer</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="152"/>
        <source>sgn(x)		Sign of x; -1 if x &lt; 0, +1 if x &gt; 0, else zero</source>
        <translation>sgn(x)		x 的符号；x &lt; 0 为 -1，x &gt; 0 为 +1，否则为 0</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="153"/>
        <source>sqrt(x)		Square root of x, where x &gt;= 0</source>
        <translation>sqrt(x)		x 的平方根（x &gt;= 0）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="154"/>
        <source>sum(x, y, ..,)	Sum of all the inputs</source>
        <translation>sum(x, y, ..,)	Sum of all the inputs</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="155"/>
        <source>swap(x, y)	Swap the values of the variables x and y and return the current value of y</source>
        <translation>swap(x, y)	Swap the values of the variables x and y and return the current value of y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="156"/>
        <source>trunc(x)		Integer portion of x</source>
        <translation>trunc(x)		Integer portion of x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="160"/>
        <source>Trigonometry functions:</source>
        <translation>三角函数：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="161"/>
        <source>acos(x)		Arc cosine of x expressed in radians. Interval [-1,+1]</source>
        <translation>acos(x)		x 的反余弦（弧度）。区间 [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="162"/>
        <source>acosh(x)		Inverse hyperbolic cosine of x expressed in radians</source>
        <translation>acosh(x)		x 的反双曲余弦（弧度）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="163"/>
        <source>asin(x)		Arc sine of x expressed in radians. Interval [-1,+1]</source>
        <translation>asin(x)		x 的反正弦（弧度）。区间 [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="164"/>
        <source>asinh(x)		Inverse hyperbolic sine of x expressed in radians</source>
        <translation>asinh(x)		x 的反双曲正弦（弧度）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="165"/>
        <source>atan(x)		Arc tangent of x expressed in radians. Interval [-1,+1]</source>
        <translation>atan(x)		x 的反正切（弧度）。区间 [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="166"/>
        <source>atan2(x, y)	Arc tangent of (x / y) expressed in radians. [-pi,+pi]  </source>
        <translation>atan2(x, y)	Arc tangent of (x / y) expressed in radians. [-pi,+pi]  </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="167"/>
        <source>atanh(x)		Inverse hyperbolic tangent of x expressed in radians</source>
        <translation>atanh(x)		x 的反双曲正切（弧度）</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="168"/>
        <source>cos(x)		Cosine of x</source>
        <translation>cos(x)		x 的余弦</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="169"/>
        <source>cosh(x)		Hyperbolic cosine of x</source>
        <translation>cosh(x)		x 的双曲余弦</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="170"/>
        <source>cot(x)		Cotangent of x</source>
        <translation>cot(x)		x 的余切</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="171"/>
        <source>csc(x)		Cosectant of x</source>
        <translation>csc(x)		x 的余割</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="172"/>
        <source>sec(x)		Secant of x</source>
        <translation>sec(x)		x 的正割</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="173"/>
        <source>sin(x)		Sine of x</source>
        <translation>sin(x)		x 的正弦</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="174"/>
        <source>sinc(x)		Sine cardinal of x</source>
        <translation>sinc(x)		x 的 sinc 函数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="175"/>
        <source>sinh(x)		Hyperbolic sine of x</source>
        <translation>sinh(x)		x 的双曲正弦</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="176"/>
        <source>tan(x)		Tangent of x</source>
        <translation>tan(x)		Tangent of x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="177"/>
        <source>tanh(x)		Hyperbolic tangent of x</source>
        <translation>tanh(x)		Hyperbolic tangent of x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="178"/>
        <source>deg2rad(x)	Convert x from degrees to radians</source>
        <translation>deg2rad(x)	将 x 从角度转换为弧度</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="179"/>
        <source>deg2grad(x)	Convert x from degrees to gradians</source>
        <translation>deg2grad(x)	将 x 从角度转换为百分度</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="180"/>
        <source>rad2deg(x)	Convert x from radians to degrees</source>
        <translation>rad2deg(x)	将 x 从弧度转换为角度</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="181"/>
        <source>grad2deg(x)	Convert x from gradians to degrees</source>
        <translation>grad2deg(x)	将 x 从百分度转换为角度</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="185"/>
        <source>Logic operators:</source>
        <translation>逻辑运算符：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="202"/>
        <source>Comparisons:</source>
        <translation>比较运算：</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="203"/>
        <source>x = y or x == y	True only if x is strictly equal to y</source>
        <translation>x = y or x == y	True only if x is strictly equal to y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="204"/>
        <source>x &lt;&gt; y or x != y	True only if x does not equal y</source>
        <translation>x &lt;&gt; y or x != y	True only if x does not equal y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="205"/>
        <source>x &lt; y		True only if x is less than y</source>
        <translation>x &lt; y		True only if x is less than y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="206"/>
        <source>x &lt;= y		True only if x is less than or equal to y</source>
        <translation>x &lt;= y		True only if x is less than or equal to y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="207"/>
        <source>x &gt; y		True only if x is greater than y</source>
        <translation>x &gt; y		True only if x is greater than y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="208"/>
        <source>x &gt;= y		True only if x is greater than or equal to y</source>
        <translation>x &gt;= y		True only if x is greater than or equal to y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="209"/>
        <source>Flow control:</source>
        <translation>Flow control:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="210"/>
        <source>{ ... }		Beginning and end of instruction block</source>
        <translation>{ ... }		Beginning and end of instruction block</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="211"/>
        <source>if (x, y, z)	If x is true then return y else return z
if (x) y;			variant without implied else
if (x) { y };		variant with an instruction block
if (x) y; else z;		variant with explicit else
if (x) { y } else { z };	variant with instruction blocks</source>
        <translation>if (x, y, z)	If x is true then return y else return z
if (x) y;			variant without implied else
if (x) { y };		variant with an instruction block
if (x) y; else z;		variant with explicit else
if (x) { y } else { z };	variant with instruction blocks</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="212"/>
        <source>x ? y : z	Ternary operator, equivalent to &apos;if (x, y, z)&apos;</source>
        <translation>x ? y : z	Ternary operator, equivalent to 'if (x, y, z)'</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="213"/>
        <source>switch {		The first true case condition that is encountered will
 case x &gt; 1: a;	determine the result of the switch. If none of the case
 case x &lt; 1: b;	conditions hold true, the default action is used
 default:     c;	to determine the return value
}</source>
        <translation>switch {		The first true case condition that is encountered will
 case x &gt; 1: a;	determine the result of the switch. If none of the case
 case x &lt; 1: b;	conditions hold true, the default action is used
 default:     c;	to determine the return value
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="217"/>
        <source>while (conditon) {	Evaluates expression repeatedly as long as condition is true,
 expression;	returning the last value of expression
}</source>
        <translation>while (conditon) {	Evaluates expression repeatedly as long as condition is true,
 expression;	returning the last value of expression
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="218"/>
        <source>repeat		Evalues expression repeatedly as long as condition is false,
 expression;	returning the last value of expression
until (condition)
</source>
        <translation>repeat		Evalues expression repeatedly as long as condition is false,
 expression;	returning the last value of expression
until (condition)
</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="219"/>
        <source>for (var x := 0; condition; x += 1) {	Repeatedly evaluates expression while the condition is true,
 expression			while evaluating the &apos;increment&apos; expression on each loop
}</source>
        <translation>for (var x := 0; condition; x += 1) {	Repeatedly evaluates expression while the condition is true,
 expression			while evaluating the 'increment' expression on each loop
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="220"/>
        <source>break		Terminates the execution of the nearest enclosed loop, returning NaN</source>
        <translation>break		终止最近一层循环的执行，返回 NaN</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="221"/>
        <source>break[x]		Terminates the execution of the nearest enclosed loop, returning x</source>
        <translation>break[x]		终止最近一层循环的执行，返回 x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="222"/>
        <source>continue		Interrupts loop execution and resumes with the next loop iteration</source>
        <translation>continue		中断本次循环，继续下一次迭代</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="223"/>
        <source>return[x]		Returns immediately from within the current expression, returning x</source>
        <translation>return[x]		立即从当前表达式返回，返回值为 x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="224"/>
        <source>~(expr; expr; ...)	Evaluates each sub-expression and returns the value of the last one
~{expr; expr; ...}</source>
        <translation>~(expr; expr; ...)	Evaluates each sub-expression and returns the value of the last one
~{expr; expr; ...}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="236"/>
        <source>Copy to expression</source>
        <translation>复制到表达式</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="249"/>
        <source>Basics</source>
        <translation>基础</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="250"/>
        <source>Functions 1</source>
        <translation>函数 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="251"/>
        <source>Functions 2</source>
        <translation>函数 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="252"/>
        <source>Trigonometry</source>
        <translation>三角函数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="253"/>
        <source>Logic</source>
        <translation type="unfinished">逻辑</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="254"/>
        <source>Flow Control 1</source>
        <translation>流程控制 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="255"/>
        <source>Flow Control 2</source>
        <translation>流程控制 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="256"/>
        <source>Examples</source>
        <translation>示例</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::MathSignal</name>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="315"/>
        <source>Expression</source>
        <translation>表达式</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="319"/>
        <location filename="../pv/views/trace/mathsignal.cpp" line="329"/>
        <source>same as session</source>
        <translation>与会话相同</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="320"/>
        <location filename="../pv/views/trace/mathsignal.cpp" line="330"/>
        <source>100</source>
        <translation>100</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="321"/>
        <location filename="../pv/views/trace/mathsignal.cpp" line="331"/>
        <source>10000</source>
        <translation>10000</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="322"/>
        <location filename="../pv/views/trace/mathsignal.cpp" line="332"/>
        <source>1000000</source>
        <translation>1000000</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="325"/>
        <source>Number of Samples</source>
        <translation>采样数</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="333"/>
        <source>Sample rate</source>
        <translation>采样率</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Ruler</name>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="155"/>
        <source>Create marker here</source>
        <translation>在此处创建标记</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="159"/>
        <source>Set as zero point</source>
        <translation>设为零点</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="164"/>
        <source>Reset zero point</source>
        <translation>重置零点</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="177"/>
        <source>Disable mouse hover marker</source>
        <translation>禁用鼠标位置标记</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="177"/>
        <source>Enable mouse hover marker</source>
        <translation>启用鼠标位置标记</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Signal</name>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="155"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="169"/>
        <source>Remove</source>
        <translation>删除</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="171"/>
        <source>Disable</source>
        <translation>关闭</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::StandardBar</name>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="61"/>
        <source>Zoom &amp;In</source>
        <translation>放大</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="69"/>
        <source>Zoom &amp;Out</source>
        <translation>缩小</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="77"/>
        <source>Zoom to &amp;Fit</source>
        <translation>适应窗口</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="89"/>
        <source>Show &amp;Cursors</source>
        <translation>测量</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="92"/>
        <source>Show Cursors</source>
        <translation>测量</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="95"/>
        <source>Display last segment only</source>
        <translation>仅显示最后一段</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="100"/>
        <source>Display last complete segment only</source>
        <translation>仅显示最后一个完整分段</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="105"/>
        <source>Display a single segment</source>
        <translation>仅显示一段</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TimeMarker</name>
    <message>
        <location filename="../pv/views/trace/timemarker.cpp" line="223"/>
        <source>Time</source>
        <translation>时间</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Trace</name>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="238"/>
        <source>Create marker here</source>
        <translation>在此处创建标记</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="384"/>
        <source>Color</source>
        <translation>颜色</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="449"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TraceGroup</name>
    <message>
        <location filename="../pv/views/trace/tracegroup.cpp" line="142"/>
        <source>Ungroup</source>
        <translation>取消组合</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::View</name>
</context>
<context>
    <name>pv::views::trace::ViewWidget</name>
</context>
<context>
    <name>pv::views::trace::Viewport</name>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="275"/>
        <location filename="../pv/views/trace/viewport.cpp" line="318"/>
        <source>Width: </source>
        <translation>宽度: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="320"/>
        <source>Period: </source>
        <translation>周期: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="322"/>
        <source>Frequency: </source>
        <translation>频率: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="324"/>
        <source>Duty Cycle: </source>
        <translation>占空比: </translation>
    </message>
</context>
<context>
    <name>pv::widgets::DecoderGroupBox</name>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="48"/>
        <source>Show/hide this decoder trace</source>
        <translation>显示/隐藏此解码器采集</translation>
    </message>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="58"/>
        <source>Delete this decoder trace</source>
        <translation>删除此解码器采集</translation>
    </message>
</context>
<context>
    <name>pv::widgets::DeviceToolButton</name>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="80"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="87"/>
        <source>&lt;No Device&gt;</source>
        <translation>&lt;无设备&gt;</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="179"/>
        <source>Refresh Device List</source>
        <translation>刷新设备列表</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ExportMenu</name>
    <message>
        <location filename="../pv/widgets/exportmenu.cpp" line="73"/>
        <source>Export %1...</source>
        <translation>导出 %1...</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ImportMenu</name>
    <message>
        <location filename="../pv/widgets/importmenu.cpp" line="70"/>
        <source>Import %1...</source>
        <translation>导入 %1...</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder</name>
<message>
        <location filename="lu_src/pulseview/pv/views/tabular_decoder/view.cpp" line="58"/>
        <source>CSV, commas escaped</source>
        <translation>CSV，逗号转义</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/views/tabular_decoder/view.cpp" line="59"/>
        <source>CSV, fields quoted</source>
        <translation>CSV，字段加引号</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/views/tabular_decoder/view.cpp" line="63"/>
        <source>Show all</source>
        <translation>显示全部</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/views/tabular_decoder/view.cpp" line="64"/>
        <source>Show all and focus on newest</source>
        <translation>显示全部并聚焦最新</translation>
    </message><message>
        <location filename="lu_src/pulseview/pv/views/tabular_decoder/view.cpp" line="65"/>
        <source>Show visible in main view</source>
        <translation>显示主视图中可见的部分</translation>
    </message></context>
</TS>
