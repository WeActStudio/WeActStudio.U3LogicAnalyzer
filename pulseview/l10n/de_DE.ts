<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE">
<context>
    <name>QApplication</name>
    <message>
        <location filename="../pv/devices/device.cpp" line="72"/>
        <source>Querying config key %1 is not allowed</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Das Abfragen des Konfigurationsschlüssels %1 ist nicht erlaubt</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="81"/>
        <source>Querying config key %1 resulted in %2</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Das Abfragen des Konfigurationsschlüssels %1 ergab %2</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="95"/>
        <source>Unknown type supplied when attempting to query %1</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Unbekannter Typ beim Abfragen von %1</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="342"/>
        <source>Error when scanning device driver &apos;%1&apos;: %2</source>
        <translation>Fehler beim Suchen nach dem Gerätetreiber &apos;%1&apos;: %2</translation>
    </message>
</context>
<context>
    <name>QHexView</name>
    <message>
        <location filename="../pv/views/decoder_binary/QHexView.cpp" line="347"/>
        <source>No data available</source>
        <translation>Keine Daten vorhanden</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../pv/devicemanager.cpp" line="81"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="112"/>
        <source>Scanning for devices that driver %1 can access...</source>
        <translation>Suche nach Geräten, die von Treiber %1 angesprochen werden können...</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="142"/>
        <source>Stack trace of previous crash:</source>
        <translation>Stack-Ablauf des vorherigen Absturzes:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="156"/>
        <source>Don&apos;t show this message again</source>
        <translation>Diese Meldung in Zukunft nicht mehr anzeigen</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="159"/>
        <source>When %1 last crashed, it created a stack trace.
A human-readable form has been saved to disk and was written to the log. You may access it from the settings dialog.</source>
        <translation>Beim letzten Absturz von %1 wurde ein Stack-Ablauf erzeugt.
Eine lesbare Fassung wurde auf der Festplatte und im Protokoll abgelegt. Sie ist im Einstellungsdialog erreichbar.</translation>
    </message>
</context>
<context>
    <name>pv::MainWindow</name>
    <message>
        <location filename="../pv/mainwindow.cpp" line="534"/>
        <location filename="../pv/mainwindow.cpp" line="1333"/>
        <source>Decoder Selector</source>
        <translation>Protokolldekoder</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="601"/>
        <source>Session %1</source>
        <translation>Analysesitzung %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="202"/>
        <location filename="../pv/mainwindow.cpp" line="785"/>
        <location filename="../pv/mainwindow.cpp" line="1548"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="171"/>
        <location filename="../pv/mainwindow.cpp" line="748"/>
        <location filename="../pv/mainwindow.cpp" line="1528"/>
        <source>Warning</source>
        <translation>Warnung</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="192"/>
        <source>Current available memory size is less than 4GB and will affect usage effect. 
Do you want to continue to use?</source>
        <translation>Der verfügbare Arbeitsspeicher ist kleiner als 4 GB, was die Nutzung beeinträchtigt. 
Möchten Sie trotzdem fortfahren?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="232"/>
        <source>No udev access rules were found for:
%1

Without them the device cannot be opened by an unprivileged user, so it will not be detected.

Enter your administrator password to install:
%2</source>
        <translation>Es wurden keine udev-Zugriffsregeln gefunden für:
%1

Ohne sie kann das Gerät von einem Benutzer ohne Administrationsrechte nicht geöffnet und daher nicht erkannt werden.

Geben Sie das Administratorkennwort ein, um sie zu installieren:
%2</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="243"/>
        <source>Administrator privileges required</source>
        <translation>Administratorrechte erforderlich</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="252"/>
        <source>Access rules installed</source>
        <translation>Zugriffsregeln installiert</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="253"/>
        <source>The udev rules have been installed.

If the device is already plugged in, unplug and replug it before scanning again.</source>
        <translation>Die udev-Regeln wurden installiert.

Ist das Gerät bereits angeschlossen, ziehen Sie es ab und stecken Sie es vor dem nächsten Suchen erneut an.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="260"/>
        <source>Installation failed</source>
        <translation>Installation fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="261"/>
        <source>Could not install the udev rules:

%1

Please try again, or cancel to skip.</source>
        <translation>Die udev-Regeln konnten nicht installiert werden:

%1

Bitte erneut versuchen oder abbrechen, um diesen Schritt zu überspringen.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="267"/>
        <source>Skipped installing access rules</source>
        <translation>Installation der Zugriffsregeln übersprungen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="268"/>
        <source>The udev rules are not installed, so the device may not be detected.

You can run this application as root later, or copy the rule files from the repository to /etc/udev/rules.d/ and replug the device.</source>
        <translation>Die udev-Regeln sind nicht installiert, daher wird das Gerät möglicherweise nicht erkannt.

Sie können dieses Programm später als root starten oder die Regeldateien aus dem Repository nach /etc/udev/rules.d/ kopieren und das Gerät erneut anschließen.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="773"/>
        <source>device detached!
Do you want to save the session?</source>
        <translation>Gerät getrennt!
Möchten Sie die Sitzung speichern?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="784"/>
        <source>Save</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="978"/>
        <source>Device scan timed out</source>
        <translation>Zeitüberschreitung bei der Gerätesuche</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="979"/>
        <source>Scanning these drivers took too long, so the software started without waiting for them:</source>
        <translation>Die Suche nach diesen Treibern dauerte zu lange, daher wurde ohne sie gestartet:</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="981"/>
        <source>Their devices are not in the list. Unplug and replug the analyzer - or close whatever else is using it - and scan again.</source>
        <translation>Ihre Geräte stehen nicht in der Liste. Stecken Sie den Analyzer ab und wieder an – oder beenden Sie das andere Programm, das ihn verwendet – und suchen Sie erneut.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1316"/>
        <location filename="../pv/mainwindow.cpp" line="1383"/>
        <source>Start/Stop Acquisition</source>
        <translation>Datenerfassung starten/stoppen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1319"/>
        <location filename="../pv/mainwindow.cpp" line="1394"/>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1457"/>
        <location filename="../pv/mainwindow.cpp" line="1642"/>
        <source>Reload</source>
        <translation>Neu laden</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1547"/>
        <source>Discard &amp;&amp; Exit</source>
        <translation>Verwerfen &amp;&amp; Beenden</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1592"/>
        <source>Capture Failed</source>
        <translation>Aufnahme fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1657"/>
        <source>Device stopped sending data</source>
        <translation>Gerät sendet keine Daten mehr</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1658"/>
        <source>The device stopped sending samples before the capture was due to end, usually because the USB link cannot sustain the configured %1.

The acquisition was stopped. The data captured before that may be incomplete or wrong. Try a lower sample rate, fewer channels, or another USB port.</source>
        <translation>Das Gerät sendete keine Samples mehr, bevor die Aufnahme hätte enden sollen – meist weil die USB-Verbindung die eingestellten %1 nicht durchhält.

Die Aufnahme wurde gestoppt. Die bis dahin erfassten Daten sind möglicherweise unvollständig oder falsch. Versuchen Sie eine niedrigere Abtastrate, weniger Kanäle oder einen anderen USB-Anschluss.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1899"/>
        <location filename="../pv/mainwindow.cpp" line="1925"/>
        <source>This session contains unsaved data. Close anyway?</source>
        <translation>Diese Sitzung enthält nicht gespeicherte Daten. Trotzdem schließen?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1457"/>
        <location filename="../pv/mainwindow.cpp" line="1465"/>
        <location filename="../pv/mainwindow.cpp" line="1642"/>
        <source>Run</source>
        <translation>Starten</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="201"/>
        <location filename="../pv/mainwindow.cpp" line="1898"/>
        <location filename="../pv/mainwindow.cpp" line="1924"/>
        <source>Confirm</source>
        <translation>Bestätigen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1317"/>
        <location filename="../pv/mainwindow.cpp" line="1318"/>
        <location filename="../pv/mainwindow.cpp" line="1386"/>
        <location filename="../pv/mainwindow.cpp" line="1387"/>
        <source>Repeat Acquisition</source>
        <translation>Aufnahme wiederholen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1471"/>
        <source>Stop</source>
        <translation>Stoppen</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1521"/>
        <source>Untitled</source>
        <translation>Unbenannt</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1540"/>
        <source>&lt;h3&gt;Unsaved Capture Data&lt;/h3&gt;&lt;p&gt;Data will be permanently lost on exit.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Nicht gespeicherte Erfassungsdaten&lt;/h3&gt;&lt;p&gt;Daten gehen beim Beenden dauerhaft verloren.&lt;/p&gt;</translation>
    </message>
</context>
<context>
    <name>pv::Session</name>
    <message>
        <location filename="../pv/session.cpp" line="797"/>
        <source>Failed to open device</source>
        <translation>Fehler beim Öffnen des Gerätes</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="924"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="475"/>
        <source>Can&apos;t restore generated signal of unknown type %1 (%2)</source>
        <translation>Erzeugtes Signal unbekannten Typs %1 (%2) kann nicht wiederhergestellt werden</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="702"/>
        <source>this feature is currently not supported.</source>
        <translation>diese Funktion wird derzeit nicht unterstützt.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="791"/>
        <source>The device is already in use, or this user has no access to it. Close the program that holds it, then try again.</source>
        <translation>Das Gerät wird bereits verwendet, oder dieser Benutzer hat keinen Zugriff darauf. Beenden Sie das Programm, das es belegt, und versuchen Sie es erneut.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="794"/>
        <source>The device could not be opened.</source>
        <translation>Das Gerät konnte nicht geöffnet werden.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="925"/>
        <source>Unexpected input format: %1</source>
        <translation>Unerwartetes Importformat: %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="961"/>
        <source>Failed to load %1</source>
        <translation>Fehler beim Laden von %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1003"/>
        <source>No active device set, can&apos;t start acquisition.</source>
        <translation>Kein Gerät aktiv, kann Datenerfassung nicht starten.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1018"/>
        <source>No channels enabled.</source>
        <translation>Keine aktiven Kanäle vorhanden.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="2088"/>
        <source>Can&apos;t handle more than 64 logic channels.</source>
        <translation>Es können nicht mehr als 64 Logikkanäle behandelt werden.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="697"/>
        <source>Out of memory, acquisition stopped.</source>
        <translation>Nicht genügend Arbeitsspeicher vorhanden, Datenerfassung wurde gestoppt.</translation>
    </message>
