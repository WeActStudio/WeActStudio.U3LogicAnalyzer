<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>QApplication</name>
    <message>
        <location filename="../pv/devicemanager.cpp" line="349"/>
        <source>Error when scanning device driver &apos;%1&apos;: %2</source>
        <translation>Ошибка при поиске драйвера устройства &apos;%1&apos;: %2</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="72"/>
        <source>Querying config key %1 is not allowed</source>
        <translation>Запрос ключа конфигурации %1 не разрешён</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="81"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>Запрос ключа конфигурации %1 дал %2</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="95"/>
        <source>Unknown type supplied when attempting to query %1</source>
        <translation>При запросе %1 передан неизвестный тип</translation>
    </message>
</context>
<context>
    <name>QHexView</name>
    <message>
        <location filename="../pv/views/decoder_binary/QHexView.cpp" line="347"/>
        <source>No data available</source>
        <translation>Нет данных</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="129"/>
        <source>Stack trace of previous crash:</source>
        <translation>Трассировка предыдущего сбоя:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="143"/>
        <source>Don&apos;t show this message again</source>
        <translation>Больше не показывать это сообщение</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="146"/>
        <source>When %1 last crashed, it created a stack trace.
A human-readable form has been saved to disk and was written to the log. You may access it from the settings dialog.</source>
        <translation>При последнем сбое %1 создал трассировку стека.
Читаемая форма сохранена на диск и записана в журнал. Открыть её можно из диалога настроек.</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="81"/>
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="119"/>
        <source>Scanning for devices that driver %1 can access...</source>
        <translation>Поиск устройств, доступных драйверу %1...</translation>
    </message>
</context>
<context>
    <name>pv::MainWindow</name>
    <message>
        <location filename="../pv/mainwindow.cpp" line="185"/>
        <location filename="../pv/mainwindow.cpp" line="1157"/>
        <location filename="../pv/mainwindow.cpp" line="2383"/>
        <source>Warning</source>
        <translation>Предупреждение</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="206"/>
        <source>Current available memory size is less than 4GB and will affect usage effect. 
Do you want to continue to use?</source>
        <translation>Доступный объём памяти меньше 4 ГБ, это повлияет на работу. 
Продолжить?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="215"/>
        <location filename="../pv/mainwindow.cpp" line="2802"/>
        <location filename="../pv/mainwindow.cpp" line="3434"/>
        <source>Confirm</source>
        <translation>Подтвердить</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="216"/>
        <location filename="../pv/mainwindow.cpp" line="1218"/>
        <location filename="../pv/mainwindow.cpp" line="2403"/>
        <location filename="../pv/mainwindow.cpp" line="3293"/>
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="246"/>
        <source>No udev access rules were found for:
%1

Without them the device cannot be opened by an unprivileged user, so it will not be detected.

Enter your administrator password to install:
%2</source>
        <translation>Не найдены правила доступа udev для:
%1

Без них непривилегированный пользователь не сможет открыть устройство, и оно не будет обнаружено.

Введите пароль администратора для установки:
%2</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="257"/>
        <source>Administrator privileges required</source>
        <translation>Требуются права администратора</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="266"/>
        <source>Access rules installed</source>
        <translation>Правила доступа установлены</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="267"/>
        <source>The udev rules have been installed.

If the device is already plugged in, unplug and replug it before scanning again.</source>
        <translation>Правила udev установлены.

Если устройство уже подключено, отключите и подключите его заново перед повторным поиском.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="274"/>
        <source>Installation failed</source>
        <translation>Не удалось выполнить установку</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="275"/>
        <source>Could not install the udev rules:

%1

Please try again, or cancel to skip.</source>
        <translation>Не удалось установить правила udev:

%1

Повторите попытку или отмените, чтобы пропустить этот шаг.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="281"/>
        <source>Skipped installing access rules</source>
        <translation>Установка правил доступа пропущена</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="282"/>
        <source>The udev rules are not installed, so the device may not be detected.

You can run this application as root later, or copy the rule files from the repository to /etc/udev/rules.d/ and replug the device.</source>
        <translation>Правила udev не установлены, поэтому устройство может не обнаружиться.

Позже можно запустить программу от root либо скопировать файлы правил из репозитория в /etc/udev/rules.d/ и подключить устройство заново.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="769"/>
        <location filename="../pv/mainwindow.cpp" line="2181"/>
        <source>Decoder Selector</source>
        <translation>Выбор decoder</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="836"/>
        <source>Session %1</source>
        <translation>Сеанс %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1192"/>
        <source>device detached!
Do you want to save the session?</source>
        <translation>устройство отключено!
Сохранить сеанс?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1217"/>
        <source>Save</source>
        <translation>Сохранить</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1264"/>
        <source>Save File</source>
        <translation>Сохранить файл</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1533"/>
        <source>Device scan timed out</source>
        <translation>Поиск устройств превысил время ожидания</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1534"/>
        <source>Scanning these drivers took too long, so the software started without waiting for them:</source>
        <translation>Поиск этих драйверов занял слишком много времени, поэтому программа запустилась не дожидаясь их:</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1536"/>
        <source>Their devices are not in the list. Unplug and replug the analyzer - or close whatever else is using it - and scan again.</source>
        <translation>Их устройства отсутствуют в списке. Отключите и подключите анализатор заново — или закройте программу, которая его использует, — и повторите поиск.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2131"/>
        <source>Sessions not restored</source>
        <translation>Сеансы не восстановлены</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2132"/>
        <source>These boards are not connected, so the pages they held are not here:</source>
        <translation>Эти платы не подключены, поэтому страниц, которые они содержали, здесь нет:</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2135"/>
        <source>Plug the board in, or close the program that is using it, then open the page again.</source>
        <translation>Подключите плату или закройте программу, которая её использует, и откройте страницу заново.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2164"/>
        <location filename="../pv/mainwindow.cpp" line="2235"/>
        <source>Start/Stop Acquisition</source>
        <translation>Запуск/остановка захвата</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2165"/>
        <location filename="../pv/mainwindow.cpp" line="2166"/>
        <location filename="../pv/mainwindow.cpp" line="2238"/>
        <location filename="../pv/mainwindow.cpp" line="2239"/>
        <source>Repeat Acquisition</source>
        <translation>Повторить захват</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2167"/>
        <location filename="../pv/mainwindow.cpp" line="2246"/>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2317"/>
        <location filename="../pv/mainwindow.cpp" line="2523"/>
        <source>Reload</source>
        <translation>Перезагрузить</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2317"/>
        <location filename="../pv/mainwindow.cpp" line="2325"/>
        <location filename="../pv/mainwindow.cpp" line="2523"/>
        <source>Run</source>
        <translation>Запуск</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2331"/>
        <source>Stop</source>
        <translation>Стоп</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2395"/>
        <source>&lt;h3&gt;Unsaved Capture Data&lt;/h3&gt;&lt;p&gt;Data will be permanently lost on exit.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Несохранённые данные захвата&lt;/h3&gt;&lt;p&gt;При выходе данные будут потеряны безвозвратно.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2402"/>
        <source>Discard &amp;&amp; Exit</source>
        <translation>Отменить &amp;&amp; Выйти</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2473"/>
        <source>Capture Failed</source>
        <translation>Сбой захвата</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2538"/>
        <source>Device stopped sending data</source>
        <translation>Устройство перестало передавать данные</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2539"/>
        <source>The device stopped sending samples before the capture was due to end, usually because the USB link cannot sustain the configured %1.

The acquisition was stopped. The data captured before that may be incomplete or wrong. Try a lower sample rate, fewer channels, or another USB port.</source>
        <translation>Устройство перестало передавать выборки до того, как захват должен был завершиться, обычно потому что USB-соединение не выдерживает настроенные %1.

Захват остановлен. Данные, полученные до этого, могут быть неполными или неверными. Попробуйте меньший sample rate, меньше каналов или другой порт USB.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2803"/>
        <location filename="../pv/mainwindow.cpp" line="3435"/>
        <source>This session contains unsaved data. Close anyway?</source>
        <translation>В этом сеансе есть несохранённые данные. Всё равно закрыть?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3035"/>
        <source>Board: %1</source>
        <translation>Плата: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3131"/>
        <source>Close Session</source>
        <translation>Закрыть сеанс</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3353"/>
        <location filename="../pv/mainwindow.cpp" line="3376"/>
        <source>Could not copy the capture: %1</source>
        <translation>Не удалось скопировать запись: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3408"/>
        <location filename="../pv/mainwindow.cpp" line="3420"/>
        <source>Could not save the session: %1</source>
        <translation>Не удалось сохранить сеанс: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3050"/>
        <source>Decoders: %1</source>
        <translation>Декодеры: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3295"/>
        <source>Export to a New Session</source>
        <translation>Экспортировать в новый сеанс</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3030"/>
        <source>Port: %1</source>
        <translation>Порт: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3040"/>
        <source>SN: %1</source>
        <translation>SN: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1265"/>
        <source>Session files (*.sr);;All Files (*)</source>
        <translation>Файлы сеансов (*.sr);;Все файлы (*)</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3023"/>
        <source>Shows a session file</source>
        <translation>Показан файл сеанса</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3128"/>
        <source>Export the capture to a new session</source>
        <translation>Экспортировать запись в новый сеанс</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3127"/>
        <location filename="../pv/mainwindow.cpp" line="3293"/>
        <location filename="../pv/mainwindow.cpp" line="3323"/>
        <source>Exporting the capture... %1%</source>
        <translation>Экспорт записи… %1%</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1215"/>
        <source>Export Only the Capture to a New Session</source>
        <translation>Экспортировать только запись в новый сеанс</translation>
    </message>