</context>
<context>
    <name>pv::StoreSession</name>
    <message>
        <location filename="../pv/storesession.cpp" line="116"/>
        <source>Can&apos;t save logic channel without data.</source>
        <translation>Kann Logikkanal nicht speichern, da er keine Daten beinhaltet.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="132"/>
        <source>Can&apos;t save analog channel without data.</source>
        <translation>Kann Analogkanal nicht speichern, da er keine Daten beinhaltet.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="144"/>
        <source>No channels enabled.</source>
        <translation>Keine Kanäle aktiviert.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="172"/>
        <source>Can&apos;t save range without sample data.</source>
        <translation>In dem gewählten Bereich befinden sich keine Daten zum Speichern.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="193"/>
        <location filename="../pv/storesession.cpp" line="300"/>
        <location filename="../pv/storesession.cpp" line="315"/>
        <source>Error while saving: </source>
        <translation>Fehler beim Speichern: </translation>
    </message>
</context>
<context>
    <name>pv::binding::Device</name>
    <message>
        <location filename="../pv/binding/device.cpp" line="84"/>
        <source>Note for device developers: Ignoring device configuration capability &apos;%1&apos; as it is missing GET and/or SET</source>
        <translation>Hinweis für Geräteentwickler: Die Gerätekonfiguration &apos;%1&apos; wird ignoriert, da GET und/oder SET fehlen</translation>
    </message>
    <message>
        <location filename="../pv/binding/device.cpp" line="109"/>
        <source>No Limit</source>
        <translation>Kein Limit</translation>
    </message>
</context>
<context>
    <name>pv::data::DecodeSignal</name>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="244"/>
        <source>No decoders</source>
        <translation>Keine Protokolldekoder</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="251"/>
        <source>There are no channels assigned to this decoder</source>
        <translation>Dem Protokolldekoder sind keine Kanäle zugeordnet</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="265"/>
        <source>One or more required channels have not been specified</source>
        <translation>Mindestens ein notwendiger Kanal wurde noch nicht zugeordnet</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="281"/>
        <source>No input data</source>
        <translation>Keine Daten zum Auswerten vorhanden</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="1617"/>
        <location filename="../pv/data/decodesignal.cpp" line="1702"/>
        <source>Decoder reported an error</source>
        <translation>Protokolldekoder meldet Fehler</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="1908"/>
        <source>Failed to create decoder instance</source>
        <translation>Fehler beim Erzeugen des Protokolldekoders</translation>
    </message>
</context>
<context>
    <name>pv::data::MathSignal</name>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="107"/>
        <source>Math%1</source>
        <translation>Mathe%1</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="306"/>
        <source>No expression defined, nothing to do</source>
        <translation>Kein Ausdruck definiert, nichts zu tun</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="345"/>
        <source>%1 at line %2, column %3: %4</source>
        <translation>%1 in Zeile %2, Spalte %3: %4</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="364"/>
        <location filename="../pv/data/mathsignal.cpp" line="536"/>
        <source>&quot;%1&quot; isn&apos;t a valid analog signal</source>
        <translation>&quot;%1&quot; ist kein gültiges analoges Signal</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="374"/>
        <location filename="../pv/data/mathsignal.cpp" line="611"/>
        <source>No data will be generated as %1 must be enabled</source>
        <translation>Es werden keine Daten erzeugt, da %1 aktiviert sein muss</translation>
    </message>
</context>
<context>
    <name>pv::data::SignalBase</name>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="521"/>
        <source>Signal average</source>
        <translation>Durchschnittlicher Signalpegel</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="522"/>
        <source>0.9V (for 1.8V CMOS)</source>
        <translation>0.9V (für 1.8V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="523"/>
        <source>1.8V (for 3.3V CMOS)</source>
        <translation>1.8V (für 3.3V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="524"/>
        <source>2.5V (for 5.0V CMOS)</source>
        <translation>2.5V (für 5.0V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="525"/>
        <source>1.5V (for TTL)</source>
        <translation>1.5V (für TTL)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="530"/>
        <source>Signal average +/- 15%</source>
        <translation>Durchschnittlicher Signalpegel +/- 15%</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="531"/>
        <source>0.3V/1.2V (for 1.8V CMOS)</source>
        <translation>0.3V/1.2V (für 1.8V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="532"/>
        <source>0.7V/2.5V (for 3.3V CMOS)</source>
        <translation>0.7V/2.5V (für 3.3V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="533"/>
        <source>1.3V/3.7V (for 5.0V CMOS)</source>
        <translation>1.3V/3.7V (für 5.0V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="534"/>
        <source>0.8V/2.0V (for TTL)</source>
        <translation>0.8V/2.0V (für TTL)</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::Connect</name>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="58"/>
        <source>&amp;Scan for devices using driver above</source>
        <translation>Nach Geräten &amp;suchen, die der ausgewählte Treiber ansprechen kann</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="63"/>
        <source>Connect to Device</source>
        <translation>Mit Gerät verbinden</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="75"/>
        <source>Step 1: Choose the driver</source>
        <translation>Schritt 1: Treiber auswählen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="79"/>
        <source>&amp;USB</source>
        <translation>&amp;USB</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="80"/>
        <source>Serial &amp;Port</source>
        <translation>Serielle Sch&amp;nittstelle</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="81"/>
        <source>&amp;TCP/IP</source>
        <translation>&amp;TCP/IP</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="116"/>
        <source>Protocol:</source>
        <translation>Protokoll:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="134"/>
        <source>Step 2: Choose the interface</source>
        <translation>Schritt 2: Schnittstelle auswählen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="140"/>
        <source>Step 3: Scan for devices</source>
        <translation>Schritt 3: Nach Geräten suchen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="146"/>
        <source>Step 4: Select the device</source>
        <translation>Schritt 4: Gerät auswählen</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::DecoderChannelDialog</name>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="50"/>
        <source>Decoder Configuration</source>
        <translation>Decoder-Konfiguration</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="92"/>
        <source>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</source>
        <translation>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="105"/>
        <source>Channel Mapping</source>
        <translation>Kanalzuordnung</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="110"/>
        <source>Decoder Options</source>
        <translation>Decoder-Optionen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="121"/>
        <source>Apply &amp;&amp; Decode</source>
        <translation>Anwenden &amp;&amp; Dekodieren</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="122"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="173"/>
        <source>-- Not assigned --</source>
        <translation>-- Nicht zugeordnet --</translation>
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
        <translation>FW aktualisieren</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="101"/>
        <source>FW File:</source>
        <translation>Firmware Datei:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="103"/>
        <source>Select firmware file in BIN or HEX format</source>
        <translation>Firmware Datei in BIN oder HEX Format auswählen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="105"/>
        <source>Browse</source>
        <translation>suchen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="114"/>
        <source>Ready</source>
        <translation>Listo</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="119"/>
        <source>Update Log:</source>
        <translation>Update registro:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="128"/>
        <source>Start</source>
        <translation>Starten</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="130"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="189"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="156"/>
        <source>Select firmware file</source>
        <translation>Firmware Datei auswählen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="158"/>
        <source>Firmware File (*.bin *.hex);;All Files (*.*)</source>
        <translation>Firmware-Datei (*.bin *.hex);;Alle Dateien (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="172"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="177"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="172"/>
        <source>Please select a firmware file</source>
        <translation>Firmware Datei auswählen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="177"/>
        <source>Firmware file does not exist</source>
        <translation>Firmware Datei existiert nicht</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="237"/>
        <source>Close</source>
        <translation>Cerrar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="243"/>
        <source>Update Success</source>
        <translation>Update erfolgreich</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="245"/>
        <source>Success</source>
        <translation>Éxito</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="245"/>
        <source>Firmware Update Success!
Device will restart.</source>
        <translation>Firmware aktualisierung erfolgreich
Device wird neu gestartet</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="247"/>
        <source>Update Failed</source>
        <translation>Update fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="249"/>
        <source>Fail</source>
        <translation>Fehlerfehl</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="249"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>Firmware aktualisierung fehlgeschlagen
Bitte überprüfen Sie die Verbindung und die Firmware Datei.</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::IAPWorker</name>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="508"/>
        <source>Erasing flash...</source>
        <translation>Flash wird gelöscht...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="548"/>
        <source>Programming flash...</source>
        <translation>Flash wird programmiert...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="608"/>
        <source>Programming flash... %1%</source>
        <translation>Flash wird programmiert... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="626"/>
        <source>Verifying flash...</source>
        <translation>Flash wird überprüft...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="694"/>
        <source>Verifying flash... %1%</source>
        <translation>Flash wird überprüft... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="704"/>
        <source>Finishing the upgrade...</source>
        <translation>Upgrade wird abgeschlossen...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="882"/>
        <source>Reading the firmware file...</source>
        <translation>Firmware-Datei wird gelesen...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="897"/>
        <source>Device already in IAP mode...</source>
        <translation>Gerät ist bereits im IAP-Modus...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="899"/>
        <source>Entering IAP mode...</source>
        <translation>IAP-Modus wird betreten...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="906"/>
        <source>Looking for the HID device...</source>
        <translation>HID-Gerät wird gesucht...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="978"/>
        <source>Upgrade complete</source>
        <translation>Upgrade abgeschlossen</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::Settings</name>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="210"/>
        <location filename="../pv/dialogs/settings.cpp" line="289"/>
        <source>General</source>
        <translation>Allgemein</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="219"/>
        <source>Views</source>
        <translation>Ansichten</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="229"/>
        <location filename="../pv/dialogs/settings.cpp" line="496"/>
        <source>Decoders</source>
        <translation>Protokolldekoder</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="239"/>
        <source>About</source>
        <translation>Programmdetails</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="248"/>
        <source>Logging</source>
        <translation>Programminterne Meldungen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="317"/>
        <source>User interface language</source>
        <translation>Sprache der Benutzeroberfläche</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="321"/>
        <source>Follow system</source>
        <translation>Systemeinstellung folgen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="322"/>
        <source>Light</source>
        <translation>Hell</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="323"/>
        <source>Dark</source>
        <translation>Dunkel</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="329"/>
        <source>User interface theme</source>
        <translation>Design der Benutzeroberfläche</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="367"/>
        <source>Save session &amp;setup along with .sr file</source>
        <translation>Analyse&amp;sitzungs-Konfiguration zusammen mit .sr-Dateien speichern</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="386"/>
        <source>Trace View</source>
        <translation>Signalansicht</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="394"/>
        <location filename="../pv/mainwindow.cpp" line="1986"/>
        <source>Use colored trace &amp;background</source>
        <translation>Verwende &amp;farbigen Kanalhintergrund</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="398"/>
        <source>Constantly perform &amp;zoom-to-fit during acquisition</source>
        <translation>Ständig den &amp;Zoom anpassen, während Daten aufgezeichnet werden</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="402"/>
        <source>Perform a zoom-to-&amp;fit when acquisition stops</source>
        <translation>Den Zoom &amp;anpassen, wenn die Datenerfassung stoppt</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="410"/>
        <location filename="../pv/mainwindow.cpp" line="1996"/>
        <source>Always keep &amp;newest samples at the right edge during capture</source>
        <translation>Die neuesten Datenpunkte während der Aufzeichnung immer am rechten &amp;Rand anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="418"/>
        <location filename="../pv/mainwindow.cpp" line="2006"/>
        <source>Show data &amp;sampling points</source>
        <translation>Daten&amp;punkte visuell hervorheben</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="429"/>
        <source>Color to fill high areas of logic signals with</source>
        <translation>Farbe für hervorgehobene High-Pegel</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="433"/>
        <location filename="../pv/mainwindow.cpp" line="2016"/>
        <source>Show analog minor grid in addition to div grid</source>
        <translation>Vertikale Unterteilungen nochmals unterteilen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="362"/>
        <source>User interface style</source>
        <translation>Stil der Benutzeroberfläche</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="437"/>
        <source>Highlight mouse cursor using a vertical marker line</source>
        <translation>Position des Mauscursors durch vertikalen Balken hervorheben</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="441"/>
        <location filename="../pv/dialogs/settings.cpp" line="467"/>
        <location filename="../pv/dialogs/settings.cpp" line="476"/>
        <source> pixels</source>
        <translation> Pixel</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="446"/>
        <source>Maximum distance from edges before markers snap to them</source>
        <translation>Abstand zu Signalflanken, bevor Markierer einrasten</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="453"/>
        <source>Color to fill cursor area with</source>
        <translation>Farbe für die Auswahl-Markierung</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="456"/>
        <source>None</source>
        <translation>Keine</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="457"/>
        <source>Background</source>
        <translation>Hintergrundfarbe</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="458"/>
        <source>Dots</source>
        <translation>Farbige Abtastpunkte</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="463"/>
        <source>Conversion threshold display mode (analog traces only)</source>
        <translation>Darstellung von Konvertierungsschwellen (nur für analoge Kanäle)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="472"/>
        <source>Default analog trace div height</source>
        <translation>Standardgröße von analogen Kanälen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="481"/>
        <source>Default logic trace height</source>
        <translation>Standardgröße von Logikkanälen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="504"/>
        <source>Allow configuration of &amp;initial signal state</source>
        <translation>&amp;Initialzustände von Signalen konfigurierbar machen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="508"/>
        <source>Always show all &amp;rows, even if no annotation is visible</source>
        <translation>Immer alle &amp;Kategorien (Zeilen) anzeigen, auch wenn hierfür keine dekodierten Werte vorliegen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="516"/>
        <source>Annotation export format</source>
        <translation>Format für zu exportierende Dekodierwerte</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="517"/>
        <source>%s = sample range; %d: decoder name; %r: row name; %c: class name</source>
        <translation>%s = Start-/Endsample; %d: Dekodername; %r: Kategorie (Name der Zeile); %c: Unterkategorie</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="520"/>
        <source>%1: longest annotation text; %a: all annotation texts; %q: use quotation marks</source>
        <translation>%1: Längste Beschreibung des dekodierten Wertes; %a: Alle Beschreibungen des dekodierten Wertes; %q: Benutze Anführungszeichen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="569"/>
        <source>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;(Note: Set environment variable SIGROKDECODE_DIR to add a custom directory)&lt;/td&gt;&lt;/tr&gt;</source>
        <translation>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;(Hinweis: Umgebungsvariable SIGROKDECODE_DIR setzen, um ein eigenes Verzeichnis hinzuzufügen)&lt;/td&gt;&lt;/tr&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="406"/>
        <source>Show time zero at the &amp;trigger</source>
        <translation>Zeitnullpunkt am &amp;Trigger anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="414"/>
        <source>Allow &amp;vertical dragging in the view area</source>
        <translation>&amp;vertikales Ziehen im Ansichtsbereich erlauben</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="422"/>
        <source>Fill &amp;high areas of logic signals</source>
        <translation>&amp;hohe Bereiche von Logiksignalen füllen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="540"/>
        <source>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</source>
        <translation>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="541"/>
        <source>GNU GPL, version 3 or later</source>
        <translation>GNU GPL, Version 3 oder neuer</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="552"/>
        <source>Versions, libraries and features:</source>
        <translation>Versionen, Bibliotheken und Features:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="559"/>
        <source>Firmware search paths:</source>
        <translation>Suchpfade für Firmware:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="566"/>
        <source>Protocol decoder search paths:</source>
        <translation>Suchpfade für Protokolldekoder:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="574"/>
        <source>Supported hardware drivers:</source>
        <translation>Unterstützte Hardwaretreiber:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="581"/>
        <source>Supported input formats:</source>
        <translation>Unterstützte Importformate:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="588"/>
        <source>Supported output formats:</source>
        <translation>Unterstützte Exportformate:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="596"/>
        <source>Supported protocol decoders:</source>
        <translation>Unterstützte Protokolldekoder:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="604"/>
        <source>Available Translations:</source>
        <translation>Verfügbare Übersetzungen:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="652"/>
        <source>Log level:</source>
        <translation>Log-Level:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="657"/>
        <source> lines</source>
        <translation> Zeilen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="666"/>
        <source>Length of background buffer:</source>
        <translation>Länge des Logpuffers:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="672"/>
        <source>&amp;Save to File</source>
        <translation>&amp;Speichern</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="679"/>
        <source>&amp;Pop out</source>
        <translation>&amp;Abdocken</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="977"/>
        <source>Save Log</source>
        <translation>Log speichern</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="977"/>
        <source>Log Files (*.txt *.log);;All Files (*)</source>
        <translation>Logdateien (*.txt *.log);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="989"/>
        <source>Success</source>
        <translation>Erfolg</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="989"/>
        <source>Log saved to %1.</source>
        <translation>Log als %1 gespeichert.</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="999"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="999"/>
        <source>File %1 could not be written to.</source>
        <translation>Konnte Datei %1 nicht speichern.</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="1013"/>
        <source>%1 Log</source>
        <translation>%1-Protokoll</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::StoreProgress</name>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Saving...</source>
        <translation>Speichere...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="89"/>
        <source>Failed to save session.</source>
        <translation>Beim Speichern trat ein Fehler auf.</translation>
    </message>
</context>
<context>
    <name>pv::popups::Channels</name>
    <message>
        <location filename="../pv/popups/channels.cpp" line="219"/>
        <location filename="../pv/popups/channels.cpp" line="309"/>
        <location filename="../pv/popups/channels.cpp" line="1270"/>
        <source>Model:</source>
        <translation>Modell:</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="223"/>
        <location filename="../pv/popups/channels.cpp" line="315"/>
        <location filename="../pv/popups/channels.cpp" line="1271"/>
        <source>SN:</source>
        <translation>SN:</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="225"/>
        <location filename="../pv/popups/channels.cpp" line="1259"/>
        <source>Options</source>
        <translation>Optionen</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="334"/>
        <location filename="../pv/popups/channels.cpp" line="533"/>
        <location filename="../pv/popups/channels.cpp" line="1275"/>
        <source>With a repeating signal the trigger point floats: this is a limit rather than a fixed place.</source>
        <translation>Bei einem periodischen Signal wandert der Trigger-Punkt: dies ist eine Grenze und kein fester Ort.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="339"/>
        <location filename="../pv/popups/channels.cpp" line="536"/>
        <location filename="../pv/popups/channels.cpp" line="1273"/>
        <source>Pre-trigger</source>
        <translation>Pre-trigger</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="396"/>
        <location filename="../pv/popups/channels.cpp" line="1344"/>
        <source>Use 2 channels (Max 40MHz)</source>
        <translation>2 Kanäle verwenden (max. 40 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="397"/>
        <location filename="../pv/popups/channels.cpp" line="1345"/>
        <source>Use 4 channels (Max 40MHz)</source>
        <translation>4 Kanäle verwenden (max. 40 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="398"/>
        <location filename="../pv/popups/channels.cpp" line="1346"/>
        <source>Use 8 channels (Max 40MHz)</source>
        <translation>8 Kanäle verwenden (max. 40 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="399"/>
        <location filename="../pv/popups/channels.cpp" line="1347"/>
        <source>Use 16 channels (Max 20MHz)</source>
        <translation>16 Kanäle verwenden (max. 20 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="402"/>
        <location filename="../pv/popups/channels.cpp" line="1349"/>
        <source>Use 2 channels (Max 200MHz)</source>
        <translation>2 Kanäle verwenden (max. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="403"/>
        <location filename="../pv/popups/channels.cpp" line="1350"/>
        <source>Use 4 channels (Max 200MHz)</source>
        <translation>4 Kanäle verwenden (max. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="404"/>
        <location filename="../pv/popups/channels.cpp" line="1351"/>
        <source>Use 8 channels (Max 200MHz)</source>
        <translation>8 Kanäle verwenden (max. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="405"/>
        <location filename="../pv/popups/channels.cpp" line="1352"/>
        <source>Use 16 channels (Max 200MHz)</source>
        <translation>16 Kanäle verwenden (max. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="409"/>
        <location filename="../pv/popups/channels.cpp" line="1355"/>
        <source>Use 2 channels (Max 1GHz)</source>
        <translation>2 Kanäle verwenden (max. 1 GHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="410"/>
        <location filename="../pv/popups/channels.cpp" line="1356"/>
        <source>Use 4 channels (Max 500MHz)</source>
        <translation>4 Kanäle verwenden (max. 500 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="411"/>
        <location filename="../pv/popups/channels.cpp" line="1357"/>
        <source>Use 8 channels (Max 250MHz)</source>
        <translation>8 Kanäle verwenden (max. 250 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="412"/>
        <location filename="../pv/popups/channels.cpp" line="1358"/>
        <source>Use 16 channels (Max 125MHz)</source>
        <translation>16 Kanäle verwenden (max. 125 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="415"/>
        <location filename="../pv/popups/channels.cpp" line="1361"/>
        <source>Use 2 channels (Max 500MHz)</source>
        <translation>2 Kanäle verwenden (max. 500 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="416"/>
        <location filename="../pv/popups/channels.cpp" line="1362"/>
        <source>Use 4 channels (Max 250MHz)</source>
        <translation>4 Kanäle verwenden (max. 250 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="417"/>
        <location filename="../pv/popups/channels.cpp" line="1363"/>
        <source>Use 8 channels (Max 125MHz)</source>
        <translation>8 Kanäle verwenden (max. 125 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="418"/>
        <location filename="../pv/popups/channels.cpp" line="1364"/>
        <source>Use 16 channels (Max 62.5MHz)</source>
        <translation>16 Kanäle verwenden (max. 62,5 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="454"/>
        <location filename="../pv/popups/channels.cpp" line="1278"/>
        <source>Trigger Threshold(0~7 Channel)</source>
        <translation>Trigger Threshold (Kanal 0~7)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="462"/>
        <location filename="../pv/popups/channels.cpp" line="1279"/>
        <source>Trigger Threshold(8~15 Channel)</source>
        <translation>Trigger Threshold (Kanal 8~15)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="469"/>
        <location filename="../pv/popups/channels.cpp" line="1284"/>
        <source>Logic Analyzer</source>
        <translation>Logic Analyzer</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="472"/>
        <location filename="../pv/popups/channels.cpp" line="1286"/>
        <source>ADC</source>
        <translation>ADC</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="480"/>
        <location filename="../pv/popups/channels.cpp" line="1272"/>
        <source>Device Mode</source>
        <translation>Gerätemodus</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="503"/>
        <source>Always match this to the level of the signal being measured, or the samples will be read at the wrong level.</source>
        <translation>Immer auf den Pegel des gemessenen Signals einstellen, sonst werden die Samples mit dem falschen Pegel gelesen.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="521"/>
        <location filename="../pv/popups/channels.cpp" line="1281"/>
        <source>Input Level</source>
        <translation>Eingangspegel</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="521"/>
        <location filename="../pv/popups/channels.cpp" line="1225"/>
        <location filename="../pv/popups/channels.cpp" line="1281"/>
        <location filename="../pv/popups/channels.cpp" line="1299"/>
        <source>Threshold Level</source>
        <translation>Schwellwertpegel</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1228"/>
        <location filename="../pv/popups/channels.cpp" line="1302"/>
        <source>Buffer Board</source>
        <translation>Buffer-Board</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="540"/>
        <location filename="../pv/popups/channels.cpp" line="1289"/>
        <source>ADC Channel 0</source>
        <translation>ADC-Kanal 0</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="541"/>
        <location filename="../pv/popups/channels.cpp" line="1290"/>
        <source>ADC Channel 1</source>
        <translation>ADC-Kanal 1</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="545"/>
        <location filename="../pv/popups/channels.cpp" line="1266"/>
        <source>ADC Channel</source>
        <translation>ADC-Kanal</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="551"/>
        <location filename="../pv/popups/channels.cpp" line="1294"/>
        <source>8-bit</source>
        <translation>8 Bit</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="552"/>
        <location filename="../pv/popups/channels.cpp" line="1295"/>
        <source>10-bit</source>
        <translation>10 Bit</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="556"/>
        <location filename="../pv/popups/channels.cpp" line="1268"/>
        <source>ADC Precision</source>
        <translation>ADC-Auflösung</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="623"/>
        <location filename="../pv/popups/channels.cpp" line="689"/>
        <location filename="../pv/popups/channels.cpp" line="1261"/>
        <location filename="../pv/popups/channels.cpp" line="1263"/>
        <source>Channels</source>
        <translation>Kanäle</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1141"/>
        <source>Information</source>
        <translation>Informationen</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1142"/>
        <source>The current mode does not support opening this channel.</source>
        <translation>Der aktuelle Modus unterstützt das Öffnen dieses Kanals nicht.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1143"/>
        <source>Confirm</source>
        <translation>Bestätigen</translation>
    </message>
</context>
<context>
    <name>pv::prop::Bool</name>
    <message>
        <location filename="../pv/prop/bool.cpp" line="51"/>
        <location filename="../pv/prop/bool.cpp" line="82"/>
        <source>Querying config key %1 resulted in %2</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Das Abfragen des Konfigurationsschlüssels %1 ergab %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Double</name>
    <message>
        <location filename="../pv/prop/double.cpp" line="65"/>
        <location filename="../pv/prop/double.cpp" line="96"/>
        <source>Querying config key %1 resulted in %2</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Das Abfragen des Konfigurationsschlüssels %1 ergab %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Enum</name>
    <message>
        <location filename="../pv/prop/enum.cpp" line="113"/>
        <location filename="../pv/prop/enum.cpp" line="176"/>
        <source>Querying config key %1 resulted in %2</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Das Abfragen des Konfigurationsschlüssels %1 ergab %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Int</name>
    <message>
        <location filename="../pv/prop/int.cpp" line="63"/>
        <location filename="../pv/prop/int.cpp" line="127"/>
        <source>Querying config key %1 resulted in %2</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Das Abfragen des Konfigurationsschlüssels %1 ergab %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::String</name>
    <message>
        <location filename="../pv/prop/string.cpp" line="59"/>
        <location filename="../pv/prop/string.cpp" line="84"/>
        <source>Querying config key %1 resulted in %2</source>
        <translatorcomment>Internal message</translatorcomment>
        <translation>Das Abfragen des Konfigurationsschlüssels %1 ergab %2</translation>
    </message>
</context>
<context>
    <name>pv::subwindows::decoder_selector::DecoderCollectionModel</name>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="42"/>
        <source>Decoder</source>
        <translation>Dekoder</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="43"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="44"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="51"/>
        <source>All Decoders</source>
        <translation>Alle Dekoder</translation>
    </message>
</context>
<context>
    <name>pv::subwindows::decoder_selector::SubWindow</name>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="54"/>
        <source>Select a decoder to see its description here.</source>
        <translation>Wähle einen Dekoder, um dessen Beschreibung hier lesen zu können.</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="352"/>
        <source>, %1</source>
        <translation>, %1</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="369"/>
        <source>&lt;p align=&apos;right&apos;&gt;Tags: %1&lt;/p&gt;</source>
        <translation>&lt;p align=&apos;right&apos;&gt;Stichworte: %1&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="431"/>
        <source>Protocol decoder &lt;b&gt;%1&lt;/b&gt; requires input type &lt;b&gt;%2&lt;/b&gt; which several decoders provide.&lt;br&gt;Choose which one to use:&lt;br&gt;</source>
        <translation>Protokolldekoder &lt;b&gt;%1&lt;/b&gt; benötigt Daten vom Typ &lt;b&gt;%2&lt;/b&gt;, die von verschiedenen Protokolldekodern bereitgestellt werden. &lt;br&gt;Wähle, welcher benutzt werden soll:&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="439"/>
        <source>Choose Decoder</source>
        <translation>Wähle Protokolldekoder</translation>
    </message>
</context>
<context>
    <name>pv::toolbars::MainBar</name>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2616"/>
        <source>Warning</source>
        <translation>Warnung</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="480"/>
        <source>&amp;Open...</source>
        <translation>&amp;Öffnen...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="481"/>
        <source>Restore Session Setu&amp;p...</source>
        <translation>&amp;Konfiguration der Analysesitzung laden...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="483"/>
        <source>Save &amp;As...</source>
        <translation>Speichern &amp;als...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="484"/>
        <source>Save Selected &amp;Range As...</source>
        <translation>Ausgewählten &amp;Bereich speichern als...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="485"/>
        <source>Save Session Setu&amp;p...</source>
        <translation>&amp;Konfiguration der Analysesitzung speichern...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="489"/>
        <source>&amp;Export</source>
        <translation>&amp;Exportieren</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="491"/>
        <source>&amp;Import</source>
        <translation>&amp;Importieren</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="486"/>
        <source>&amp;Connect to Device...</source>
        <translation>Mit Gerät &amp;verbinden...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="514"/>
        <source>Add protocol decoder</source>
        <translation>Protokolldekoder hinzufügen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="517"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="519"/>
        <source>Configure Channels</source>
        <translation>Kanäle konfigurieren</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="748"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1303"/>
        <source>Failed to get sample rate list:</source>
        <translation>Konnte Liste unterstützter Abtastraten nicht abfragen:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="479"/>
        <source>Decoding Result</source>
        <translation>Dekodierergebnis</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="511"/>
        <source>Open</source>
        <translation>Öffnen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="482"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="512"/>
        <source>Save</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="502"/>
        <source>Single</source>
        <translation>Einzel</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="503"/>
        <source>Repeat</source>
        <translation>Wiederholen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="505"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2575"/>
        <source>Version Info</source>
        <translation>Versionsinformationen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="506"/>
        <source>FW Update</source>
        <translation>FW aktualisieren</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="509"/>
        <source>Help</source>
        <translation>Hilfe</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2305"/>
        <source>Start</source>
        <translation>Starten</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="510"/>
        <source>Model</source>
        <translation>Modell</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="504"/>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="515"/>
        <source>Decoder</source>
        <translation>Dekoder</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="518"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="520"/>
        <source>Option</source>
        <translation>Option</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="851"/>
        <source>Failed to get sample rate value:</source>
        <translation>Abtastrate konnte nicht ermittelt werden:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="905"/>
        <source>Failed to get sample limit list:</source>
        <translation>Konnte Liste der maximal erlaubten Abtastraten nicht abfragen:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1106"/>
        <source>Failed to configure pre-trigger:</source>
        <translation>Pre-trigger konnte nicht konfiguriert werden:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1134"/>
        <source>Failed to configure pattern mode:</source>
        <translation>Pattern-Modus konnte nicht konfiguriert werden:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1155"/>
        <source>Failed to configure ADC channel:</source>
        <translation>ADC-Kanal konnte nicht konfiguriert werden:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1178"/>
        <source>Failed to configure ADC precision:</source>
        <translation>ADC-Auflösung konnte nicht konfiguriert werden:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1213"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1232"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1278"/>
        <source>Demo</source>
        <translation>Demo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1214"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1233"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1279"/>
        <source>Device Status: Demo</source>
        <translation>Gerätestatus: Demo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1220"/>
        <source>File</source>
        <translation>Datei</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1221"/>
        <source>Device Status: File</source>
        <translation>Gerätestatus: Datei</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1265"/>
        <source>USB2.0</source>
        <translation>USB2.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1266"/>
        <source>Device Status: USB 2.0</source>
        <translation>Gerätestatus: USB 2.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1269"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1274"/>
        <source>USB3.0</source>
        <translation>USB3.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1270"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1275"/>
        <source>Device Status: USB 3.0</source>
        <translation>Gerätestatus: USB 3.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1630"/>
        <source>Failed to configure samplerate:</source>
        <translation>Konnte Abtastrate nicht einstellen:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1776"/>
        <source>Failed to configure sample count:</source>
        <translation>Konnte Anzahl der Abtastpunkte nicht einstellen:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1789"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1813"/>
        <source>Missing Cursors</source>
        <translation>Fehlende Auswahl</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1813"/>
        <source>You need to set the cursors before you can save the data enclosed by them to a session file (e.g. using the Show Cursors button).</source>
        <translation>Du musst die Auswahl-Markierer setzen, bevor du die darin befindlichen Daten abspeichern kannst. Verwende hierzu bspw. den Knopf für die Auswahl-Markierer.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1832"/>
        <source>Invalid Range</source>
        <translation>Auswahl ungültig</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1832"/>
        <source>The cursors don&apos;t define a valid range of samples.</source>
        <translation>Die Auswahl-Markierer geben keinen gültigen Datenbereich an.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1846"/>
        <source>%1 files </source>
        <translation>%1-Dateien </translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1851"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1902"/>
        <source>All Files</source>
        <translation>Alle Dateien</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1855"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2372"/>
        <source>Save File</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1868"/>
        <source>Export %1</source>
        <translation>%1 exportieren</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1900"/>
        <source>%1 files</source>
        <translation>%1-Dateien</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1910"/>
        <source>Import File</source>
        <translation>Dateiimport</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1920"/>
        <source>Import %1</source>
        <translation>%1 importieren</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2310"/>
        <source>Stop</source>
        <translation>Stoppen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2330"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2388"/>
        <source>Open File</source>
        <translation>Öffnen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2330"/>
        <source>sigrok Sessions (*.sr);;All Files (*)</source>
        <translation>sigrok-Datenformat (*.sr);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2372"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2388"/>
        <source>PulseView Session Setups (*.pvs);;All Files (*)</source>
        <translation>Analysesitzungs-Konfigurationen (*.pvs);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2458"/>
        <source>Total Sampling Count: %1</source>
        <translation>Gesamtzahl der Samples: %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2485"/>
        <source>Device Options</source>
        <translation>Geräteoptionen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2539"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2558"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2570"/>
        <source>&lt;b&gt;Software Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Softwareversion:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2559"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2571"/>
        <source>&lt;b&gt;Firmware Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Firmwareversion:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2572"/>
        <source>&lt;b&gt;FPGA Version:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;FPGA-Version:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2617"/>
        <source>The current device does not support firmware upgrade.
Please connect the CH32H417 device and try again.</source>
        <translation>Der aktuelle Gerät unterstützt nicht die Firmware-Upgrade.
Bitte verbinden Sie sich mit dem CH32H417-Gerät und versuchen Sie es erneut.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2624"/>
        <source>Select firmware file</source>
        <translation>Firmware Datei auswählen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2626"/>
        <source>Firmware File (*.bin *.BIN *.hex *.HEX);;All Files (*.*)</source>
        <translation>Firmware Datei (*.bin *.BIN *.hex *.HEX);;Alle Dateien (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2641"/>
        <source>Firmware update in progress, do not disconnect the USB connection</source>
        <translation>Firmware aktualisierung in Fortgang, bitte nicht die USB-Verbindung abbrechen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2641"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2642"/>
        <source>Firmware Update</source>
        <translation>Firmware aktualisierung</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="507"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2743"/>
        <source>About</source>
        <translation>Über</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2712"/>
        <source>Information</source>
        <translation>Informationen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2713"/>
        <source>PDF file not found:
%1

Make sure the doc directory and the user manual are present.</source>
        <translation>PDF-Datei nicht gefunden:
%1

Stellen Sie sicher, dass das doc-Verzeichnis und das Benutzerhandbuch vorhanden sind.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2724"/>
        <source>The PDF file could not be opened. Make sure a PDF reader is installed.</source>
        <translation>Die PDF-Datei konnte nicht geöffnet werden. Stellen Sie sicher, dass ein PDF-Betrachter installiert ist.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2767"/>
        <source>Update Success</source>
        <translation>Update erfolgreich</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2768"/>
        <source>Firmware Update Success!
Device already restart.</source>
        <translation>Firmware aktualisierung erfolgreich
Device wurde neu gestartet</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2686"/>
        <source>Update Failed</source>
        <translation>Update fehlgeschlagen</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1968"/>
        <source>Failed to set CH32H417 mode on device switch:</source>
        <translation>CH32H417-Modus konnte beim Gerätewechsel nicht gesetzt werden:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2687"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>Firmware aktualisierung fehlgeschlagen
Bitte überprüfen Sie die Verbindung und die Firmware Datei.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2723"/>
        <source>Open Failed</source>
        <translation>Abriendo falló</translation>
    </message>
    <message>
        <location filename="../pv/util.cpp" line="474"/>
        <source>Confirm</source>
        <translation>Bestätigen</translation>
    </message>
</context>
<context>
    <name>pv::views</name>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="42"/>
        <source>Trace View</source>
        <translation>Signalansicht</translation>
    </message>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="44"/>
        <source>Binary Decoder Output View</source>
        <translation>Binäre Dekoderansicht</translation>
    </message>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="45"/>
        <source>Tabular Decoder Output View</source>
        <translation>Tabellarische Dekoderansicht</translation>
    </message>
</context>
<context>
    <name>pv::views::decoder_binary::View</name>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="56"/>
        <source>Binary</source>
        <translation>Binär</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="57"/>
        <source>Hex Dump, plain</source>
        <translation>Hex-Dump, ohne Offset</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="58"/>
        <source>Hex Dump, with offset</source>
        <translation>Hex-Dump, mit Offset</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="59"/>
        <source>Hex Dump, canonical</source>
        <translation>Hex-Dump, kanonisch</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="92"/>
        <source>Decoder:</source>
        <translation>Dekoder:</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="100"/>
        <source>Show data as</source>
        <translation>Zeige Daten als</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="108"/>
        <source>Hexdump</source>
        <translation>Hex-Dump</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="122"/>
        <source>&amp;Save...</source>
        <translation>&amp;Speichern...</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="346"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="386"/>
        <source>Save Binary Data</source>
        <translation>Binäre Daten speichern</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="346"/>
        <source>Binary Data Files (*.bin);;All Files (*)</source>
        <translation>Binärdateien (*.bin);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="365"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="417"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="365"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="417"/>
        <source>File %1 could not be written to.</source>
        <translation>Konnte Datei %1 nicht speichern.</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="386"/>
        <source>Hex Dumps (*.txt);;All Files (*)</source>
        <translation>Hex-Dumps (*.txt);;Alle Dateien (*)</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::AnnotationCollectionModel</name>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="58"/>
        <source>Sample</source>
        <translation>Sample</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="59"/>
        <source>Time</source>
        <translation>Zeit</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="60"/>
        <source>Decoder</source>
        <translation>Dekoder</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="61"/>
        <source>Ann Row</source>
        <translation>Kategorie</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="62"/>
        <source>Ann Class</source>
        <translation>Unterkategorie</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="63"/>
        <source>Value</source>
        <translation>Wert</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="66"/>
        <source>End Sample</source>
        <translation>End-Sample</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="67"/>
        <source>Duration</source>
        <translation>Dauer</translation>
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
        <translation>Nach dieser Spalte filtern</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::ColumnFilterPopup</name>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="651"/>
        <source>Clear filter</source>
        <translation>Filter zurücksetzen</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::View</name>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="73"/>
        <source>CSV, commas escaped</source>
        <translation>CSV, Kommas maskiert</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="74"/>
        <source>CSV, fields quoted</source>
        <translation>CSV, Felder in Anführungszeichen</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="78"/>
        <source>Show all</source>
        <translation>Alle anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="79"/>
        <source>Show all and focus on newest</source>
        <translation>Alle anzeigen und auf das Neueste springen</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="81"/>
        <source>Show visible in main view</source>
        <translation>Im Hauptfenster sichtbare anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="766"/>
        <source>Decoder:</source>
        <translation>Dekoder:</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="793"/>
        <source>Filter rows</source>
        <translation>Zeilen filtern</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="832"/>
        <source>Colors</source>
        <translation>Farben</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="833"/>
        <source>Colored annotation rows</source>
        <translation>Annotationszeilen farbig darstellen</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="839"/>
        <source>&amp;Save</source>
        <translation>&amp;Speichern</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1170"/>
        <source>%1 rows</source>
        <translation>%1 Zeilen</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1171"/>
        <source>%1 of %2 rows</source>
        <translation>%1 von %2 Zeilen</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1206"/>
        <source>Save Annotations as CSV</source>
        <translation>Dekodierte Werte als CSV speichern</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1206"/>
        <source>CSV Files (*.csv);;Text Files (*.txt);;All Files (*)</source>
        <translation>CSV-Dateien (*.csv);;Textdateien (*.txt);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1274"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1274"/>
        <source>File %1 could not be written to.</source>
        <translation>Konnte Datei %1 nicht speichern.</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::AnalogSignal</name>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="890"/>
        <source>Number of pos vertical divs</source>
        <translation>Anzahl Unterteilungen im Positiven</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="898"/>
        <source>Number of neg vertical divs</source>
        <translation>Anzahl Unterteilungen im Negativen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="903"/>
        <source> pixels</source>
        <translation> Pixel</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="907"/>
        <source>Div height</source>
        <translation>Höhe einer Unterteilung</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="926"/>
        <source>V/div</source>
        <translation>V/div</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="930"/>
        <source>Vertical resolution</source>
        <translation>Vertikale Auflösung</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="939"/>
        <source>Autoranging</source>
        <translation>Automatische Skalierung</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="944"/>
        <source>none</source>
        <translation>keine</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="946"/>
        <source>to logic via threshold</source>
        <translation>zu Logik mittels Schwellwert</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="948"/>
        <source>to logic via schmitt-trigger</source>
        <translation>zu Logik mittels Schmitt-Trigger</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="954"/>
        <source>Conversion</source>
        <translation>Konvertierung</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="963"/>
        <source>Conversion threshold(s)</source>
        <translation>Konvertierungs-Schwellwert(e)</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="973"/>
        <source>analog</source>
        <translation>nur analog</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="974"/>
        <source>converted</source>
        <translation>nur konvertiert</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="975"/>
        <source>analog+converted</source>
        <translation>analog+konvertiert</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="980"/>
        <source>Show traces for</source>
        <translation>Anzuzeigende Signale</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Cursor</name>
    <message>
        <location filename="../pv/views/trace/cursor.cpp" line="105"/>
        <source>Disable snapping</source>
        <translation>Einrasten deaktivieren</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::CursorPair</name>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="131"/>
        <source>Display interval</source>
        <translation>Intervall anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="143"/>
        <source>Display frequency</source>
        <translation>Frequenz anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="155"/>
        <source>Display samples</source>
        <translation>Samples anzeigen</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::DecodeTrace</name>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="480"/>
        <source>&lt;p&gt;&lt;i&gt;No decoders in the stack&lt;/i&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;i&gt;Keine Protokolldekoder vorhanden&lt;/i&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="491"/>
        <source>&lt;i&gt;* Required channels&lt;/i&gt;</source>
        <translation>&lt;i&gt;* Notwendige Kanäle&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="495"/>
        <source>Stack Decoder</source>
        <translation>Protokolldekoder stapeln</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="496"/>
        <source>Stack a higher-level decoder on top of this one</source>
        <translation>Weiteren Protokolldekoder auf diesen stapeln</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="598"/>
        <source>Delete</source>
        <translation>Löschen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="640"/>
        <source>Resume decoding</source>
        <translation>Dekodierung fortsetzen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="646"/>
        <source>Pause decoding</source>
        <translation>Dekodierung anhalten</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="653"/>
        <source>Copy annotation text to clipboard</source>
        <translation>Dekodierten Wert in die Zwischenablage kopieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="661"/>
        <source>Export all annotations</source>
        <translation>Alle dekodierten Werte exportieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="667"/>
        <source>Export all annotations for this row</source>
        <translation>Alle dekodierten Werte dieser Kategorie (Zeile) exportieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="675"/>
        <source>Export all annotations, starting here</source>
        <translation>Alle dekodierten Werte ab hier exportieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="681"/>
        <source>Export annotations for this row, starting here</source>
        <translation>Alle dekodierten Werte dieser Kategorie (Zeile) ab hier exportieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="689"/>
        <source>Export all annotations within cursor range</source>
        <translation>Alle dekodierten Werte innerhalb des gewählten Bereiches exportieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="695"/>
        <source>Export annotations for this row within cursor range</source>
        <translation>Alle dekodierten Werte dieser Kategorie (Zeile) innerhalb des gewählten Bereiches exportieren</translation>
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
        <translation>Dekodierte Werte exportieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1300"/>
        <source>Text Files (*.txt);;All Files (*)</source>
        <translation>Textdateien (*.txt);;Alle Dateien (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1365"/>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1374"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1365"/>
        <source>File %1 could not be written to.</source>
        <translation>Konnte Datei %1 nicht speichern.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1434"/>
        <source>Show this row</source>
        <translation>Diese Kategorie anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1445"/>
        <source>Show All</source>
        <translation>Alle anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1453"/>
        <source>Hide All</source>
        <translation>Alle verstecken</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Flag</name>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="146"/>
        <source>Text</source>
        <translation>Text</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="155"/>
        <source>Delete</source>
        <translation>Löschen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="160"/>
        <source>Disable snapping</source>
        <translation>Einrasten deaktivieren</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Header</name>
    <message>
        <location filename="../pv/views/trace/header.cpp" line="138"/>
        <source>Group</source>
        <translation>Gruppieren</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::LogicSignal</name>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="553"/>
        <source>No trigger</source>
        <translation>Kein Trigger</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="566"/>
        <source>Trigger on rising edge</source>
        <translation>Trigger auf steigende Flanke</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="579"/>
        <source>Trigger on high level</source>
        <translation>Trigger auf High-Pegel</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="592"/>
        <source>Trigger on falling edge</source>
        <translation>Trigger auf fallende Flanke</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="605"/>
        <source>Trigger on low level</source>
        <translation>Trigger auf Low-Pegel</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="618"/>
        <source>Trigger on rising or falling edge</source>
        <translation>Trigger auf steigende oder fallende Flanke</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="706"/>
        <source> pixels</source>
        <translation> Pixel</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="710"/>
        <source>Trace height</source>
        <translation>Kanalgröße</translation>
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
        <translation>Editor für mathematische Ausdrücke</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="95"/>
        <source>Inputs:</source>
        <translation>Eingaben:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="101"/>
        <source>Variables:</source>
        <translation>Variablen:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="103"/>
        <source>Basic operators:</source>
        <translation>Grundoperatoren:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="110"/>
        <source>Assignments:</source>
        <translation>Zuweisungen:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="120"/>
        <source>General purpose functions:</source>
        <translation>Allgemeine Funktionen:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="121"/>
        <source>abs(x)		Absolute value of x</source>
        <translation>abs(x)		Absolutwert von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="122"/>
        <source>avg(x, y, ...)	Average of all input values</source>
        <translation>avg(x, y, ...)	Mittelwert aller Eingabewerte</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="123"/>
        <source>ceil(x)		Smallest integer that is greater than or equal to x</source>
        <translation>ceil(x)		Kleinste ganze Zahl, die größer oder gleich x ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="124"/>
        <source>clamp(lb, x, ub)	Clamp x in range between lb and ub, where lb &lt; ub</source>
        <translation>clamp(lb, x, ub)	Begrenzt x auf den Bereich zwischen lb und ub, wobei lb &lt; ub</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="125"/>
        <source>equal(x, y)	Equality test between x and y using normalised epsilon</source>
        <translation>equal(x, y)	Gleichheitstest zwischen x und y mit normalisiertem Epsilon</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="126"/>
        <source>erf(x)		Error function of x</source>
        <translation>erf(x)		Fehlerfunktion von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="127"/>
        <source>erfc(x)		Complimentary error function of x</source>
        <translation>erfc(x)		Komplementäre Fehlerfunktion von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="128"/>
        <source>exp(x)		e to the power of x</source>
        <translation>exp(x)		e hoch x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="129"/>
        <source>expm1(x)	e to the power of x minus 1, where x is very small.</source>
        <translation>expm1(x)	e hoch x minus 1, für sehr kleine x.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="130"/>
        <source>floor(x)		Largest integer that is less than or equal to x</source>
        <translation>floor(x)		Größte ganze Zahl, die kleiner oder gleich x ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="131"/>
        <source>frac(x)		Fractional portion of x</source>
        <translation>frac(x)		Nachkommaanteil von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="132"/>
        <source>hypot(x)		Hypotenuse of x and y (i.e. sqrt(x*x + y*y))</source>
        <translation>hypot(x)		Hypotenuse von x und y (d. h. sqrt(x*x + y*y))</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="133"/>
        <source>iclamp(lb, x, ub)	Inverse-clamp x outside of the range lb and ub, where lb &lt; ub.
		If x is within the range it will snap to the closest bound</source>
        <translation>iclamp(lb, x, ub)	Umgekehrtes Begrenzen: schiebt x aus dem Bereich lb bis ub heraus, wobei lb &lt; ub.
		Liegt x im Bereich, wird es auf die nächste Grenze gezogen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="134"/>
        <source>inrange(lb, x, ub)	In-range returns true when x is within the range lb and ub, where lb &lt; ub.</source>
        <translation>inrange(lb, x, ub)	In-range liefert wahr, wenn x im Bereich lb bis ub liegt, wobei lb &lt; ub.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="135"/>
        <source>log(x)		Natural logarithm of x</source>
        <translation>log(x)		Natürlicher Logarithmus von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="136"/>
        <source>log10(x)		Base 10 logarithm of x</source>
        <translation>log10(x)		Zehnerlogarithmus von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="140"/>
        <source>log1p(x)		Natural logarithm of 1 + x, where x is very small</source>
        <translation>log1p(x)		Natürlicher Logarithmus von 1 + x, für sehr kleine x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="141"/>
        <source>log2(x)		Base 2 logarithm of x</source>
        <translation>log2(x)		Zweierlogarithmus von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="142"/>
        <source>logn(x)		Base N logarithm of x, where n is a positive integer</source>
        <translation>logn(x)		Logarithmus von x zur Basis n, wobei n eine positive ganze Zahl ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="143"/>
        <source>max(x, y, ...)	Largest value of all the inputs</source>
        <translation>max(x, y, ...)	Größter Wert aller Eingaben</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="144"/>
        <source>min(x, y, ...)	Smallest value of all the inputs</source>
        <translation>min(x, y, ...)	Kleinster Wert aller Eingaben</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="145"/>
        <source>mul(x, y, ...)	Product of all the inputs</source>
        <translation>mul(x, y, ...)	Produkt aller Eingaben</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="146"/>
        <source>ncdf(x)		Normal cumulative distribution function</source>
        <translation>ncdf(x)		Kumulative Verteilungsfunktion der Normalverteilung</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="147"/>
        <source>nequal(x, y)	Not-equal test between x and y using normalised epsilon</source>
        <translation>nequal(x, y)	Ungleichheitstest zwischen x und y mit normalisiertem Epsilon</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="148"/>
        <source>pow(x, y)	x to the power of y</source>
        <translation>pow(x, y)	x hoch y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="149"/>
        <source>root(x, n)	Nth-Root of x, where n is a positive integer</source>
        <translation>root(x, n)	n-te Wurzel von x, wobei n eine positive ganze Zahl ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="150"/>
        <source>round(x)		Round x to the nearest integer</source>
        <translation>round(x)		Rundet x auf die nächste ganze Zahl</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="151"/>
        <source>roundn(x, n)	Round x to n decimal places, where n &gt; 0 and is an integer</source>
        <translation>roundn(x, n)	Rundet x auf n Nachkommastellen, wobei n &gt; 0 und ganzzahlig ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="152"/>
        <source>sgn(x)		Sign of x; -1 if x &lt; 0, +1 if x &gt; 0, else zero</source>
        <translation>sgn(x)		Vorzeichen von x; -1 wenn x &lt; 0, +1 wenn x &gt; 0, sonst null</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="153"/>
        <source>sqrt(x)		Square root of x, where x &gt;= 0</source>
        <translation>sqrt(x)		Quadratwurzel von x, wobei x &gt;= 0</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="154"/>
        <source>sum(x, y, ..,)	Sum of all the inputs</source>
        <translation>sum(x, y, ..,)	Summe aller Eingaben</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="155"/>
        <source>swap(x, y)	Swap the values of the variables x and y and return the current value of y</source>
        <translation>swap(x, y)	Vertauscht die Werte der Variablen x und y und liefert den aktuellen Wert von y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="156"/>
        <source>trunc(x)		Integer portion of x</source>
        <translation>trunc(x)		Ganzzahlanteil von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="160"/>
        <source>Trigonometry functions:</source>
        <translation>Trigonometrische Funktionen:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="161"/>
        <source>acos(x)		Arc cosine of x expressed in radians. Interval [-1,+1]</source>
        <translation>acos(x)		Arkuskosinus von x im Bogenmaß. Intervall [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="162"/>
        <source>acosh(x)		Inverse hyperbolic cosine of x expressed in radians</source>
        <translation>acosh(x)		Inverser hyperbolischer Kosinus von x im Bogenmaß</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="163"/>
        <source>asin(x)		Arc sine of x expressed in radians. Interval [-1,+1]</source>
        <translation>asin(x)		Arkussinus von x im Bogenmaß. Intervall [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="164"/>
        <source>asinh(x)		Inverse hyperbolic sine of x expressed in radians</source>
        <translation>asinh(x)		Inverser hyperbolischer Sinus von x im Bogenmaß</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="165"/>
        <source>atan(x)		Arc tangent of x expressed in radians. Interval [-1,+1]</source>
        <translation>atan(x)		Arkustangens von x im Bogenmaß. Intervall [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="166"/>
        <source>atan2(x, y)	Arc tangent of (x / y) expressed in radians. [-pi,+pi]  </source>
        <translation>atan2(x, y)	Arkustangens von (x / y) im Bogenmaß. [-pi,+pi]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="167"/>
        <source>atanh(x)		Inverse hyperbolic tangent of x expressed in radians</source>
        <translation>atanh(x)		Inverser hyperbolischer Tangens von x im Bogenmaß</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="168"/>
        <source>cos(x)		Cosine of x</source>
        <translation>cos(x)		Kosinus von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="169"/>
        <source>cosh(x)		Hyperbolic cosine of x</source>
        <translation>cosh(x)		Hyperbolischer Kosinus von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="170"/>
        <source>cot(x)		Cotangent of x</source>
        <translation>cot(x)		Kotangens von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="171"/>
        <source>csc(x)		Cosectant of x</source>
        <translation>csc(x)		Kosekans von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="172"/>
        <source>sec(x)		Secant of x</source>
        <translation>sec(x)		Sekans von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="173"/>
        <source>sin(x)		Sine of x</source>
        <translation>sin(x)		Sinus von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="174"/>
        <source>sinc(x)		Sine cardinal of x</source>
        <translation>sinc(x)		Sinc-Funktion von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="175"/>
        <source>sinh(x)		Hyperbolic sine of x</source>
        <translation>sinh(x)		Hyperbolischer Sinus von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="176"/>
        <source>tan(x)		Tangent of x</source>
        <translation>tan(x)		Tangens von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="177"/>
        <source>tanh(x)		Hyperbolic tangent of x</source>
        <translation>tanh(x)		Hyperbolischer Tangens von x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="178"/>
        <source>deg2rad(x)	Convert x from degrees to radians</source>
        <translation>deg2rad(x)	Rechnet x von Grad in Bogenmaß um</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="179"/>
        <source>deg2grad(x)	Convert x from degrees to gradians</source>
        <translation>deg2grad(x)	Rechnet x von Grad in Gon um</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="180"/>
        <source>rad2deg(x)	Convert x from radians to degrees</source>
        <translation>rad2deg(x)	Rechnet x von Bogenmaß in Grad um</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="181"/>
        <source>grad2deg(x)	Convert x from gradians to degrees</source>
        <translation>grad2deg(x)	Rechnet x von Gon in Grad um</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="185"/>
        <source>Logic operators:</source>
        <translation>Logikoperatoren:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="202"/>
        <source>Comparisons:</source>
        <translation>Vergleiche:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="203"/>
        <source>x = y or x == y	True only if x is strictly equal to y</source>
        <translation>x = y oder x == y	Wahr nur, wenn x genau gleich y ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="204"/>
        <source>x &lt;&gt; y or x != y	True only if x does not equal y</source>
        <translation>x &lt;&gt; y oder x != y	Wahr nur, wenn x ungleich y ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="205"/>
        <source>x &lt; y		True only if x is less than y</source>
        <translation>x &lt; y		Wahr nur, wenn x kleiner als y ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="206"/>
        <source>x &lt;= y		True only if x is less than or equal to y</source>
        <translation>x &lt;= y		Wahr nur, wenn x kleiner oder gleich y ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="207"/>
        <source>x &gt; y		True only if x is greater than y</source>
        <translation>x &gt; y		Wahr nur, wenn x größer als y ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="208"/>
        <source>x &gt;= y		True only if x is greater than or equal to y</source>
        <translation>x &gt;= y		Wahr nur, wenn x größer oder gleich y ist</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="209"/>
        <source>Flow control:</source>
        <translation>Ablaufsteuerung:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="210"/>
        <source>{ ... }		Beginning and end of instruction block</source>
        <translation>{ ... }		Anfang und Ende eines Anweisungsblocks</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="211"/>
        <source>if (x, y, z)	If x is true then return y else return z
if (x) y;			variant without implied else
if (x) { y };		variant with an instruction block
if (x) y; else z;		variant with explicit else
if (x) { y } else { z };	variant with instruction blocks</source>
        <translation>if (x, y, z)	Wenn x wahr ist, liefere y, sonst liefere z
if (x) y;			Variante ohne implizites else
if (x) { y };		Variante mit Anweisungsblock
if (x) y; else z;		Variante mit ausdrücklichem else
if (x) { y } else { z };	Variante mit Anweisungsblöcken</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="212"/>
        <source>x ? y : z	Ternary operator, equivalent to &apos;if (x, y, z)&apos;</source>
        <translation>x ? y : z	Ternärer Operator, entspricht &apos;if (x, y, z)&apos;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="213"/>
        <source>switch {		The first true case condition that is encountered will
 case x &gt; 1: a;	determine the result of the switch. If none of the case
 case x &lt; 1: b;	conditions hold true, the default action is used
 default:     c;	to determine the return value
}</source>
        <translation>switch {		Die erste zutreffende case-Bedingung, auf die die
 case x &gt; 1: a;	Ausführung trifft, bestimmt das Ergebnis des switch.
 case x &lt; 1: b;	Trifft keine der case-Bedingungen zu, wird die
 default:     c;	default-Anweisung als Rückgabewert verwendet
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="217"/>
        <source>while (conditon) {	Evaluates expression repeatedly as long as condition is true,
 expression;	returning the last value of expression
}</source>
        <translation>while (conditon) {	Wertet den Ausdruck wiederholt aus, solange die Bedingung wahr ist,
 expression;	und liefert den letzten Wert des Ausdrucks
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="218"/>
        <source>repeat		Evalues expression repeatedly as long as condition is false,
 expression;	returning the last value of expression
until (condition)
</source>
        <translation>repeat		Wertet den Ausdruck wiederholt aus, solange die Bedingung falsch ist,
 expression;	und liefert den letzten Wert des Ausdrucks
until (condition)
</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="219"/>
        <source>for (var x := 0; condition; x += 1) {	Repeatedly evaluates expression while the condition is true,
 expression			while evaluating the &apos;increment&apos; expression on each loop
}</source>
        <translation>for (var x := 0; condition; x += 1) {	Wertet den Ausdruck wiederholt aus, solange die Bedingung wahr ist,
 expression			und wertet dabei in jeder Runde den &apos;increment&apos;-Ausdruck aus
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="220"/>
        <source>break		Terminates the execution of the nearest enclosed loop, returning NaN</source>
        <translation>break		Beendet die Ausführung der nächsten umschließenden Schleife und liefert NaN</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="221"/>
        <source>break[x]		Terminates the execution of the nearest enclosed loop, returning x</source>
        <translation>break[x]		Beendet die Ausführung der nächsten umschließenden Schleife und liefert x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="222"/>
        <source>continue		Interrupts loop execution and resumes with the next loop iteration</source>
        <translation>continue		Unterbricht die Schleifenausführung und fährt mit der nächsten Runde fort</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="223"/>
        <source>return[x]		Returns immediately from within the current expression, returning x</source>
        <translation>return[x]		Kehrt sofort aus dem aktuellen Ausdruck zurück und liefert x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="224"/>
        <source>~(expr; expr; ...)	Evaluates each sub-expression and returns the value of the last one
~{expr; expr; ...}</source>
        <translation>~(expr; expr; ...)	Wertet jeden Teilausdruck aus und liefert den Wert des letzten
~{expr; expr; ...}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="236"/>
        <source>Copy to expression</source>
        <translation>In Ausdruck übernehmen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="249"/>
        <source>Basics</source>
        <translation>Grundlagen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="250"/>
        <source>Functions 1</source>
        <translation>Funktionen 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="251"/>
        <source>Functions 2</source>
        <translation>Funktionen 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="252"/>
        <source>Trigonometry</source>
        <translation>Trigonometrie</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="253"/>
        <source>Logic</source>
        <translation>Logik</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="254"/>
        <source>Flow Control 1</source>
        <translation>Ablaufsteuerung 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="255"/>
        <source>Flow Control 2</source>
        <translation>Ablaufsteuerung 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="256"/>
        <source>Examples</source>
        <translation>Beispiele</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::MathSignal</name>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="315"/>
        <source>Expression</source>
        <translation>Ausdruck</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="319"/>
        <location filename="../pv/views/trace/mathsignal.cpp" line="329"/>
        <source>same as session</source>
        <translation>wie in der Sitzung</translation>
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
        <translation>Anzahl der Samples</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="333"/>
        <source>Sample rate</source>
        <translation>Abtastrate</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Ruler</name>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="165"/>
        <source>Create marker here</source>
        <translation>Hier neue Markierung anlegen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="169"/>
        <source>Set as zero point</source>
        <translation>Als Nullpunkt setzen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="174"/>
        <source>Reset zero point</source>
        <translation>Nullpunkt zurücksetzen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="187"/>
        <source>Disable mouse hover marker</source>
        <translation>Mauszeigerbalken deaktivieren</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="187"/>
        <source>Enable mouse hover marker</source>
        <translation>Mauszeigerbalken aktivieren</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Signal</name>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="155"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="169"/>
        <source>Remove</source>
        <translation>Entfernen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="171"/>
        <source>Disable</source>
        <translation>Deaktivieren</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::StandardBar</name>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="174"/>
        <source>Zoom &amp;In</source>
        <translation>H&amp;ineinzoomen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="175"/>
        <source>Zoom &amp;Out</source>
        <translation>Hera&amp;uszoomen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="176"/>
        <location filename="../pv/views/trace/standardbar.cpp" line="177"/>
        <source>Zoom to &amp;Fit</source>
        <translation>&amp;Passend zoomen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="178"/>
        <source>Show &amp;Cursors</source>
        <translation>&amp;Auswahl-Markierer anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="179"/>
        <source>Show Cursors</source>
        <translation>Cursors anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="180"/>
        <source>Display last segment only</source>
        <translation>Nur letztes Segment anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="181"/>
        <source>Display last complete segment only</source>
        <translation>Nur letztes vollständiges Segment anzeigen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="182"/>
        <source>Display a single segment</source>
        <translation>Einzelnes Segment anzeigen</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TimeMarker</name>
    <message>
        <location filename="../pv/views/trace/timemarker.cpp" line="223"/>
        <source>Time</source>
        <translation>Zeit</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Trace</name>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="238"/>
        <source>Create marker here</source>
        <translation>Hier neue Markierung anlegen</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="390"/>
        <source>Color</source>
        <translation>Farbe</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="455"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TraceGroup</name>
    <message>
        <location filename="../pv/views/trace/tracegroup.cpp" line="144"/>
        <source>Ungroup</source>
        <translation>Trennen</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Viewport</name>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="303"/>
        <location filename="../pv/views/trace/viewport.cpp" line="349"/>
        <source>Width: </source>
        <translation>Breite: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="350"/>
        <source>Period: </source>
        <translation>Periode: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="351"/>
        <source>Frequency: </source>
        <translation>Frequenz: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="352"/>
        <source>Duty Cycle: </source>
        <translation>Tastverhältnis: </translation>
    </message>
</context>
<context>
    <name>pv::widgets::DecoderGroupBox</name>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="48"/>
        <source>Show/hide this decoder trace</source>
        <translation>Dekoder anzeigen/verbergen</translation>
    </message>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="58"/>
        <source>Delete this decoder trace</source>
        <translation>Protokolldekoder entfernen</translation>
    </message>
</context>
<context>
    <name>pv::widgets::DeviceToolButton</name>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="83"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="198"/>
        <source>Refresh Device List</source>
        <translation>Geräteliste aktualisieren</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="89"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="105"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="112"/>
        <source>&lt;No Device&gt;</source>
        <translation>&lt;Kein Gerät&gt;</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ExportMenu</name>
    <message>
        <location filename="../pv/widgets/exportmenu.cpp" line="73"/>
        <source>Export %1...</source>
        <translation>%1 exportieren...</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ImportMenu</name>
    <message>
        <location filename="../pv/widgets/importmenu.cpp" line="70"/>
        <source>Import %1...</source>
        <translation>%1 importieren...</translation>
    </message>
</context>
</TS>