</context>
<context>
    <name>pv::Session</name>
    <message>
        <location filename="../pv/session.cpp" line="521"/>
        <source>Can&apos;t restore generated signal of unknown type %1 (%2)</source>
        <translation>Не удалось восстановить сгенерированный сигнал неизвестного типа %1 (%2)</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="742"/>
        <source>Out of memory, acquisition stopped.</source>
        <translation>Недостаточно памяти, захват остановлен.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="747"/>
        <source>this feature is currently not supported.</source>
        <translation>эта функция пока не поддерживается.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="874"/>
        <source>The device is already in use, or this user has no access to it. Close the program that holds it, then try again.</source>
        <translation>Устройство уже используется, либо у этого пользователя нет к нему доступа. Закройте программу, которая его занимает, и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="877"/>
        <source>The device could not be opened.</source>
        <translation>Не удалось открыть устройство.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="880"/>
        <source>Failed to open device</source>
        <translation>Не удалось открыть устройство</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1110"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1111"/>
        <source>Unexpected input format: %1</source>
        <translation>Неожиданный формат входных данных: %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1147"/>
        <source>Failed to load %1</source>
        <translation>Не удалось загрузить %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1189"/>
        <source>No active device set, can&apos;t start acquisition.</source>
        <translation>Активное устройство не задано, невозможно начать захват.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1204"/>
        <source>No channels enabled.</source>
        <translation>Каналы не включены.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="2275"/>
        <source>Can&apos;t handle more than 64 logic channels.</source>
        <translation>Невозможно обработать более 64 логических каналов.</translation>
    </message>
</context>
<context>
    <name>pv::StoreSession</name>
    <message>
        <location filename="../pv/storesession.cpp" line="116"/>
        <source>Can&apos;t save logic channel without data.</source>
        <translation>Невозможно сохранить логический канал без данных.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="132"/>
        <source>Can&apos;t save analog channel without data.</source>
        <translation>Невозможно сохранить аналоговый канал без данных.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="144"/>
        <source>No channels enabled.</source>
        <translation>Каналы не включены.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="172"/>
        <source>Can&apos;t save range without sample data.</source>
        <translation>Невозможно сохранить диапазон без данных выборок.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="193"/>
        <location filename="../pv/storesession.cpp" line="300"/>
        <location filename="../pv/storesession.cpp" line="315"/>
        <source>Error while saving: </source>
        <translation>Ошибка при сохранении: </translation>
    </message>
</context>
<context>
    <name>pv::binding::Device</name>
    <message>
        <location filename="../pv/binding/device.cpp" line="84"/>
        <source>Note for device developers: Ignoring device configuration capability &apos;%1&apos; as it is missing GET and/or SET</source>
        <translation>Примечание для разработчиков устройств: возможность настройки &apos;%1&apos; игнорируется, так как отсутствует GET и/или SET</translation>
    </message>
    <message>
        <location filename="../pv/binding/device.cpp" line="109"/>
        <source>No Limit</source>
        <translation>Без ограничения</translation>
    </message>
</context>
<context>
    <name>pv::data::DecodeSignal</name>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="257"/>
        <source>No decoders</source>
        <translation>Нет decoders</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="264"/>
        <source>There are no channels assigned to this decoder</source>
        <translation>К этому decoder не назначено ни одного канала</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="278"/>
        <source>One or more required channels have not been specified</source>
        <translation>Один или несколько обязательных каналов не заданы</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="294"/>
        <source>No input data</source>
        <translation>Нет входных данных</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="1648"/>
        <location filename="../pv/data/decodesignal.cpp" line="1733"/>
        <source>Decoder reported an error</source>
        <translation>Decoder сообщил об ошибке</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="1956"/>
        <source>Failed to create decoder instance</source>
        <translation>Не удалось создать экземпляр decoder</translation>
    </message>
</context>
<context>
    <name>pv::data::MathSignal</name>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="107"/>
        <source>Math%1</source>
        <translation>Math%1</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="306"/>
        <source>No expression defined, nothing to do</source>
        <translation>Выражение не задано, нечего выполнять</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="345"/>
        <source>%1 at line %2, column %3: %4</source>
        <translation>%1 в строке %2, столбце %3: %4</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="364"/>
        <location filename="../pv/data/mathsignal.cpp" line="536"/>
        <source>&quot;%1&quot; isn&apos;t a valid analog signal</source>
        <translation>&quot;%1&quot; не является допустимым аналоговым сигналом</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="374"/>
        <location filename="../pv/data/mathsignal.cpp" line="611"/>
        <source>No data will be generated as %1 must be enabled</source>
        <translation>Данные не будут созданы, так как %1 должен быть включён</translation>
    </message>
</context>
<context>
    <name>pv::data::SignalBase</name>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="537"/>
        <source>Signal average</source>
        <translation>Среднее значение сигнала</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="538"/>
        <source>0.9V (for 1.8V CMOS)</source>
        <translation>0.9 В (для 1.8 В CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="539"/>
        <source>1.8V (for 3.3V CMOS)</source>
        <translation>1.8 В (для 3.3 В CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="540"/>
        <source>2.5V (for 5.0V CMOS)</source>
        <translation>2.5 В (для 5.0 В CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="541"/>
        <source>1.5V (for TTL)</source>
        <translation>1.5 В (для TTL)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="546"/>
        <source>Signal average +/- 15%</source>
        <translation>Среднее сигнала +/- 15 %</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="547"/>
        <source>0.3V/1.2V (for 1.8V CMOS)</source>
        <translation>0.3 В/1.2 В (для 1.8 В CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="548"/>
        <source>0.7V/2.5V (for 3.3V CMOS)</source>
        <translation>0.7 В/2.5 В (для 3.3 В CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="549"/>
        <source>1.3V/3.7V (for 5.0V CMOS)</source>
        <translation>1.3 В/3.7 В (для 5.0 В CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="550"/>
        <source>0.8V/2.0V (for TTL)</source>
        <translation>0.8 В/2.0 В (для TTL)</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::Connect</name>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="58"/>
        <source>&amp;Scan for devices using driver above</source>
        <translation>&amp;Искать устройства с указанным выше драйвером</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="63"/>
        <source>Connect to Device</source>
        <translation>Подключение к устройству</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="75"/>
        <source>Step 1: Choose the driver</source>
        <translation>Шаг 1. Выберите драйвер</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="79"/>
        <source>&amp;USB</source>
        <translation>&amp;USB</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="80"/>
        <source>Serial &amp;Port</source>
        <translation>Последовательный &amp;порт</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="81"/>
        <source>&amp;TCP/IP</source>
        <translation>&amp;TCP/IP</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="116"/>
        <source>Protocol:</source>
        <translation>Протокол:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="134"/>
        <source>Step 2: Choose the interface</source>
        <translation>Шаг 2. Выберите интерфейс</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="140"/>
        <source>Step 3: Scan for devices</source>
        <translation>Шаг 3. Найдите устройства</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="146"/>
        <source>Step 4: Select the device</source>
        <translation>Шаг 4. Выберите устройство</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::DecoderChannelDialog</name>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="50"/>
        <source>Decoder Configuration</source>
        <translation>Настройка decoder</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="92"/>
        <source>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</source>
        <translation>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="105"/>
        <source>Channel Mapping</source>
        <translation>Сопоставление каналов</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="110"/>
        <source>Decoder Options</source>
        <translation>Параметры decoder</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="121"/>
        <source>Apply &amp;&amp; Decode</source>
        <translation>Применить &amp;&amp; декодировать</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="122"/>
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="173"/>
        <source>-- Not assigned --</source>
        <translation>-- Не назначен --</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="214"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2)%3</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2)%3</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::IAPDialog</name>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="94"/>
        <source>FW Update</source>
        <translation>Обновление прошивки</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="101"/>
        <source>FW File:</source>
        <translation>Файл прошивки:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="103"/>
        <source>Select firmware file in BIN or HEX format</source>
        <translation>Выберите файл прошивки в формате BIN или HEX</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="105"/>
        <source>Browse</source>
        <translation>Обзор</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="114"/>
        <source>Ready</source>
        <translation>Готово</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="119"/>
        <source>Update Log:</source>
        <translation>Журнал обновления:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="128"/>
        <source>Start</source>
        <translation>Старт</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="130"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="189"/>
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="156"/>
        <source>Select firmware file</source>
        <translation>Выбор файла прошивки</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="158"/>
        <source>Firmware File (*.bin *.hex);;All Files (*.*)</source>
        <translation>Файл прошивки (*.bin *.hex);;Все файлы (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="172"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="177"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="172"/>
        <source>Please select a firmware file</source>
        <translation>Выберите файл прошивки</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="177"/>
        <source>Firmware file does not exist</source>
        <translation>Файл прошивки не существует</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="237"/>
        <source>Close</source>
        <translation>Закрыть</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="243"/>
        <source>Update Success</source>
        <translation>Обновление выполнено</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="245"/>
        <source>Success</source>
        <translation>Успешно</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="245"/>
        <source>Firmware Update Success!
Device will restart.</source>
        <translation>Прошивка успешно обновлена!
Устройство перезапустится.</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="247"/>
        <source>Update Failed</source>
        <translation>Обновление не удалось</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="249"/>
        <source>Fail</source>
        <translation>Сбой</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="249"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>Не удалось обновить прошивку!
Проверьте подключение устройства и файл прошивки.</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::IAPWorker</name>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="508"/>
        <source>Erasing flash...</source>
        <translation>Стирание flash...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="548"/>
        <source>Programming flash...</source>
        <translation>Программирование flash...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="608"/>
        <source>Programming flash... %1%</source>
        <translation>Программирование flash... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="626"/>
        <source>Verifying flash...</source>
        <translation>Проверка flash...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="694"/>
        <source>Verifying flash... %1%</source>
        <translation>Проверка flash... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="704"/>
        <source>Finishing the upgrade...</source>
        <translation>Завершение обновления...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="882"/>
        <source>Reading the firmware file...</source>
        <translation>Чтение файла прошивки...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="897"/>
        <source>Device already in IAP mode...</source>
        <translation>Устройство уже в режиме IAP...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="899"/>
        <source>Entering IAP mode...</source>
        <translation>Вход в режим IAP...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="906"/>
        <source>Looking for the HID device...</source>
        <translation>Поиск устройства HID...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="978"/>
        <source>Upgrade complete</source>
        <translation>Обновление завершено</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::Settings</name>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="394"/>
        <location filename="../pv/mainwindow.cpp" line="3496"/>
        <source>Use colored trace &amp;background</source>
        <translation>Использовать &amp;цветной фон сигналов</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="410"/>
        <location filename="../pv/mainwindow.cpp" line="3506"/>
        <source>Always keep &amp;newest samples at the right edge during capture</source>
        <translation>Всегда держать &amp;новые выборки у правого края во время захвата</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="418"/>
        <location filename="../pv/mainwindow.cpp" line="3516"/>
        <source>Show data &amp;sampling points</source>
        <translation>Показывать точки &amp;выборки данных</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="433"/>
        <location filename="../pv/mainwindow.cpp" line="3526"/>
        <source>Show analog minor grid in addition to div grid</source>
        <translation>Показывать дополнительную мелкую сетку на аналоговых сигналах</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="210"/>
        <location filename="../pv/dialogs/settings.cpp" line="289"/>
        <source>General</source>
        <translation>Общие</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="219"/>
        <source>Views</source>
        <translation>Вид</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="229"/>
        <location filename="../pv/dialogs/settings.cpp" line="496"/>
        <source>Decoders</source>
        <translation>Decoders</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="239"/>
        <source>About</source>
        <translation>О программе</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="248"/>
        <source>Logging</source>
        <translation>Журналирование</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="317"/>
        <source>User interface language</source>
        <translation>Язык интерфейса</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="321"/>
        <source>Follow system</source>
        <translation>Как в системе</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="322"/>
        <source>Light</source>
        <translation>Светлая</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="323"/>
        <source>Dark</source>
        <translation>Тёмная</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="329"/>
        <source>User interface theme</source>
        <translation>Тема интерфейса</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="362"/>
        <source>User interface style</source>
        <translation>Стиль интерфейса</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="367"/>
        <source>Save session &amp;setup along with .sr file</source>
        <translation>Сохранять настройки сеанса &amp;вместе с файлом .sr</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="386"/>
        <source>Trace View</source>
        <translation>Вид сигналов</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="398"/>
        <source>Constantly perform &amp;zoom-to-fit during acquisition</source>
        <translation>Постоянно выполнять &amp;подгонку по размеру во время захвата</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="402"/>
        <source>Perform a zoom-to-&amp;fit when acquisition stops</source>
        <translation>Выполнять подгонку по &amp;размеру при остановке захвата</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="406"/>
        <source>Show time zero at the &amp;trigger</source>
        <translation>Показывать нулевой момент времени на &amp;trigger</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="414"/>
        <source>Allow &amp;vertical dragging in the view area</source>
        <translation>Разрешить &amp;перетаскивание по вертикали в области просмотра</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="422"/>
        <source>Fill &amp;high areas of logic signals</source>
        <translation>Заливать &amp;верхние участки логических сигналов</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="429"/>
        <source>Color to fill high areas of logic signals with</source>
        <translation>Цвет заливки верхних участков логических сигналов</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="437"/>
        <source>Highlight mouse cursor using a vertical marker line</source>
        <translation>Подсвечивать курсор мыши вертикальной линией</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="441"/>
        <location filename="../pv/dialogs/settings.cpp" line="467"/>
        <location filename="../pv/dialogs/settings.cpp" line="476"/>
        <source> pixels</source>
        <translation> пикселей</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="446"/>
        <source>Maximum distance from edges before markers snap to them</source>
        <translation>Максимальное расстояние до края, при котором маркеры к нему привязываются</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="453"/>
        <source>Color to fill cursor area with</source>
        <translation>Цвет заливки области курсора</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="456"/>
        <source>None</source>
        <translation>Нет</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="457"/>
        <source>Background</source>
        <translation>Фон</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="458"/>
        <source>Dots</source>
        <translation>Точки</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="463"/>
        <source>Conversion threshold display mode (analog traces only)</source>
        <translation>Режим отображения порога преобразования (только для аналоговых сигналов)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="472"/>
        <source>Default analog trace div height</source>
        <translation>Высота деления аналогового сигнала по умолчанию</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="481"/>
        <source>Default logic trace height</source>
        <translation>Высота логического сигнала по умолчанию</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="504"/>
        <source>Allow configuration of &amp;initial signal state</source>
        <translation>Разрешить настройку &amp;начального состояния сигнала</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="508"/>
        <source>Always show all &amp;rows, even if no annotation is visible</source>
        <translation>Всегда показывать все &amp;строки, даже если аннотации не видны</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="516"/>
        <source>Annotation export format</source>
        <translation>Формат экспорта аннотаций</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="517"/>
        <source>%s = sample range; %d: decoder name; %r: row name; %c: class name</source>
        <translation>%s = диапазон выборок; %d: имя decoder; %r: имя строки; %c: имя класса</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="520"/>
        <source>%1: longest annotation text; %a: all annotation texts; %q: use quotation marks</source>
        <translation>%1: самый длинный текст аннотации; %a: все тексты аннотаций; %q: использовать кавычки</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="540"/>
        <source>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</source>
        <translation>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="541"/>
        <source>GNU GPL, version 3 or later</source>
        <translation>GNU GPL, версия 3 или новее</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="552"/>
        <source>Versions, libraries and features:</source>
        <translation>Версии, библиотеки и возможности:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="559"/>
        <source>Firmware search paths:</source>
        <translation>Пути поиска прошивок:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="566"/>
        <source>Protocol decoder search paths:</source>
        <translation>Пути поиска protocol decoder:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="569"/>
        <source>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;(Note: Set environment variable SIGROKDECODE_DIR to add a custom directory)&lt;/td&gt;&lt;/tr&gt;</source>
        <translation>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;(Примечание: задайте переменную среды SIGROKDECODE_DIR, чтобы добавить собственный каталог)&lt;/td&gt;&lt;/tr&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="574"/>
        <source>Supported hardware drivers:</source>
        <translation>Поддерживаемые драйверы устройств:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="581"/>
        <source>Supported input formats:</source>
        <translation>Поддерживаемые входные форматы:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="588"/>
        <source>Supported output formats:</source>
        <translation>Поддерживаемые выходные форматы:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="596"/>
        <source>Supported protocol decoders:</source>
        <translation>Поддерживаемые protocol decoder:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="604"/>
        <source>Available Translations:</source>
        <translation>Доступные языки:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="652"/>
        <source>Log level:</source>
        <translation>Уровень журналирования:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="657"/>
        <source> lines</source>
        <translation> строк</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="666"/>
        <source>Length of background buffer:</source>
        <translation>Размер фонового буфера:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="672"/>
        <source>&amp;Save to File</source>
        <translation>&amp;Сохранить в файл</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="679"/>
        <source>&amp;Pop out</source>
        <translation>&amp;Открепить</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="977"/>
        <source>Save Log</source>
        <translation>Сохранить журнал</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="977"/>
        <source>Log Files (*.txt *.log);;All Files (*)</source>
        <translation>Журналы (*.txt *.log);;Все файлы (*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="989"/>
        <source>Success</source>
        <translation>Успешно</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="989"/>
        <source>Log saved to %1.</source>
        <translation>Журнал сохранён в %1.</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="999"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="999"/>
        <source>File %1 could not be written to.</source>
        <translation>Не удалось записать в файл %1.</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="1013"/>
        <source>%1 Log</source>
        <translation>Журнал %1</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::StoreProgress</name>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Saving...</source>
        <translation>Сохранение...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="89"/>
        <source>Failed to save session.</source>
        <translation>Не удалось сохранить сеанс.</translation>
    </message>
</context>
<context>
    <name>pv::popups::Channels</name>
    <message>
        <location filename="../pv/popups/channels.cpp" line="228"/>
        <location filename="../pv/popups/channels.cpp" line="326"/>
        <location filename="../pv/popups/channels.cpp" line="1331"/>
        <source>Model:</source>
        <translation>Модель:</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="232"/>
        <location filename="../pv/popups/channels.cpp" line="332"/>
        <location filename="../pv/popups/channels.cpp" line="1332"/>
        <source>SN:</source>
        <translation>SN:</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="234"/>
        <location filename="../pv/popups/channels.cpp" line="1320"/>
        <source>Options</source>
        <translation>Параметры</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="377"/>
        <location filename="../pv/popups/channels.cpp" line="587"/>
        <location filename="../pv/popups/channels.cpp" line="1337"/>
        <source>With a repeating signal the trigger point floats: this is a limit rather than a fixed place.</source>
        <translation>При периодическом сигнале точка trigger плавает: это предел, а не фиксированное место.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="382"/>
        <location filename="../pv/popups/channels.cpp" line="590"/>
        <location filename="../pv/popups/channels.cpp" line="1335"/>
        <source>Pre-trigger</source>
        <translation>Pre-trigger</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="450"/>
        <location filename="../pv/popups/channels.cpp" line="1406"/>
        <source>Use 2 channels (Max 40MHz)</source>
        <translation>Использовать 2 канала (макс. 40 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="451"/>
        <location filename="../pv/popups/channels.cpp" line="1407"/>
        <source>Use 4 channels (Max 40MHz)</source>
        <translation>Использовать 4 канала (макс. 40 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="452"/>
        <location filename="../pv/popups/channels.cpp" line="1408"/>
        <source>Use 8 channels (Max 40MHz)</source>
        <translation>Использовать 8 каналов (макс. 40 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="453"/>
        <location filename="../pv/popups/channels.cpp" line="1409"/>
        <source>Use 16 channels (Max 20MHz)</source>
        <translation>Использовать 16 каналов (макс. 20 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="456"/>
        <location filename="../pv/popups/channels.cpp" line="1411"/>
        <source>Use 2 channels (Max 200MHz)</source>
        <translation>Использовать 2 канала (макс. 200 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="457"/>
        <location filename="../pv/popups/channels.cpp" line="1412"/>
        <source>Use 4 channels (Max 200MHz)</source>
        <translation>Использовать 4 канала (макс. 200 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="458"/>
        <location filename="../pv/popups/channels.cpp" line="1413"/>
        <source>Use 8 channels (Max 200MHz)</source>
        <translation>Использовать 8 каналов (макс. 200 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="459"/>
        <location filename="../pv/popups/channels.cpp" line="1414"/>
        <source>Use 16 channels (Max 200MHz)</source>
        <translation>Использовать 16 каналов (макс. 200 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="463"/>
        <location filename="../pv/popups/channels.cpp" line="1417"/>
        <source>Use 2 channels (Max 1GHz)</source>
        <translation>Использовать 2 канала (макс. 1 ГГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="464"/>
        <location filename="../pv/popups/channels.cpp" line="1418"/>
        <source>Use 4 channels (Max 500MHz)</source>
        <translation>Использовать 4 канала (макс. 500 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="465"/>
        <location filename="../pv/popups/channels.cpp" line="1419"/>
        <source>Use 8 channels (Max 250MHz)</source>
        <translation>Использовать 8 каналов (макс. 250 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="466"/>
        <location filename="../pv/popups/channels.cpp" line="1420"/>
        <source>Use 16 channels (Max 125MHz)</source>
        <translation>Использовать 16 каналов (макс. 125 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="469"/>
        <location filename="../pv/popups/channels.cpp" line="1423"/>
        <source>Use 2 channels (Max 500MHz)</source>
        <translation>Использовать 2 канала (макс. 500 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="470"/>
        <location filename="../pv/popups/channels.cpp" line="1424"/>
        <source>Use 4 channels (Max 250MHz)</source>
        <translation>Использовать 4 канала (макс. 250 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="471"/>
        <location filename="../pv/popups/channels.cpp" line="1425"/>
        <source>Use 8 channels (Max 125MHz)</source>
        <translation>Использовать 8 каналов (макс. 125 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="472"/>
        <location filename="../pv/popups/channels.cpp" line="1426"/>
        <source>Use 16 channels (Max 62.5MHz)</source>
        <translation>Использовать 16 каналов (макс. 62,5 МГц)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="508"/>
        <location filename="../pv/popups/channels.cpp" line="1340"/>
        <source>Trigger Threshold(0~7 Channel)</source>
        <translation>Trigger Threshold (каналы 0~7)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="516"/>
        <location filename="../pv/popups/channels.cpp" line="1341"/>
        <source>Trigger Threshold(8~15 Channel)</source>
        <translation>Trigger Threshold (каналы 8~15)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="523"/>
        <location filename="../pv/popups/channels.cpp" line="1346"/>
        <source>Logic Analyzer</source>
        <translation>Logic Analyzer</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="526"/>
        <location filename="../pv/popups/channels.cpp" line="1348"/>
        <source>ADC</source>
        <translation>ADC</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="534"/>
        <location filename="../pv/popups/channels.cpp" line="1334"/>
        <source>Device Mode</source>
        <translation>Режим устройства</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="557"/>
        <source>Always match this to the level of the signal being measured, or the samples will be read at the wrong level.</source>
        <translation>Всегда согласуйте это с уровнем измеряемого сигнала, иначе выборки будут считаны на неверном уровне.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="575"/>
        <location filename="../pv/popups/channels.cpp" line="1343"/>
        <source>Input Level</source>
        <translation>Входной уровень</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="575"/>
        <location filename="../pv/popups/channels.cpp" line="1286"/>
        <location filename="../pv/popups/channels.cpp" line="1343"/>
        <location filename="../pv/popups/channels.cpp" line="1361"/>
        <source>Threshold Level</source>
        <translation>Уровень threshold</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="594"/>
        <location filename="../pv/popups/channels.cpp" line="1351"/>
        <source>ADC Channel 0</source>
        <translation>Канал ADC 0</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="595"/>
        <location filename="../pv/popups/channels.cpp" line="1352"/>
        <source>ADC Channel 1</source>
        <translation>Канал ADC 1</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="599"/>
        <location filename="../pv/popups/channels.cpp" line="1327"/>
        <source>ADC Channel</source>
        <translation>Канал ADC</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="605"/>
        <location filename="../pv/popups/channels.cpp" line="1356"/>
        <source>8-bit</source>
        <translation>8 бит</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="606"/>
        <location filename="../pv/popups/channels.cpp" line="1357"/>
        <source>10-bit</source>
        <translation>10 бит</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="610"/>
        <location filename="../pv/popups/channels.cpp" line="1329"/>
        <source>ADC Precision</source>
        <translation>Разрядность ADC</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="677"/>
        <location filename="../pv/popups/channels.cpp" line="743"/>
        <location filename="../pv/popups/channels.cpp" line="1322"/>
        <location filename="../pv/popups/channels.cpp" line="1324"/>
        <source>Channels</source>
        <translation>Каналы</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1202"/>
        <source>Information</source>
        <translation>Информация</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1203"/>
        <source>The current mode does not support opening this channel.</source>
        <translation>Текущий режим не поддерживает открытие этого канала.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1204"/>
        <source>Confirm</source>
        <translation>Подтвердить</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1289"/>
        <location filename="../pv/popups/channels.cpp" line="1364"/>
        <source>Buffer Board</source>
        <translation>Плата buffer</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="223"/>
        <location filename="../pv/popups/channels.cpp" line="320"/>
        <location filename="../pv/popups/channels.cpp" line="1333"/>
        <source>Port:</source>
        <translation>Порт:</translation>
    </message>
</context>
<context>
    <name>pv::prop::Bool</name>
    <message>
        <location filename="../pv/prop/bool.cpp" line="51"/>
        <location filename="../pv/prop/bool.cpp" line="82"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>Запрос ключа конфигурации %1 дал %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Double</name>
    <message>
        <location filename="../pv/prop/double.cpp" line="65"/>
        <location filename="../pv/prop/double.cpp" line="96"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>Запрос ключа конфигурации %1 дал %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Enum</name>
    <message>
        <location filename="../pv/prop/enum.cpp" line="113"/>
        <location filename="../pv/prop/enum.cpp" line="176"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>Запрос ключа конфигурации %1 дал %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Int</name>
    <message>
        <location filename="../pv/prop/int.cpp" line="63"/>
        <location filename="../pv/prop/int.cpp" line="127"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>Запрос ключа конфигурации %1 дал %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::String</name>
    <message>
        <location filename="../pv/prop/string.cpp" line="59"/>
        <location filename="../pv/prop/string.cpp" line="84"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>Запрос ключа конфигурации %1 дал %2</translation>
    </message>
</context>
<context>
    <name>pv::subwindows::decoder_selector::DecoderCollectionModel</name>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="42"/>
        <source>Decoder</source>
        <translation>Decoder</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="43"/>
        <source>Name</source>
        <translation>Имя</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="44"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="51"/>
        <source>All Decoders</source>
        <translation>Все decoders</translation>
    </message>
</context>
<context>
    <name>pv::subwindows::decoder_selector::SubWindow</name>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="54"/>
        <source>Select a decoder to see its description here.</source>
        <translation>Выберите decoder, чтобы увидеть его описание.</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="352"/>
        <source>, %1</source>
        <translation>, %1</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="369"/>
        <source>&lt;p align=&apos;right&apos;&gt;Tags: %1&lt;/p&gt;</source>
        <translation>&lt;p align=&apos;right&apos;&gt;Теги: %1&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="431"/>
        <source>Protocol decoder &lt;b&gt;%1&lt;/b&gt; requires input type &lt;b&gt;%2&lt;/b&gt; which several decoders provide.&lt;br&gt;Choose which one to use:&lt;br&gt;</source>
        <translation>Protocol decoder &lt;b&gt;%1&lt;/b&gt; требует тип входа &lt;b&gt;%2&lt;/b&gt;, который предоставляют несколько decoders.&lt;br&gt;Выберите, какой использовать:&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="439"/>
        <source>Choose Decoder</source>
        <translation>Выбор decoder</translation>
    </message>
</context>
<context>
    <name>pv::toolbars::MainBar</name>
    <message>
        <source>Device offline</source>
        <translation>Устройство отключено</translation>
    </message>
    <message>
        <source>Waiting for the capture in the other session to end</source>
        <translation>Ожидание завершения записи в другом сеансе</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2358"/>
        <source>Switch device</source>
        <translation>Переключить устройство</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2359"/>
        <source>Switch device? The captured data and its decoders will be lost.</source>
        <translation>Переключить устройство? Захваченные данные и их декодеры будут потеряны.</translation>
    </message>
    <message>
        <location filename="../pv/util.cpp" line="474"/>
        <source>Confirm</source>
        <translation>Подтвердить</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="399"/>
        <source>Export to a New Session</source>
        <translation>Экспортировать в новый сеанс</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="507"/>
        <source>Decoding Result</source>
        <translation>Результат декодирования</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="508"/>
        <source>&amp;Open...</source>
        <translation>&amp;Открыть...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="509"/>
        <source>Restore Session Setu&amp;p...</source>
        <translation>Восстановить настройки сеан&amp;са...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="510"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="540"/>
        <source>Save</source>
        <translation>Сохранить</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="511"/>
        <source>Save &amp;As...</source>
        <translation>Сохранить &amp;как...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="512"/>
        <source>Save Selected &amp;Range As...</source>
        <translation>Сохранить выбранный &amp;диапазон как...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="513"/>
        <source>Save Session Setu&amp;p...</source>
        <translation>Сохранить настройки сеан&amp;са...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="514"/>
        <source>&amp;Connect to Device...</source>
        <translation>&amp;Подключиться к устройству...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="517"/>
        <source>&amp;Export</source>
        <translation>&amp;Экспорт</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="519"/>
        <source>&amp;Import</source>
        <translation>&amp;Импорт</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="530"/>
        <source>Single</source>
        <translation>Однократно</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="531"/>
        <source>Repeat</source>
        <translation>Повтор</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="532"/>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="533"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3063"/>
        <source>Version Info</source>
        <translation>Информация о версии</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="534"/>
        <source>FW Update</source>
        <translation>Обновление прошивки</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="535"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3231"/>
        <source>About</source>
        <translation>О программе</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="537"/>
        <source>Help</source>
        <translation>Справка</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="538"/>
        <source>Model</source>
        <translation>Модель</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="539"/>
        <source>Open</source>
        <translation>Открыть</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="542"/>
        <source>Add protocol decoder</source>
        <translation>Добавить protocol decoder</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="543"/>
        <source>Decoder</source>
        <translation>Decoder</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="545"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="547"/>
        <source>Configure Channels</source>
        <translation>Настроить каналы</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="546"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="548"/>
        <source>Option</source>
        <translation>Параметр</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="875"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1443"/>
        <source>Failed to get sample rate list:</source>
        <translation>Не удалось получить список sample rate:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="987"/>
        <source>Failed to get sample rate value:</source>
        <translation>Не удалось получить значение sample rate:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1041"/>
        <source>Failed to get sample limit list:</source>
        <translation>Не удалось получить список ограничений выборки:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1231"/>
        <source>Failed to configure pre-trigger:</source>
        <translation>Не удалось настроить pre-trigger:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1260"/>
        <source>Failed to configure pattern mode:</source>
        <translation>Не удалось настроить режим шаблона:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1281"/>
        <source>Failed to configure ADC channel:</source>
        <translation>Не удалось настроить канал ADC:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1304"/>
        <source>Failed to configure ADC precision:</source>
        <translation>Не удалось настроить разрядность ADC:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1353"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1372"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1418"/>
        <source>Demo</source>
        <translation>Demo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1354"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1373"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1419"/>
        <source>Device Status: Demo</source>
        <translation>Состояние устройства: Demo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1360"/>
        <source>File</source>
        <translation>Файл</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1361"/>
        <source>Device Status: File</source>
        <translation>Состояние устройства: файл</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1405"/>
        <source>USB2.0</source>
        <translation>USB2.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1406"/>
        <source>Device Status: USB 2.0</source>
        <translation>Состояние устройства: USB 2.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1409"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1414"/>
        <source>USB3.0</source>
        <translation>USB3.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1410"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1415"/>
        <source>Device Status: USB 3.0</source>
        <translation>Состояние устройства: USB 3.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1981"/>
        <source>Failed to configure samplerate:</source>
        <translation>Не удалось настроить sample rate:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2174"/>
        <source>Failed to configure sample count:</source>
        <translation>Не удалось настроить число выборок:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2189"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2213"/>
        <source>Missing Cursors</source>
        <translation>Курсоры не заданы</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2213"/>
        <source>You need to set the cursors before you can save the data enclosed by them to a session file (e.g. using the Show Cursors button).</source>
        <translation>Сначала нужно задать курсоры, прежде чем сохранять данные между ними в файл сеанса (например, кнопкой «Показать курсоры»).</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2232"/>
        <source>Invalid Range</source>
        <translation>Недопустимый диапазон</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2232"/>
        <source>The cursors don&apos;t define a valid range of samples.</source>
        <translation>Курсоры не задают допустимый диапазон выборок.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2246"/>
        <source>%1 files </source>
        <translation>файлов %1 </translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2251"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2302"/>
        <source>All Files</source>
        <translation>Все файлы</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2255"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2838"/>
        <source>Save File</source>
        <translation>Сохранить файл</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2268"/>
        <source>Export %1</source>
        <translation>Экспорт %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2300"/>
        <source>%1 files</source>
        <translation>файлов %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2310"/>
        <source>Import File</source>
        <translation>Импортировать файл</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2320"/>
        <source>Import %1</source>
        <translation>Импорт %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2416"/>
        <source>Failed to set CH32H417 mode on device switch:</source>
        <translation>Не удалось установить режим CH32H417 при смене устройства:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2765"/>
        <source>Start</source>
        <translation>Старт</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2770"/>
        <source>Stop</source>
        <translation>Стоп</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2790"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2854"/>
        <source>Open File</source>
        <translation>Открыть файл</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2790"/>
        <source>sigrok Sessions (*.sr);;All Files (*)</source>
        <translation>Сеансы sigrok (*.sr);;Все файлы (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2838"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2854"/>
        <source>PulseView Session Setups (*.pvs);;All Files (*)</source>
        <translation>Настройки сеанса PulseView (*.pvs);;Все файлы (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2924"/>
        <source>Total Sampling Count: %1</source>
        <translation>Всего выборок: %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2958"/>
        <source>Device Options</source>
        <translation>Параметры устройства</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3027"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3046"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3058"/>
        <source>&lt;b&gt;Software Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Версия ПО:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3047"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3059"/>
        <source>&lt;b&gt;Firmware Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Версия прошивки:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3060"/>
        <source>&lt;b&gt;FPGA Version:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Версия FPGA:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3104"/>
        <source>Warning</source>
        <translation>Предупреждение</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3105"/>
        <source>The current device does not support firmware upgrade.
Please connect the CH32H417 device and try again.</source>
        <translation>Текущее устройство не поддерживает обновление прошивки.
Подключите устройство CH32H417 и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3112"/>
        <source>Select firmware file</source>
        <translation>Выбор файла прошивки</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3114"/>
        <source>Firmware File (*.bin *.BIN *.hex *.HEX);;All Files (*.*)</source>
        <translation>Файл прошивки (*.bin *.BIN *.hex *.HEX);;Все файлы (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3129"/>
        <source>Firmware update in progress, do not disconnect the USB connection</source>
        <translation>Идёт обновление прошивки, не отключайте USB</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3129"/>
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3130"/>
        <source>Firmware Update</source>
        <translation>Обновление прошивки</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3174"/>
        <source>Update Failed</source>
        <translation>Обновление не удалось</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3175"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>Не удалось обновить прошивку!
Проверьте подключение устройства и файл прошивки.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3200"/>
        <source>Information</source>
        <translation>Информация</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3201"/>
        <source>PDF file not found:
%1

Make sure the doc directory and the user manual are present.</source>
        <translation>Файл PDF не найден:
%1

Убедитесь, что каталог doc и руководство пользователя на месте.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3211"/>
        <source>Open Failed</source>
        <translation>Не удалось открыть</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3212"/>
        <source>The PDF file could not be opened. Make sure a PDF reader is installed.</source>
        <translation>Не удалось открыть файл PDF. Убедитесь, что установлена программа для чтения PDF.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3255"/>
        <source>Update Success</source>
        <translation>Обновление выполнено</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3256"/>
        <source>Firmware Update Success!
Device already restart.</source>
        <translation>Прошивка успешно обновлена!
Устройство уже перезапущено.</translation>
    </message>
</context>
<context>
    <name>pv::views</name>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="42"/>
        <source>Trace View</source>
        <translation>Вид сигналов</translation>
    </message>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="44"/>
        <source>Binary Decoder Output View</source>
        <translation>Бинарный вывод decoder</translation>
    </message>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="45"/>
        <source>Tabular Decoder Output View</source>
        <translation>Табличный вывод decoder</translation>
    </message>
</context>
<context>
    <name>pv::views::decoder_binary::View</name>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="57"/>
        <source>Binary</source>
        <translation>Бинарный</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="58"/>
        <source>Hex Dump, plain</source>
        <translation>Hex-дамп, без смещения</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="59"/>
        <source>Hex Dump, with offset</source>
        <translation>Hex-дамп, со смещением</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="60"/>
        <source>Hex Dump, canonical</source>
        <translation>Hex-дамп, канонический</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="93"/>
        <source>Decoder:</source>
        <translation>Decoder:</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="101"/>
        <source>Show data as</source>
        <translation>Показать данные как</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="109"/>
        <source>Hexdump</source>
        <translation>Hexdump</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="123"/>
        <source>&amp;Save...</source>
        <translation>&amp;Сохранить...</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="346"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="386"/>
        <source>Save Binary Data</source>
        <translation>Сохранить бинарные данные</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="346"/>
        <source>Binary Data Files (*.bin);;All Files (*)</source>
        <translation>Файлы бинарных данных (*.bin);;Все файлы (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="365"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="417"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="365"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="417"/>
        <source>File %1 could not be written to.</source>
        <translation>Не удалось записать в файл %1.</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="386"/>
        <source>Hex Dumps (*.txt);;All Files (*)</source>
        <translation>Hex-дампы (*.txt);;Все файлы (*)</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::AnnotationCollectionModel</name>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="58"/>
        <source>Sample</source>
        <translation>Выборка</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="59"/>
        <source>Time</source>
        <translation>Время</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="60"/>
        <source>Decoder</source>
        <translation>Decoder</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="61"/>
        <source>Ann Row</source>
        <translation>Категория</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="62"/>
        <source>Ann Class</source>
        <translation>Подкатегория</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="63"/>
        <source>Value</source>
        <translation>Значение</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="66"/>
        <source>End Sample</source>
        <translation>Конец выборки</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="67"/>
        <source>Duration</source>
        <translation>Длительность</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="86"/>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="105"/>
        <source>s</source>
        <translation>s</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="86"/>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="105"/>
        <source>sa</source>
        <translation>sa</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="218"/>
        <source>Filter this column</source>
        <translation>Фильтровать по этому столбцу</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::ColumnFilterPopup</name>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="593"/>
        <source>Clear filter</source>
        <translation>Сбросить фильтр</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::View</name>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="74"/>
        <source>CSV, commas escaped</source>
        <translation>CSV, запятые экранированы</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="75"/>
        <source>CSV, fields quoted</source>
        <translation>CSV, поля в кавычках</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="79"/>
        <source>Show all</source>
        <translation>Показать все</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="80"/>
        <source>Show all and focus on newest</source>
        <translation>Показать все и перейти к новейшим</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="82"/>
        <source>Show visible in main view</source>
        <translation>Показать видимые в главном окне</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="708"/>
        <source>Decoder:</source>
        <translation>Decoder:</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="735"/>
        <source>Filter rows</source>
        <translation>Фильтр строк</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="774"/>
        <source>Colors</source>
        <translation>Цвета</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="775"/>
        <source>Colored annotation rows</source>
        <translation>Цветные строки аннотаций</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="781"/>
        <source>&amp;Save</source>
        <translation>&amp;Сохранить</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1110"/>
        <source>%1 rows</source>
        <translation>%1 строк</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1111"/>
        <source>%1 of %2 rows</source>
        <translation>%1 из %2 строк</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1146"/>
        <source>Save Annotations as CSV</source>
        <translation>Сохранить аннотации в CSV</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1146"/>
        <source>CSV Files (*.csv);;Text Files (*.txt);;All Files (*)</source>
        <translation>Файлы CSV (*.csv);;Текстовые файлы (*.txt);;Все файлы (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1214"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1214"/>
        <source>File %1 could not be written to.</source>
        <translation>Не удалось записать в файл %1.</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::AnalogSignal</name>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="890"/>
        <source>Number of pos vertical divs</source>
        <translation>Число положительных делений по вертикали</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="898"/>
        <source>Number of neg vertical divs</source>
        <translation>Число отрицательных делений по вертикали</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="903"/>
        <source> pixels</source>
        <translation> пикселей</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="907"/>
        <source>Div height</source>
        <translation>Высота деления</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="926"/>
        <source>V/div</source>
        <translation>В/дел</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="930"/>
        <source>Vertical resolution</source>
        <translation>Разрешение по вертикали</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="939"/>
        <source>Autoranging</source>
        <translation>Автоматический выбор диапазона</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="944"/>
        <source>none</source>
        <translation>нет</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="946"/>
        <source>to logic via threshold</source>
        <translation>в логический по threshold</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="948"/>
        <source>to logic via schmitt-trigger</source>
        <translation>в логический по schmitt-trigger</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="954"/>
        <source>Conversion</source>
        <translation>Преобразование</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="963"/>
        <source>Conversion threshold(s)</source>
        <translation>Порог(и) преобразования</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="973"/>
        <source>analog</source>
        <translation>аналоговый</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="974"/>
        <source>converted</source>
        <translation>преобразованный</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="975"/>
        <source>analog+converted</source>
        <translation>аналоговый+преобразованный</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="980"/>
        <source>Show traces for</source>
        <translation>Показывать сигналы</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Cursor</name>
    <message>
        <location filename="../pv/views/trace/cursor.cpp" line="105"/>
        <source>Disable snapping</source>
        <translation>Отключить привязку</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::CursorPair</name>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="131"/>
        <source>Display interval</source>
        <translation>Показывать интервал</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="143"/>
        <source>Display frequency</source>
        <translation>Показывать частоту</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="155"/>
        <source>Display samples</source>
        <translation>Показывать выборки</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::DecodeTrace</name>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="480"/>
        <source>&lt;p&gt;&lt;i&gt;No decoders in the stack&lt;/i&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;i&gt;В стеке нет decoders&lt;/i&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="491"/>
        <source>&lt;i&gt;* Required channels&lt;/i&gt;</source>
        <translation>&lt;i&gt;* Обязательные каналы&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="495"/>
        <source>Stack Decoder</source>
        <translation>Стек decoder</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="496"/>
        <source>Stack a higher-level decoder on top of this one</source>
        <translation>Добавить поверх этого decoder более высокого уровня</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="598"/>
        <source>Delete</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="640"/>
        <source>Resume decoding</source>
        <translation>Продолжить декодирование</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="646"/>
        <source>Pause decoding</source>
        <translation>Приостановить декодирование</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="653"/>
        <source>Copy annotation text to clipboard</source>
        <translation>Скопировать текст аннотации в буфер обмена</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="661"/>
        <source>Export all annotations</source>
        <translation>Экспортировать все аннотации</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="667"/>
        <source>Export all annotations for this row</source>
        <translation>Экспортировать все аннотации этой строки</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="675"/>
        <source>Export all annotations, starting here</source>
        <translation>Экспортировать все аннотации, начиная отсюда</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="681"/>
        <source>Export annotations for this row, starting here</source>
        <translation>Экспортировать аннотации этой строки, начиная отсюда</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="689"/>
        <source>Export all annotations within cursor range</source>
        <translation>Экспортировать все аннотации в диапазоне курсоров</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="695"/>
        <source>Export annotations for this row within cursor range</source>
        <translation>Экспортировать аннотации этой строки в диапазоне курсоров</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1187"/>
        <source>%1:
%2</source>
        <translation>%1:
%2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1231"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2) %3</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2) %3</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1300"/>
        <source>Export annotations</source>
        <translation>Экспортировать аннотации</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1300"/>
        <source>Text Files (*.txt);;All Files (*)</source>
        <translation>Текстовые файлы (*.txt);;Все файлы (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1365"/>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1374"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1365"/>
        <source>File %1 could not be written to.</source>
        <translation>Не удалось записать в файл %1.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1434"/>
        <source>Show this row</source>
        <translation>Показать эту строку</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1445"/>
        <source>Show All</source>
        <translation>Показать все</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1453"/>
        <source>Hide All</source>
        <translation>Скрыть все</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Flag</name>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="146"/>
        <source>Text</source>
        <translation>Текст</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="155"/>
        <source>Delete</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="160"/>
        <source>Disable snapping</source>
        <translation>Отключить привязку</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Header</name>
    <message>
        <location filename="../pv/views/trace/header.cpp" line="138"/>
        <source>Group</source>
        <translation>Группировать</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::LogicSignal</name>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="553"/>
        <source>No trigger</source>
        <translation>Без trigger</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="566"/>
        <source>Trigger on rising edge</source>
        <translation>Trigger по фронту</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="579"/>
        <source>Trigger on high level</source>
        <translation>Trigger по высокому уровню</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="592"/>
        <source>Trigger on falling edge</source>
        <translation>Trigger по спаду</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="605"/>
        <source>Trigger on low level</source>
        <translation>Trigger по низкому уровню</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="618"/>
        <source>Trigger on rising or falling edge</source>
        <translation>Trigger по фронту или спаду</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="706"/>
        <source> pixels</source>
        <translation> пикселей</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="710"/>
        <source>Trace height</source>
        <translation>Высота сигнала</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="773"/>
        <source>Trigger</source>
        <translation>Trigger</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::MathEditDialog</name>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="90"/>
        <source>Math Expression Editor</source>
        <translation>Редактор математических выражений</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="95"/>
        <source>Inputs:</source>
        <translation>Входы:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="101"/>
        <source>Variables:</source>
        <translation>Переменные:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="103"/>
        <source>Basic operators:</source>
        <translation>Базовые операторы:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="110"/>
        <source>Assignments:</source>
        <translation>Присваивания:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="120"/>
        <source>General purpose functions:</source>
        <translation>Общие функции:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="121"/>
        <source>abs(x)		Absolute value of x</source>
        <translation>abs(x)		Абсолютное значение x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="122"/>
        <source>avg(x, y, ...)	Average of all input values</source>
        <translation>avg(x, y, ...)	Среднее всех входных значений</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="123"/>
        <source>ceil(x)		Smallest integer that is greater than or equal to x</source>
        <translation>ceil(x)		Наименьшее целое, не меньшее x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="124"/>
        <source>clamp(lb, x, ub)	Clamp x in range between lb and ub, where lb &lt; ub</source>
        <translation>clamp(lb, x, ub)	Ограничивает x диапазоном от lb до ub, где lb &lt; ub</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="125"/>
        <source>equal(x, y)	Equality test between x and y using normalised epsilon</source>
        <translation>equal(x, y)	Проверка равенства x и y с нормализованным epsilon</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="126"/>
        <source>erf(x)		Error function of x</source>
        <translation>erf(x)		Функция ошибок от x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="127"/>
        <source>erfc(x)		Complimentary error function of x</source>
        <translation>erfc(x)		Дополнительная функция ошибок от x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="128"/>
        <source>exp(x)		e to the power of x</source>
        <translation>exp(x)		e в степени x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="129"/>
        <source>expm1(x)	e to the power of x minus 1, where x is very small.</source>
        <translation>expm1(x)	e в степени x минус 1, для очень малых x.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="130"/>
        <source>floor(x)		Largest integer that is less than or equal to x</source>
        <translation>floor(x)		Наибольшее целое, не превышающее x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="131"/>
        <source>frac(x)		Fractional portion of x</source>
        <translation>frac(x)		Дробная часть x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="132"/>
        <source>hypot(x)		Hypotenuse of x and y (i.e. sqrt(x*x + y*y))</source>
        <translation>hypot(x)		Гипотенуза x и y (то есть sqrt(x*x + y*y))</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="133"/>
        <source>iclamp(lb, x, ub)	Inverse-clamp x outside of the range lb and ub, where lb &lt; ub.
		If x is within the range it will snap to the closest bound</source>
        <translation>iclamp(lb, x, ub)	Обратное ограничение: выводит x за диапазон lb и ub, где lb &lt; ub.
		Если x внутри диапазона, он притягивается к ближайшей границе</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="134"/>
        <source>inrange(lb, x, ub)	In-range returns true when x is within the range lb and ub, where lb &lt; ub.</source>
        <translation>inrange(lb, x, ub)	In-range возвращает истину, когда x находится в диапазоне lb и ub, где lb &lt; ub.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="135"/>
        <source>log(x)		Natural logarithm of x</source>
        <translation>log(x)		Натуральный логарифм x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="136"/>
        <source>log10(x)		Base 10 logarithm of x</source>
        <translation>log10(x)		Логарифм x по основанию 10</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="140"/>
        <source>log1p(x)		Natural logarithm of 1 + x, where x is very small</source>
        <translation>log1p(x)		Натуральный логарифм 1 + x, для очень малых x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="141"/>
        <source>log2(x)		Base 2 logarithm of x</source>
        <translation>log2(x)		Логарифм x по основанию 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="142"/>
        <source>logn(x)		Base N logarithm of x, where n is a positive integer</source>
        <translation>logn(x)		Логарифм x по основанию N, где n — положительное целое</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="143"/>
        <source>max(x, y, ...)	Largest value of all the inputs</source>
        <translation>max(x, y, ...)	Наибольшее из всех входных значений</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="144"/>
        <source>min(x, y, ...)	Smallest value of all the inputs</source>
        <translation>min(x, y, ...)	Наименьшее из всех входных значений</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="145"/>
        <source>mul(x, y, ...)	Product of all the inputs</source>
        <translation>mul(x, y, ...)	Произведение всех входных значений</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="146"/>
        <source>ncdf(x)		Normal cumulative distribution function</source>
        <translation>ncdf(x)		Функция распределения нормального закона</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="147"/>
        <source>nequal(x, y)	Not-equal test between x and y using normalised epsilon</source>
        <translation>nequal(x, y)	Проверка неравенства x и y с нормализованным epsilon</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="148"/>
        <source>pow(x, y)	x to the power of y</source>
        <translation>pow(x, y)	x в степени y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="149"/>
        <source>root(x, n)	Nth-Root of x, where n is a positive integer</source>
        <translation>root(x, n)	Корень n-й степени из x, где n — положительное целое</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="150"/>
        <source>round(x)		Round x to the nearest integer</source>
        <translation>round(x)		Округляет x до ближайшего целого</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="151"/>
        <source>roundn(x, n)	Round x to n decimal places, where n &gt; 0 and is an integer</source>
        <translation>roundn(x, n)	Округляет x до n знаков после запятой, где n &gt; 0 и целое</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="152"/>
        <source>sgn(x)		Sign of x; -1 if x &lt; 0, +1 if x &gt; 0, else zero</source>
        <translation>sgn(x)		Знак x; -1 если x &lt; 0, +1 если x &gt; 0, иначе ноль</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="153"/>
        <source>sqrt(x)		Square root of x, where x &gt;= 0</source>
        <translation>sqrt(x)		Квадратный корень из x, где x &gt;= 0</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="154"/>
        <source>sum(x, y, ..,)	Sum of all the inputs</source>
        <translation>sum(x, y, ..,)	Сумма всех входных значений</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="155"/>
        <source>swap(x, y)	Swap the values of the variables x and y and return the current value of y</source>
        <translation>swap(x, y)	Меняет значения переменных x и y местами и возвращает текущее значение y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="156"/>
        <source>trunc(x)		Integer portion of x</source>
        <translation>trunc(x)		Целая часть x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="160"/>
        <source>Trigonometry functions:</source>
        <translation>Тригонометрические функции:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="161"/>
        <source>acos(x)		Arc cosine of x expressed in radians. Interval [-1,+1]</source>
        <translation>acos(x)		Арккосинус x в радианах. Интервал [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="162"/>
        <source>acosh(x)		Inverse hyperbolic cosine of x expressed in radians</source>
        <translation>acosh(x)		Обратный гиперболический косинус x в радианах</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="163"/>
        <source>asin(x)		Arc sine of x expressed in radians. Interval [-1,+1]</source>
        <translation>asin(x)		Арксинус x в радианах. Интервал [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="164"/>
        <source>asinh(x)		Inverse hyperbolic sine of x expressed in radians</source>
        <translation>asinh(x)		Обратный гиперболический синус x в радианах</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="165"/>
        <source>atan(x)		Arc tangent of x expressed in radians. Interval [-1,+1]</source>
        <translation>atan(x)		Арктангенс x в радианах. Интервал [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="166"/>
        <source>atan2(x, y)	Arc tangent of (x / y) expressed in radians. [-pi,+pi]  </source>
        <translation>atan2(x, y)	Арктангенс (x / y) в радианах. [-pi,+pi]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="167"/>
        <source>atanh(x)		Inverse hyperbolic tangent of x expressed in radians</source>
        <translation>atanh(x)		Обратный гиперболический тангенс x в радианах</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="168"/>
        <source>cos(x)		Cosine of x</source>
        <translation>cos(x)		Косинус x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="169"/>
        <source>cosh(x)		Hyperbolic cosine of x</source>
        <translation>cosh(x)		Гиперболический косинус x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="170"/>
        <source>cot(x)		Cotangent of x</source>
        <translation>cot(x)		Котангенс x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="171"/>
        <source>csc(x)		Cosectant of x</source>
        <translation>csc(x)		Косеканс x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="172"/>
        <source>sec(x)		Secant of x</source>
        <translation>sec(x)		Секанс x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="173"/>
        <source>sin(x)		Sine of x</source>
        <translation>sin(x)		Синус x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="174"/>
        <source>sinc(x)		Sine cardinal of x</source>
        <translation>sinc(x)		Кардинальный синус x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="175"/>
        <source>sinh(x)		Hyperbolic sine of x</source>
        <translation>sinh(x)		Гиперболический синус x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="176"/>
        <source>tan(x)		Tangent of x</source>
        <translation>tan(x)		Тангенс x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="177"/>
        <source>tanh(x)		Hyperbolic tangent of x</source>
        <translation>tanh(x)		Гиперболический тангенс x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="178"/>
        <source>deg2rad(x)	Convert x from degrees to radians</source>
        <translation>deg2rad(x)	Переводит x из градусов в радианы</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="179"/>
        <source>deg2grad(x)	Convert x from degrees to gradians</source>
        <translation>deg2grad(x)	Переводит x из градусов в грады</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="180"/>
        <source>rad2deg(x)	Convert x from radians to degrees</source>
        <translation>rad2deg(x)	Переводит x из радиан в градусы</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="181"/>
        <source>grad2deg(x)	Convert x from gradians to degrees</source>
        <translation>grad2deg(x)	Переводит x из градов в градусы</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="185"/>
        <source>Logic operators:</source>
        <translation>Логические операторы:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="202"/>
        <source>Comparisons:</source>
        <translation>Сравнения:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="203"/>
        <source>x = y or x == y	True only if x is strictly equal to y</source>
        <translation>x = y или x == y	Истина, только если x строго равен y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="204"/>
        <source>x &lt;&gt; y or x != y	True only if x does not equal y</source>
        <translation>x &lt;&gt; y или x != y	Истина, только если x не равен y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="205"/>
        <source>x &lt; y		True only if x is less than y</source>
        <translation>x &lt; y		Истина, только если x меньше y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="206"/>
        <source>x &lt;= y		True only if x is less than or equal to y</source>
        <translation>x &lt;= y		Истина, только если x меньше или равен y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="207"/>
        <source>x &gt; y		True only if x is greater than y</source>
        <translation>x &gt; y		Истина, только если x больше y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="208"/>
        <source>x &gt;= y		True only if x is greater than or equal to y</source>
        <translation>x &gt;= y		Истина, только если x больше или равен y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="209"/>
        <source>Flow control:</source>
        <translation>Управление ходом выполнения:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="210"/>
        <source>{ ... }		Beginning and end of instruction block</source>
        <translation>{ ... }		Начало и конец блока инструкций</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="211"/>
        <source>if (x, y, z)	If x is true then return y else return z
if (x) y;			variant without implied else
if (x) { y };		variant with an instruction block
if (x) y; else z;		variant with explicit else
if (x) { y } else { z };	variant with instruction blocks</source>
        <translation>if (x, y, z)	Если x истинно, вернуть y, иначе вернуть z
if (x) y;			вариант без неявного else
if (x) { y };		вариант с блоком инструкций
if (x) y; else z;		вариант с явным else
if (x) { y } else { z };	вариант с блоками инструкций</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="212"/>
        <source>x ? y : z	Ternary operator, equivalent to &apos;if (x, y, z)&apos;</source>
        <translation>x ? y : z	Тернарный оператор, эквивалентен &apos;if (x, y, z)&apos;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="213"/>
        <source>switch {		The first true case condition that is encountered will
 case x &gt; 1: a;	determine the result of the switch. If none of the case
 case x &lt; 1: b;	conditions hold true, the default action is used
 default:     c;	to determine the return value
}</source>
        <translation>switch {		Первое выполнившееся условие case определяет
 case x &gt; 1: a;	результат switch. Если ни одно из условий
 case x &lt; 1: b;	case не выполняется, возвращаемое значение
 default:     c;	определяет ветвь default
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="217"/>
        <source>while (conditon) {	Evaluates expression repeatedly as long as condition is true,
 expression;	returning the last value of expression
}</source>
        <translation>while (conditon) {	Многократно вычисляет выражение, пока условие истинно,
 expression;	возвращая последнее значение выражения
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="218"/>
        <source>repeat		Evalues expression repeatedly as long as condition is false,
 expression;	returning the last value of expression
until (condition)
</source>
        <translation>repeat		Многократно вычисляет выражение, пока условие ложно,
 expression;	возвращая последнее значение выражения
until (condition)
</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="219"/>
        <source>for (var x := 0; condition; x += 1) {	Repeatedly evaluates expression while the condition is true,
 expression			while evaluating the &apos;increment&apos; expression on each loop
}</source>
        <translation>for (var x := 0; condition; x += 1) {	Многократно вычисляет выражение, пока условие истинно,
 expression			вычисляя выражение &apos;increment&apos; на каждой итерации
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="220"/>
        <source>break		Terminates the execution of the nearest enclosed loop, returning NaN</source>
        <translation>break		Прерывает выполнение ближайшего цикла и возвращает NaN</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="221"/>
        <source>break[x]		Terminates the execution of the nearest enclosed loop, returning x</source>
        <translation>break[x]		Прерывает выполнение ближайшего цикла и возвращает x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="222"/>
        <source>continue		Interrupts loop execution and resumes with the next loop iteration</source>
        <translation>continue		Прерывает текущую итерацию и переходит к следующей</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="223"/>
        <source>return[x]		Returns immediately from within the current expression, returning x</source>
        <translation>return[x]		Немедленно возвращается из текущего выражения, возвращая x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="224"/>
        <source>~(expr; expr; ...)	Evaluates each sub-expression and returns the value of the last one
~{expr; expr; ...}</source>
        <translation>~(expr; expr; ...)	Вычисляет каждое подвыражение и возвращает значение последнего
~{expr; expr; ...}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="236"/>
        <source>Copy to expression</source>
        <translation>Копировать в выражение</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="249"/>
        <source>Basics</source>
        <translation>Основы</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="250"/>
        <source>Functions 1</source>
        <translation>Функции 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="251"/>
        <source>Functions 2</source>
        <translation>Функции 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="252"/>
        <source>Trigonometry</source>
        <translation>Тригонометрия</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="253"/>
        <source>Logic</source>
        <translation>Логика</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="254"/>
        <source>Flow Control 1</source>
        <translation>Управление 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="255"/>
        <source>Flow Control 2</source>
        <translation>Управление 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="256"/>
        <source>Examples</source>
        <translation>Примеры</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::MathSignal</name>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="315"/>
        <source>Expression</source>
        <translation>Выражение</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="319"/>
        <location filename="../pv/views/trace/mathsignal.cpp" line="329"/>
        <source>same as session</source>
        <translation>как в сеансе</translation>
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
        <translation>Число выборок</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="333"/>
        <source>Sample rate</source>
        <translation>Sample rate</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Ruler</name>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="165"/>
        <source>Create marker here</source>
        <translation>Создать маркер здесь</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="169"/>
        <source>Set as zero point</source>
        <translation>Задать как нулевую точку</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="174"/>
        <source>Reset zero point</source>
        <translation>Сбросить нулевую точку</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="187"/>
        <source>Disable mouse hover marker</source>
        <translation>Отключить маркер наведения мыши</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="187"/>
        <source>Enable mouse hover marker</source>
        <translation>Включить маркер наведения мыши</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Signal</name>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="155"/>
        <source>Name</source>
        <translation>Имя</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="169"/>
        <source>Remove</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="171"/>
        <source>Disable</source>
        <translation>Отключить</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::StandardBar</name>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="174"/>
        <source>Zoom &amp;In</source>
        <translation>&amp;Увеличить</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="175"/>
        <source>Zoom &amp;Out</source>
        <translation>&amp;Уменьшить</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="176"/>
        <location filename="../pv/views/trace/standardbar.cpp" line="177"/>
        <source>Zoom to &amp;Fit</source>
        <translation>По &amp;размеру</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="178"/>
        <source>Show &amp;Cursors</source>
        <translation>Показать &amp;курсоры</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="179"/>
        <source>Show Cursors</source>
        <translation>Показать курсоры</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="180"/>
        <source>Display last segment only</source>
        <translation>Показывать только последний сегмент</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="181"/>
        <source>Display last complete segment only</source>
        <translation>Показывать только последний полный сегмент</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="182"/>
        <source>Display a single segment</source>
        <translation>Показывать один сегмент</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TimeMarker</name>
    <message>
        <location filename="../pv/views/trace/timemarker.cpp" line="223"/>
        <source>Time</source>
        <translation>Время</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Trace</name>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="238"/>
        <source>Create marker here</source>
        <translation>Создать маркер здесь</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="400"/>
        <source>Color</source>
        <translation>Цвет</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="465"/>
        <source>Name</source>
        <translation>Имя</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TraceGroup</name>
    <message>
        <location filename="../pv/views/trace/tracegroup.cpp" line="144"/>
        <source>Ungroup</source>
        <translation>Разгруппировать</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Viewport</name>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="303"/>
        <location filename="../pv/views/trace/viewport.cpp" line="349"/>
        <source>Width: </source>
        <translation>Ширина: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="350"/>
        <source>Period: </source>
        <translation>Период: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="351"/>
        <source>Frequency: </source>
        <translation>Частота: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="352"/>
        <source>Duty Cycle: </source>
        <translation>Коэффициент заполнения: </translation>
    </message>
</context>
<context>
    <name>pv::widgets::DecoderGroupBox</name>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="48"/>
        <source>Show/hide this decoder trace</source>
        <translation>Показать/скрыть эту дорожку decoder</translation>
    </message>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="58"/>
        <source>Delete this decoder trace</source>
        <translation>Удалить эту дорожку decoder</translation>
    </message>
</context>
<context>
    <name>pv::widgets::DeviceToolButton</name>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="83"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="339"/>
        <source>Refresh Device List</source>
        <translation>Обновить список устройств</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="89"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="105"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="112"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="221"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="228"/>
        <source>&lt;No Device&gt;</source>
        <translation>&lt;Нет устройства&gt;</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="298"/>
        <source>This device is in use by another session</source>
        <translation>Это устройство используется другим сеансом</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="311"/>
        <source>This device is already open in another session</source>
        <translation>Это устройство уже открыто в другом сеансе</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="312"/>
        <source>Open this device in a new session</source>
        <translation>Открыть это устройство в новом сеансе</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ExportMenu</name>
    <message>
        <location filename="../pv/widgets/exportmenu.cpp" line="73"/>
        <source>Export %1...</source>
        <translation>Экспорт %1...</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ImportMenu</name>
    <message>
        <location filename="../pv/widgets/importmenu.cpp" line="70"/>
        <source>Import %1...</source>
        <translation>Импорт %1...</translation>
    </message>
</context>
</TS>
