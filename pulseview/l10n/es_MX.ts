<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es_MX">
<context>
    <name>QApplication</name>
    <message>
        <location filename="../pv/devicemanager.cpp" line="349"/>
        <source>Error when scanning device driver &apos;%1&apos;: %2</source>
        <translation>Error al escanear el controlador del dispositivo &apos;%1&apos;: %2</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="72"/>
        <source>Querying config key %1 is not allowed</source>
        <translation>No se permite consultar la clave de configuración %1</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="81"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>La consulta de la clave de configuración %1 resultó en %2</translation>
    </message>
    <message>
        <location filename="../pv/devices/device.cpp" line="95"/>
        <source>Unknown type supplied when attempting to query %1</source>
        <translation>Tipo desconocido proporcionado al intentar consultar %1</translation>
    </message>
</context>
<context>
    <name>QHexView</name>
    <message>
        <location filename="../pv/views/decoder_binary/QHexView.cpp" line="347"/>
        <source>No data available</source>
        <translation>Datos no disponibles</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="129"/>
        <source>Stack trace of previous crash:</source>
        <translation>Stack trace de crash previo:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="143"/>
        <source>Don&apos;t show this message again</source>
        <translation>No mostrar este mensaje de nuevo</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="146"/>
        <source>When %1 last crashed, it created a stack trace.
A human-readable form has been saved to disk and was written to the log. You may access it from the settings dialog.</source>
        <translation>Cuando %1 se bloqueó por última vez, creó un stack trace.\nSe guardó un formulario legible en el disco y fue escrito en el log. Puedes acceder a el desde el dialogo de configuración.</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="81"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../pv/devicemanager.cpp" line="119"/>
        <source>Scanning for devices that driver %1 can access...</source>
        <translation>Buscando dispositivos a los que el controlador %1 puede acceder...</translation>
    </message>
</context>
<context>
    <name>pv::MainWindow</name>
    <message>
        <location filename="../pv/mainwindow.cpp" line="769"/>
        <location filename="../pv/mainwindow.cpp" line="2181"/>
        <source>Decoder Selector</source>
        <translation>Selección de decoder</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="836"/>
        <source>Session %1</source>
        <translation>Sesión %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="216"/>
        <location filename="../pv/mainwindow.cpp" line="1218"/>
        <location filename="../pv/mainwindow.cpp" line="2403"/>
        <location filename="../pv/mainwindow.cpp" line="3293"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="206"/>
        <source>Current available memory size is less than 4GB and will affect usage effect. 
Do you want to continue to use?</source>
        <translation>Obtenemos que la memoria disponible es menor a 4GB y afectará el rendimiento. 
¿Desea continuar de todas modos?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="215"/>
        <location filename="../pv/mainwindow.cpp" line="2802"/>
        <location filename="../pv/mainwindow.cpp" line="3434"/>
        <source>Confirm</source>
        <translation>Confirmar</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="185"/>
        <location filename="../pv/mainwindow.cpp" line="1157"/>
        <location filename="../pv/mainwindow.cpp" line="2383"/>
        <source>Warning</source>
        <translation>Advertencia</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="246"/>
        <source>No udev access rules were found for:
%1

Without them the device cannot be opened by an unprivileged user, so it will not be detected.

Enter your administrator password to install:
%2</source>
        <translation>No se encontraron reglas de acceso udev para:
%1

Sin ellas, un usuario sin privilegios no puede abrir el dispositivo, por lo que no se detectará.

Escriba la contraseña de administrador para instalarlas:
%2</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="257"/>
        <source>Administrator privileges required</source>
        <translation>Se requieren privilegios de administrador</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="266"/>
        <source>Access rules installed</source>
        <translation>Reglas de acceso instaladas</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="267"/>
        <source>The udev rules have been installed.

If the device is already plugged in, unplug and replug it before scanning again.</source>
        <translation>Se instalaron las reglas udev.

Si el dispositivo ya está conectado, desconéctelo y vuelva a conectarlo antes de buscar de nuevo.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="274"/>
        <source>Installation failed</source>
        <translation>Falló la instalación</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="275"/>
        <source>Could not install the udev rules:

%1

Please try again, or cancel to skip.</source>
        <translation>No se pudieron instalar las reglas udev:

%1

Inténtelo de nuevo, o cancele para omitir este paso.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="281"/>
        <source>Skipped installing access rules</source>
        <translation>Se omitió la instalación de las reglas de acceso</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="282"/>
        <source>The udev rules are not installed, so the device may not be detected.

You can run this application as root later, or copy the rule files from the repository to /etc/udev/rules.d/ and replug the device.</source>
        <translation>Las reglas udev no están instaladas, por lo que puede que el dispositivo no se detecte.

Puede ejecutar esta aplicación como root más tarde, o copiar los archivos de reglas del repositorio a /etc/udev/rules.d/ y volver a conectar el dispositivo.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1192"/>
        <source>device detached!
Do you want to save the session?</source>
        <translation>¡dispositivo desconectado!
¿Desea guardar la sesión?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1217"/>
        <source>Save</source>
        <translation>Guardar</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1264"/>
        <source>Save File</source>
        <translation>Guardar archivo</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1533"/>
        <source>Device scan timed out</source>
        <translation>Se agotó el tiempo de búsqueda de dispositivos</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1534"/>
        <source>Scanning these drivers took too long, so the software started without waiting for them:</source>
        <translation>La búsqueda de estos controladores tardó demasiado, así que se inició sin esperarlos:</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1536"/>
        <source>Their devices are not in the list. Unplug and replug the analyzer - or close whatever else is using it - and scan again.</source>
        <translation>Sus dispositivos no están en la lista. Desconecte y vuelva a conectar el analizador, o cierre el otro programa que lo esté usando, y busque de nuevo.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2131"/>
        <source>Sessions not restored</source>
        <translation>Sesiones no restauradas</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2132"/>
        <source>These boards are not connected, so the pages they held are not here:</source>
        <translation>Estas placas no están conectadas, así que las páginas que contenían no están aquí:</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2135"/>
        <source>Plug the board in, or close the program that is using it, then open the page again.</source>
        <translation>Conecte la placa, o cierre el programa que la esté usando, y abra la página de nuevo.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2164"/>
        <location filename="../pv/mainwindow.cpp" line="2235"/>
        <source>Start/Stop Acquisition</source>
        <translation>Iniciar/Detener Adquisición</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2165"/>
        <location filename="../pv/mainwindow.cpp" line="2166"/>
        <location filename="../pv/mainwindow.cpp" line="2238"/>
        <location filename="../pv/mainwindow.cpp" line="2239"/>
        <source>Repeat Acquisition</source>
        <translation>Repetir adquisición</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2167"/>
        <location filename="../pv/mainwindow.cpp" line="2246"/>
        <source>Settings</source>
        <translation>Configuraciones</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2402"/>
        <source>Discard &amp;&amp; Exit</source>
        <translation>Descartar &amp;&amp; Salir</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2473"/>
        <source>Capture Failed</source>
        <translation>Falló la captura</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2538"/>
        <source>Device stopped sending data</source>
        <translation>El dispositivo dejó de enviar datos</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2539"/>
        <source>The device stopped sending samples before the capture was due to end, usually because the USB link cannot sustain the configured %1.

The acquisition was stopped. The data captured before that may be incomplete or wrong. Try a lower sample rate, fewer channels, or another USB port.</source>
        <translation>El dispositivo dejó de enviar muestras antes de que la captura debiera terminar, normalmente porque el enlace USB no sostiene los %1 configurados.

La adquisición se detuvo. Los datos capturados antes de eso pueden estar incompletos o ser incorrectos. Pruebe una tasa de muestreo más baja, menos canales u otro puerto USB.</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2803"/>
        <location filename="../pv/mainwindow.cpp" line="3435"/>
        <source>This session contains unsaved data. Close anyway?</source>
        <translation>Esta sesión contiene datos sin guardar. ¿Cerrar de todos modos?</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2395"/>
        <source>&lt;h3&gt;Unsaved Capture Data&lt;/h3&gt;&lt;p&gt;Data will be permanently lost on exit.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Datos de captura no guardados&lt;/h3&gt;&lt;p&gt;Se perderán permanentemente al salir.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2317"/>
        <location filename="../pv/mainwindow.cpp" line="2523"/>
        <source>Reload</source>
        <translation>Recargar</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2317"/>
        <location filename="../pv/mainwindow.cpp" line="2325"/>
        <location filename="../pv/mainwindow.cpp" line="2523"/>
        <source>Run</source>
        <translation>Ejecutar</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="2331"/>
        <source>Stop</source>
        <translation>Detener</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3035"/>
        <source>Board: %1</source>
        <translation>Placa: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3131"/>
        <source>Close Session</source>
        <translation>Cerrar sesión</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3353"/>
        <location filename="../pv/mainwindow.cpp" line="3376"/>
        <source>Could not copy the capture: %1</source>
        <translation>No se pudo copiar la captura: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3408"/>
        <location filename="../pv/mainwindow.cpp" line="3420"/>
        <source>Could not save the session: %1</source>
        <translation>No se pudo guardar la sesión: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3050"/>
        <source>Decoders: %1</source>
        <translation>Decodificadores: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3295"/>
        <source>Export to a New Session</source>
        <translation>Exportar a una sesión nueva</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3030"/>
        <source>Port: %1</source>
        <translation>Puerto: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3040"/>
        <source>SN: %1</source>
        <translation>SN: %1</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1265"/>
        <source>Session files (*.sr);;All Files (*)</source>
        <translation>Archivos de sesión (*.sr);;Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3023"/>
        <source>Shows a session file</source>
        <translation>Muestra un archivo de sesión</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3128"/>
        <source>Export the capture to a new session</source>
        <translation>Exportar la captura a una sesión nueva</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="3127"/>
        <location filename="../pv/mainwindow.cpp" line="3293"/>
        <location filename="../pv/mainwindow.cpp" line="3323"/>
        <source>Exporting the capture... %1%</source>
        <translation>Exportando la captura… %1%</translation>
    </message>
    <message>
        <location filename="../pv/mainwindow.cpp" line="1215"/>
        <source>Export Only the Capture to a New Session</source>
        <translation>Exportar solo la captura a una sesión nueva</translation>
    </message>
</context>
<context>
    <name>pv::Session</name>
    <message>
        <location filename="../pv/session.cpp" line="880"/>
        <source>Failed to open device</source>
        <translation>Error al abrir dispositivo</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1110"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="521"/>
        <source>Can&apos;t restore generated signal of unknown type %1 (%2)</source>
        <translation>No se puede restaurar la señal generada de tipo desconocido %1 (%2)</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="747"/>
        <source>this feature is currently not supported.</source>
        <translation>esta función no es compatible actualmente.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="874"/>
        <source>The device is already in use, or this user has no access to it. Close the program that holds it, then try again.</source>
        <translation>El dispositivo ya está en uso, o este usuario no tiene acceso a él. Cierre el programa que lo retiene e inténtelo de nuevo.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="877"/>
        <source>The device could not be opened.</source>
        <translation>No se pudo abrir el dispositivo.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1111"/>
        <source>Unexpected input format: %1</source>
        <translation>Formato de entrada inesperado: %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1147"/>
        <source>Failed to load %1</source>
        <translation>Error al cargar %1</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1189"/>
        <source>No active device set, can&apos;t start acquisition.</source>
        <translation>No hay un dispositivo activo configurado, no se puede iniciar la adquisición.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="1204"/>
        <source>No channels enabled.</source>
        <translation>No hay canales habilitados.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="742"/>
        <source>Out of memory, acquisition stopped.</source>
        <translation>Sin memoria, la adquisición se detuvo.</translation>
    </message>
    <message>
        <location filename="../pv/session.cpp" line="2275"/>
        <source>Can&apos;t handle more than 64 logic channels.</source>
        <translation>No puede manejar más de 64 canales lógicos.</translation>
    </message>
</context>
<context>
    <name>pv::StoreSession</name>
    <message>
        <location filename="../pv/storesession.cpp" line="116"/>
        <source>Can&apos;t save logic channel without data.</source>
        <translation>No se puede guardar el canal lógico sin datos.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="132"/>
        <source>Can&apos;t save analog channel without data.</source>
        <translation>No se puede guardar el canal analógico sin datos.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="144"/>
        <source>No channels enabled.</source>
        <translation>No hay canales habilitados.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="172"/>
        <source>Can&apos;t save range without sample data.</source>
        <translation>No se puede guardar el rango sin datos de muestra.</translation>
    </message>
    <message>
        <location filename="../pv/storesession.cpp" line="193"/>
        <location filename="../pv/storesession.cpp" line="300"/>
        <location filename="../pv/storesession.cpp" line="315"/>
        <source>Error while saving: </source>
        <translation>Error al guardar: </translation>
    </message>
</context>
<context>
    <name>pv::binding::Device</name>
    <message>
        <location filename="../pv/binding/device.cpp" line="84"/>
        <source>Note for device developers: Ignoring device configuration capability &apos;%1&apos; as it is missing GET and/or SET</source>
        <translation>Nota para desarrolladores de dispositivos: se ignora la capacidad de configuración &apos;%1&apos; porque le falta GET y/o SET</translation>
    </message>
    <message>
        <location filename="../pv/binding/device.cpp" line="109"/>
        <source>No Limit</source>
        <translation>Sin límite</translation>
    </message>
</context>
<context>
    <name>pv::data::DecodeSignal</name>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="257"/>
        <source>No decoders</source>
        <translation>Sin decodificadores</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="264"/>
        <source>There are no channels assigned to this decoder</source>
        <translation>No hay canales asignados a este decodificador</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="278"/>
        <source>One or more required channels have not been specified</source>
        <translation>No se han especificado uno o más canales requeridos</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="294"/>
        <source>No input data</source>
        <translation>Sin datos de entrada</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="1648"/>
        <location filename="../pv/data/decodesignal.cpp" line="1733"/>
        <source>Decoder reported an error</source>
        <translation>El decodificador reportó un error</translation>
    </message>
    <message>
        <location filename="../pv/data/decodesignal.cpp" line="1956"/>
        <source>Failed to create decoder instance</source>
        <translation>Error al crear la instancia del decodificador</translation>
    </message>
</context>
<context>
    <name>pv::data::MathSignal</name>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="107"/>
        <source>Math%1</source>
        <translation>Matemática%1</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="306"/>
        <source>No expression defined, nothing to do</source>
        <translation>No hay expresión definida, nada que hacer</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="345"/>
        <source>%1 at line %2, column %3: %4</source>
        <translation>%1 en la línea %2, columna %3: %4</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="364"/>
        <location filename="../pv/data/mathsignal.cpp" line="536"/>
        <source>&quot;%1&quot; isn&apos;t a valid analog signal</source>
        <translation>&quot;%1&quot; no es una señal analógica válida</translation>
    </message>
    <message>
        <location filename="../pv/data/mathsignal.cpp" line="374"/>
        <location filename="../pv/data/mathsignal.cpp" line="611"/>
        <source>No data will be generated as %1 must be enabled</source>
        <translation>No se generarán datos, ya que %1 debe estar habilitado</translation>
    </message>
</context>
<context>
    <name>pv::data::SignalBase</name>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="537"/>
        <source>Signal average</source>
        <translation>Promedio de la señal</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="538"/>
        <source>0.9V (for 1.8V CMOS)</source>
        <translation>0.9V (para 1.8V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="539"/>
        <source>1.8V (for 3.3V CMOS)</source>
        <translation>1.8V (para 3.3V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="540"/>
        <source>2.5V (for 5.0V CMOS)</source>
        <translation>2.5V (para 5.0V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="541"/>
        <source>1.5V (for TTL)</source>
        <translation>1.5V (para TTL)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="546"/>
        <source>Signal average +/- 15%</source>
        <translation>Promedio de la señal +/- 15%</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="547"/>
        <source>0.3V/1.2V (for 1.8V CMOS)</source>
        <translation>0.3V/1.2V (para 1.8V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="548"/>
        <source>0.7V/2.5V (for 3.3V CMOS)</source>
        <translation>0.7V/2.5V (para 3.3V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="549"/>
        <source>1.3V/3.7V (for 5.0V CMOS)</source>
        <translation>1.3V/3.7V (para 5.0V CMOS)</translation>
    </message>
    <message>
        <location filename="../pv/data/signalbase.cpp" line="550"/>
        <source>0.8V/2.0V (for TTL)</source>
        <translation>0.8V/2.0V (para TTL)</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::Connect</name>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="58"/>
        <source>&amp;Scan for devices using driver above</source>
        <translation>E&amp;Scanea por dispositivos utilizando el driver de arriba</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="63"/>
        <source>Connect to Device</source>
        <translation>Conectarse al dispositivo</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="75"/>
        <source>Step 1: Choose the driver</source>
        <translation>Paso 1: Elige el driver</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="79"/>
        <source>&amp;USB</source>
        <translation>&amp;USB</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="80"/>
        <source>Serial &amp;Port</source>
        <translation>&amp;Puerto Serial</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="81"/>
        <source>&amp;TCP/IP</source>
        <translation>&amp;TCP/IP</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="116"/>
        <source>Protocol:</source>
        <translation>Protocolo:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="134"/>
        <source>Step 2: Choose the interface</source>
        <translation>Paso 2: Elige la interfaz</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="140"/>
        <source>Step 3: Scan for devices</source>
        <translation>Paso 3: Escanear por dispositivos</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/connect.cpp" line="146"/>
        <source>Step 4: Select the device</source>
        <translation>Paso 4: Selecciona el dispositivo</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::DecoderChannelDialog</name>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="50"/>
        <source>Decoder Configuration</source>
        <translation>Configuración del decodificador</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="92"/>
        <source>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</source>
        <translation>&lt;b&gt;%1&lt;/b&gt;&lt;br/&gt;&lt;i&gt;%2&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="105"/>
        <source>Channel Mapping</source>
        <translation>Asignación de canales</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="110"/>
        <source>Decoder Options</source>
        <translation>Opciones del decodificador</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="121"/>
        <source>Apply &amp;&amp; Decode</source>
        <translation>Aplicar &amp;&amp; decodificar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="122"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/decoder_channel_dialog.cpp" line="173"/>
        <source>-- Not assigned --</source>
        <translation>-- Sin asignar --</translation>
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
        <translation>Actualización de FW</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="101"/>
        <source>FW File:</source>
        <translation>Archivo de firmware:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="103"/>
        <source>Select firmware file in BIN or HEX format</source>
        <translation>Seleccionar archivo de firmware en formato BIN o HEX</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="105"/>
        <source>Browse</source>
        <translation>Buscar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="114"/>
        <source>Ready</source>
        <translation>Listo</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="119"/>
        <source>Update Log:</source>
        <translation>Actualizar registro:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="128"/>
        <source>Start</source>
        <translation>Inicio</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="130"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="189"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="156"/>
        <source>Select firmware file</source>
        <translation>Seleccionar archivo de firmware</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="158"/>
        <source>Firmware File (*.bin *.hex);;All Files (*.*)</source>
        <translation>Archivo de firmware (*.bin *.hex);;Todos los archivos (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="172"/>
        <location filename="../pv/dialogs/iapdialog.cpp" line="177"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="172"/>
        <source>Please select a firmware file</source>
        <translation>Selecciona un archivo de firmware</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="177"/>
        <source>Firmware file does not exist</source>
        <translation>Archivo de firmware no existe</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="237"/>
        <source>Close</source>
        <translation>Cerrar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="243"/>
        <source>Update Success</source>
        <translation>Actualización exitosa</translation>
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
        <translation>Actualización exitosa de firmware
Dispositivo se reiniciará</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="247"/>
        <source>Update Failed</source>
        <translation>Actualización fallida</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="249"/>
        <source>Fail</source>
        <translation>Fallo</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="249"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>Actualización fallida de firmware
Por favor, compruebe la conexión del dispositivo y el archivo de firmware.</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::IAPWorker</name>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="508"/>
        <source>Erasing flash...</source>
        <translation>Borrando flash...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="548"/>
        <source>Programming flash...</source>
        <translation>Programando flash...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="608"/>
        <source>Programming flash... %1%</source>
        <translation>Programando flash... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="626"/>
        <source>Verifying flash...</source>
        <translation>Verificando flash...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="694"/>
        <source>Verifying flash... %1%</source>
        <translation>Verificando flash... %1%</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="704"/>
        <source>Finishing the upgrade...</source>
        <translation>Finalizando la actualización...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="882"/>
        <source>Reading the firmware file...</source>
        <translation>Leyendo el archivo de firmware...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="897"/>
        <source>Device already in IAP mode...</source>
        <translation>El dispositivo ya está en modo IAP...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="899"/>
        <source>Entering IAP mode...</source>
        <translation>Entrando en modo IAP...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="906"/>
        <source>Looking for the HID device...</source>
        <translation>Buscando el dispositivo HID...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/iapdialog.cpp" line="978"/>
        <source>Upgrade complete</source>
        <translation>Actualización completada</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::Settings</name>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="210"/>
        <location filename="../pv/dialogs/settings.cpp" line="289"/>
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="219"/>
        <source>Views</source>
        <translation>Vistas</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="229"/>
        <location filename="../pv/dialogs/settings.cpp" line="496"/>
        <source>Decoders</source>
        <translation>Decodificadores</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="239"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="248"/>
        <source>Logging</source>
        <translation>Registro</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="317"/>
        <source>User interface language</source>
        <translation>Lenguaje de la interfaz de usuario</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="321"/>
        <source>Follow system</source>
        <translation>Seguir el sistema</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="322"/>
        <source>Light</source>
        <translation>Claro</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="323"/>
        <source>Dark</source>
        <translation>Oscuro</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="329"/>
        <source>User interface theme</source>
        <translation>Tema de la interfaz de usuario</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="367"/>
        <source>Save session &amp;setup along with .sr file</source>
        <translation>Guardar &amp;configuración de sesión junto al archivo .sr</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="386"/>
        <source>Trace View</source>
        <translation>Vista de trazo</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="394"/>
        <location filename="../pv/mainwindow.cpp" line="3496"/>
        <source>Use colored trace &amp;background</source>
        <translation>Use &amp;fondo de trazas coloreado</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="398"/>
        <source>Constantly perform &amp;zoom-to-fit during acquisition</source>
        <translation>Realizar constantemente zoom para &amp;ajustar durante la adquisición</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="402"/>
        <source>Perform a zoom-to-&amp;fit when acquisition stops</source>
        <translation>Realice un zoom para ajustar cuando la adquisición se &amp;detenga</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="410"/>
        <location filename="../pv/mainwindow.cpp" line="3506"/>
        <source>Always keep &amp;newest samples at the right edge during capture</source>
        <translation>Mantenga siempre las muestras más &amp;recientes en el borde derecho durante la captura</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="418"/>
        <location filename="../pv/mainwindow.cpp" line="3516"/>
        <source>Show data &amp;sampling points</source>
        <translation>Mostrar puntos de datos &amp;sampleados</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="406"/>
        <source>Show time zero at the &amp;trigger</source>
        <translation>Mostrar el tiempo cero en el &amp;trigger</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="414"/>
        <source>Allow &amp;vertical dragging in the view area</source>
        <translation>Permitir arrastrar &amp;verticalmente en el área de vista</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="422"/>
        <source>Fill &amp;high areas of logic signals</source>
        <translation>Rellenar las áreas &amp;altas de las señales lógicas</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="429"/>
        <source>Color to fill high areas of logic signals with</source>
        <translation>Color para llenar áreas altas de señales lógicas</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="433"/>
        <location filename="../pv/mainwindow.cpp" line="3526"/>
        <source>Show analog minor grid in addition to div grid</source>
        <translation>Mostrar cuadrícula menor analogíca además de cuadrícula por div</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="362"/>
        <source>User interface style</source>
        <translation>Estilo de la interfaz</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="437"/>
        <source>Highlight mouse cursor using a vertical marker line</source>
        <translation>Resalte el cursor del mouse usando una línea de marcador vertical</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="441"/>
        <location filename="../pv/dialogs/settings.cpp" line="467"/>
        <location filename="../pv/dialogs/settings.cpp" line="476"/>
        <source> pixels</source>
        <translation> píxeles</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="446"/>
        <source>Maximum distance from edges before markers snap to them</source>
        <translation>Distancia máxima desde los bordes antes de que los marcadores se ajusten a ellos</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="453"/>
        <source>Color to fill cursor area with</source>
        <translation>Color para llenar el área del cursor</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="456"/>
        <source>None</source>
        <translation>Ninguna</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="457"/>
        <source>Background</source>
        <translation>Fondo</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="458"/>
        <source>Dots</source>
        <translation>Puntos</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="463"/>
        <source>Conversion threshold display mode (analog traces only)</source>
        <translation>Modo de visualización del umbral de conversión (solo trazas analógicas)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="472"/>
        <source>Default analog trace div height</source>
        <translation>Altura de div de trazo análogo por defecto</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="481"/>
        <source>Default logic trace height</source>
        <translation>Altura de trazo lógico por defecto</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="504"/>
        <source>Allow configuration of &amp;initial signal state</source>
        <translation>Permitir configuración de estado de señal &amp;inicial</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="508"/>
        <source>Always show all &amp;rows, even if no annotation is visible</source>
        <translation>Mostrar siempre todas las &amp;filas, incluso si no hay ninguna anotación visible</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="516"/>
        <source>Annotation export format</source>
        <translation>Formato de exportación de anotaciones</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="517"/>
        <source>%s = sample range; %d: decoder name; %r: row name; %c: class name</source>
        <translation>%s = rango de muestra; %d: nombre del decodificador; %r: nombre de fila; %c: nombre de clase</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="520"/>
        <source>%1: longest annotation text; %a: all annotation texts; %q: use quotation marks</source>
        <translation>%1: texto de anotación más largo; %a: todos los textos de anotación; %q: use comillas</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="540"/>
        <source>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</source>
        <translation>%1&lt;br /&gt;&lt;a href=&quot;http://%2&quot;&gt;%2&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="541"/>
        <source>GNU GPL, version 3 or later</source>
        <translation>GNU GPL, versión 3 o posterior</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="552"/>
        <source>Versions, libraries and features:</source>
        <translation>Versiones, librerías y características:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="559"/>
        <source>Firmware search paths:</source>
        <translation>Rutas de búsqueda de firmware:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="566"/>
        <source>Protocol decoder search paths:</source>
        <translation>Ruta de búsqueda del decodificador de protocolo:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="569"/>
        <source>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;(Note: Set environment variable SIGROKDECODE_DIR to add a custom directory)&lt;/td&gt;&lt;/tr&gt;</source>
        <translation>&lt;tr&gt;&lt;td colspan=&quot;2&quot;&gt;(Nota: defina la variable de entorno SIGROKDECODE_DIR para agregar un directorio propio)&lt;/td&gt;&lt;/tr&gt;</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="574"/>
        <source>Supported hardware drivers:</source>
        <translation>Drivers de hardware soportados:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="581"/>
        <source>Supported input formats:</source>
        <translation>Formatos de entrada soportados:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="588"/>
        <source>Supported output formats:</source>
        <translation>Formatos de salida soportados:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="596"/>
        <source>Supported protocol decoders:</source>
        <translation>Decodificadores de protocolo soportados:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="604"/>
        <source>Available Translations:</source>
        <translation>Traducciones disponibles:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="652"/>
        <source>Log level:</source>
        <translation>Nivel de log:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="657"/>
        <source> lines</source>
        <translation> líneas</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="666"/>
        <source>Length of background buffer:</source>
        <translation>Longitud del búfer de fondo:</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="672"/>
        <source>&amp;Save to File</source>
        <translation>&amp;Guardar en archivo</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="679"/>
        <source>&amp;Pop out</source>
        <translation>&amp;Desacoplar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="977"/>
        <source>Save Log</source>
        <translation>Guardar log</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="977"/>
        <source>Log Files (*.txt *.log);;All Files (*)</source>
        <translation>Archivos de log (*.txt *.log);;Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="989"/>
        <source>Success</source>
        <translation>Éxito</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="989"/>
        <source>Log saved to %1.</source>
        <translation>Log guardado en %1.</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="999"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="999"/>
        <source>File %1 could not be written to.</source>
        <translation>No se pudo escribir en el archivo%1.</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/settings.cpp" line="1013"/>
        <source>%1 Log</source>
        <translation>Registro de %1</translation>
    </message>
</context>
<context>
    <name>pv::dialogs::StoreProgress</name>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Saving...</source>
        <translation>Guardando...</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="44"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../pv/dialogs/storeprogress.cpp" line="89"/>
        <source>Failed to save session.</source>
        <translation>Error al guardar sesión.</translation>
    </message>
</context>
<context>
    <name>pv::popups::Channels</name>
    <message>
        <location filename="../pv/popups/channels.cpp" line="228"/>
        <location filename="../pv/popups/channels.cpp" line="326"/>
        <location filename="../pv/popups/channels.cpp" line="1331"/>
        <source>Model:</source>
        <translation>Modelo:</translation>
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
        <translation>Opciones</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="377"/>
        <location filename="../pv/popups/channels.cpp" line="587"/>
        <location filename="../pv/popups/channels.cpp" line="1337"/>
        <source>With a repeating signal the trigger point floats: this is a limit rather than a fixed place.</source>
        <translation>Con una señal repetitiva el punto de trigger flota: esto es un límite más que un lugar fijo.</translation>
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
        <translation>Usar 2 canales (máx. 40 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="451"/>
        <location filename="../pv/popups/channels.cpp" line="1407"/>
        <source>Use 4 channels (Max 40MHz)</source>
        <translation>Usar 4 canales (máx. 40 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="452"/>
        <location filename="../pv/popups/channels.cpp" line="1408"/>
        <source>Use 8 channels (Max 40MHz)</source>
        <translation>Usar 8 canales (máx. 40 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="453"/>
        <location filename="../pv/popups/channels.cpp" line="1409"/>
        <source>Use 16 channels (Max 20MHz)</source>
        <translation>Usar 16 canales (máx. 20 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="456"/>
        <location filename="../pv/popups/channels.cpp" line="1411"/>
        <source>Use 2 channels (Max 200MHz)</source>
        <translation>Usar 2 canales (máx. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="457"/>
        <location filename="../pv/popups/channels.cpp" line="1412"/>
        <source>Use 4 channels (Max 200MHz)</source>
        <translation>Usar 4 canales (máx. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="458"/>
        <location filename="../pv/popups/channels.cpp" line="1413"/>
        <source>Use 8 channels (Max 200MHz)</source>
        <translation>Usar 8 canales (máx. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="459"/>
        <location filename="../pv/popups/channels.cpp" line="1414"/>
        <source>Use 16 channels (Max 200MHz)</source>
        <translation>Usar 16 canales (máx. 200 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="463"/>
        <location filename="../pv/popups/channels.cpp" line="1417"/>
        <source>Use 2 channels (Max 1GHz)</source>
        <translation>Usar 2 canales (máx. 1 GHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="464"/>
        <location filename="../pv/popups/channels.cpp" line="1418"/>
        <source>Use 4 channels (Max 500MHz)</source>
        <translation>Usar 4 canales (máx. 500 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="465"/>
        <location filename="../pv/popups/channels.cpp" line="1419"/>
        <source>Use 8 channels (Max 250MHz)</source>
        <translation>Usar 8 canales (máx. 250 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="466"/>
        <location filename="../pv/popups/channels.cpp" line="1420"/>
        <source>Use 16 channels (Max 125MHz)</source>
        <translation>Usar 16 canales (máx. 125 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="469"/>
        <location filename="../pv/popups/channels.cpp" line="1423"/>
        <source>Use 2 channels (Max 500MHz)</source>
        <translation>Usar 2 canales (máx. 500 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="470"/>
        <location filename="../pv/popups/channels.cpp" line="1424"/>
        <source>Use 4 channels (Max 250MHz)</source>
        <translation>Usar 4 canales (máx. 250 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="471"/>
        <location filename="../pv/popups/channels.cpp" line="1425"/>
        <source>Use 8 channels (Max 125MHz)</source>
        <translation>Usar 8 canales (máx. 125 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="472"/>
        <location filename="../pv/popups/channels.cpp" line="1426"/>
        <source>Use 16 channels (Max 62.5MHz)</source>
        <translation>Usar 16 canales (máx. 62,5 MHz)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="508"/>
        <location filename="../pv/popups/channels.cpp" line="1340"/>
        <source>Trigger Threshold(0~7 Channel)</source>
        <translation>Trigger Threshold (canal 0~7)</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="516"/>
        <location filename="../pv/popups/channels.cpp" line="1341"/>
        <source>Trigger Threshold(8~15 Channel)</source>
        <translation>Trigger Threshold (canal 8~15)</translation>
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
        <translation>Modo del dispositivo</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="557"/>
        <source>Always match this to the level of the signal being measured, or the samples will be read at the wrong level.</source>
        <translation>Ajuste esto siempre al nivel de la señal que se mide, o las muestras se leerán con el nivel equivocado.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="575"/>
        <location filename="../pv/popups/channels.cpp" line="1343"/>
        <source>Input Level</source>
        <translation>Nivel de entrada</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="575"/>
        <location filename="../pv/popups/channels.cpp" line="1286"/>
        <location filename="../pv/popups/channels.cpp" line="1343"/>
        <location filename="../pv/popups/channels.cpp" line="1361"/>
        <source>Threshold Level</source>
        <translation>Nivel de umbral</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1289"/>
        <location filename="../pv/popups/channels.cpp" line="1364"/>
        <source>Buffer Board</source>
        <translation>Placa buffer</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="594"/>
        <location filename="../pv/popups/channels.cpp" line="1351"/>
        <source>ADC Channel 0</source>
        <translation>Canal ADC 0</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="595"/>
        <location filename="../pv/popups/channels.cpp" line="1352"/>
        <source>ADC Channel 1</source>
        <translation>Canal ADC 1</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="599"/>
        <location filename="../pv/popups/channels.cpp" line="1327"/>
        <source>ADC Channel</source>
        <translation>Canal ADC</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="605"/>
        <location filename="../pv/popups/channels.cpp" line="1356"/>
        <source>8-bit</source>
        <translation>8 bits</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="606"/>
        <location filename="../pv/popups/channels.cpp" line="1357"/>
        <source>10-bit</source>
        <translation>10 bits</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="610"/>
        <location filename="../pv/popups/channels.cpp" line="1329"/>
        <source>ADC Precision</source>
        <translation>Precisión ADC</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="677"/>
        <location filename="../pv/popups/channels.cpp" line="743"/>
        <location filename="../pv/popups/channels.cpp" line="1322"/>
        <location filename="../pv/popups/channels.cpp" line="1324"/>
        <source>Channels</source>
        <translation>Canales</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1202"/>
        <source>Information</source>
        <translation>Información</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1203"/>
        <source>The current mode does not support opening this channel.</source>
        <translation>El modo actual no permite abrir este canal.</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="1204"/>
        <source>Confirm</source>
        <translation>Confirmar</translation>
    </message>
    <message>
        <location filename="../pv/popups/channels.cpp" line="223"/>
        <location filename="../pv/popups/channels.cpp" line="320"/>
        <location filename="../pv/popups/channels.cpp" line="1333"/>
        <source>Port:</source>
        <translation>Puerto:</translation>
    </message>
</context>
<context>
    <name>pv::prop::Bool</name>
    <message>
        <location filename="../pv/prop/bool.cpp" line="51"/>
        <location filename="../pv/prop/bool.cpp" line="82"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>La consulta de la clave de configuración %1 resultó en %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Double</name>
    <message>
        <location filename="../pv/prop/double.cpp" line="65"/>
        <location filename="../pv/prop/double.cpp" line="96"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>La consulta de la clave de configuración %1 resultó en %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Enum</name>
    <message>
        <location filename="../pv/prop/enum.cpp" line="113"/>
        <location filename="../pv/prop/enum.cpp" line="176"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>La consulta de la clave de configuración %1 resultó en %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::Int</name>
    <message>
        <location filename="../pv/prop/int.cpp" line="63"/>
        <location filename="../pv/prop/int.cpp" line="127"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>La consulta de la clave de configuración %1 resultó en %2</translation>
    </message>
</context>
<context>
    <name>pv::prop::String</name>
    <message>
        <location filename="../pv/prop/string.cpp" line="59"/>
        <location filename="../pv/prop/string.cpp" line="84"/>
        <source>Querying config key %1 resulted in %2</source>
        <translation>La consulta de la clave de configuración %1 resultó en %2</translation>
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
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="44"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/model.cpp" line="51"/>
        <source>All Decoders</source>
        <translation>Todos los decoders</translation>
    </message>
</context>
<context>
    <name>pv::subwindows::decoder_selector::SubWindow</name>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="54"/>
        <source>Select a decoder to see its description here.</source>
        <translation>Seleccione un decoder para ver su descripción aquí.</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="352"/>
        <source>, %1</source>
        <translation>, %1</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="369"/>
        <source>&lt;p align=&apos;right&apos;&gt;Tags: %1&lt;/p&gt;</source>
        <translation>&lt;p align=&apos;right&apos;&gt;Tags: %1&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="431"/>
        <source>Protocol decoder &lt;b&gt;%1&lt;/b&gt; requires input type &lt;b&gt;%2&lt;/b&gt; which several decoders provide.&lt;br&gt;Choose which one to use:&lt;br&gt;</source>
        <translation>Decodificador de protocolo &lt;b&gt;%1&lt;/b&gt; requiere tipo de entrada &lt;b&gt;%2&lt;/b&gt; que proporcionan varios decodificadores.&lt;br&gt;Elige cúal usar:&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../pv/subwindows/decoder_selector/subwindow.cpp" line="439"/>
        <source>Choose Decoder</source>
        <translation>Elige Decoder</translation>
    </message>
</context>
<context>
    <name>pv::toolbars::MainBar</name>
    <message>
        <source>Device offline</source>
        <translation>Dispositivo desconectado</translation>
    </message>
    <message>
        <source>Waiting for the capture in the other session to end</source>
        <translation>Esperando a que termine la captura en la otra sesión</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2358"/>
        <source>Switch device</source>
        <translation>Cambiar de dispositivo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2359"/>
        <source>Switch device? The captured data and its decoders will be lost.</source>
        <translation>¿Cambiar de dispositivo? Se perderán los datos capturados y sus decodificadores.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3104"/>
        <source>Warning</source>
        <translation>Advertencia</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="508"/>
        <source>&amp;Open...</source>
        <translation>&amp;Abrir...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="509"/>
        <source>Restore Session Setu&amp;p...</source>
        <translation>Restaurar Configu&amp;ración de Sesión...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="511"/>
        <source>Save &amp;As...</source>
        <translation>Guardar &amp;como...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="512"/>
        <source>Save Selected &amp;Range As...</source>
        <translation>Guardar &amp;Rango Seleccionado Como...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="513"/>
        <source>Save Session Setu&amp;p...</source>
        <translation>Guardar Confi&amp;guración de Sesión...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="517"/>
        <source>&amp;Export</source>
        <translation>&amp;Exporta</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="519"/>
        <source>&amp;Import</source>
        <translation>&amp;Importa</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="514"/>
        <source>&amp;Connect to Device...</source>
        <translation>&amp;Conecta a Dispositivo...</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="542"/>
        <source>Add protocol decoder</source>
        <translation>Agregar decodificador de protocolo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="545"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="547"/>
        <source>Configure Channels</source>
        <translation>Configura Canales</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="875"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="1443"/>
        <source>Failed to get sample rate list:</source>
        <translation>Error al obtener la lista de frecuencia de muestreo:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="507"/>
        <source>Decoding Result</source>
        <translation>Resultado de decodificación</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="539"/>
        <source>Open</source>
        <translation>Abrir</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="510"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="540"/>
        <source>Save</source>
        <translation>Guardar</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="530"/>
        <source>Single</source>
        <translation>Unico</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="531"/>
        <source>Repeat</source>
        <translation>Repetir</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="533"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3063"/>
        <source>Version Info</source>
        <translation>Información de versión</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="534"/>
        <source>FW Update</source>
        <translation>Actualización de FW</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="537"/>
        <source>Help</source>
        <translation>Ayuda</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2765"/>
        <source>Start</source>
        <translation>Inicio</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="538"/>
        <source>Model</source>
        <translation>Modelo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="399"/>
        <source>Export to a New Session</source>
        <translation>Exportar a una sesión nueva</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="532"/>
        <source>Settings</source>
        <translation>Configuraciones</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="543"/>
        <source>Decoder</source>
        <translation>Decoder</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="546"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="548"/>
        <source>Option</source>
        <translation>Opción</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="987"/>
        <source>Failed to get sample rate value:</source>
        <translation>No se pudo obtener el valor de la tasa de muestreo:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1041"/>
        <source>Failed to get sample limit list:</source>
        <translation>Error al obtener la lista de límites de muestra:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1231"/>
        <source>Failed to configure pre-trigger:</source>
        <translation>No se pudo configurar el pre-trigger:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1260"/>
        <source>Failed to configure pattern mode:</source>
        <translation>No se pudo configurar el modo de patrón:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1281"/>
        <source>Failed to configure ADC channel:</source>
        <translation>No se pudo configurar el canal ADC:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1304"/>
        <source>Failed to configure ADC precision:</source>
        <translation>No se pudo configurar la precisión ADC:</translation>
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
        <translation>Estado del dispositivo: Demo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1360"/>
        <source>File</source>
        <translation>Archivo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1361"/>
        <source>Device Status: File</source>
        <translation>Estado del dispositivo: Archivo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1405"/>
        <source>USB2.0</source>
        <translation>USB2.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1406"/>
        <source>Device Status: USB 2.0</source>
        <translation>Estado del dispositivo: USB 2.0</translation>
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
        <translation>Estado del dispositivo: USB 3.0</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="1981"/>
        <source>Failed to configure samplerate:</source>
        <translation>Error al configurar frecuencia de muestreo:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2174"/>
        <source>Failed to configure sample count:</source>
        <translation>Error al configurar cuenta de muestras:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2189"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2213"/>
        <source>Missing Cursors</source>
        <translation>Cursores Faltantes</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2213"/>
        <source>You need to set the cursors before you can save the data enclosed by them to a session file (e.g. using the Show Cursors button).</source>
        <translation>Debe configurar los cursores antes de poder guardar los datos encerrados en un archivo de sesión (por ejemplo, usando el botón Mostrar Cursores).</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2232"/>
        <source>Invalid Range</source>
        <translation>Rango Inválido</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2232"/>
        <source>The cursors don&apos;t define a valid range of samples.</source>
        <translation>Los cursores no definen un rango válido de muestras.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2246"/>
        <source>%1 files </source>
        <translation>%1 archivos </translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2251"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2302"/>
        <source>All Files</source>
        <translation>Todos los archivos</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2255"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2838"/>
        <source>Save File</source>
        <translation>Guardar Archivo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2268"/>
        <source>Export %1</source>
        <translation>Exporta %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2300"/>
        <source>%1 files</source>
        <translation>%1 archivos</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2310"/>
        <source>Import File</source>
        <translation>Importa Archivo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2320"/>
        <source>Import %1</source>
        <translation>Importa %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2416"/>
        <source>Failed to set CH32H417 mode on device switch:</source>
        <translation>No se pudo establecer el modo CH32H417 al cambiar de dispositivo:</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2770"/>
        <source>Stop</source>
        <translation>Detener</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2790"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2854"/>
        <source>Open File</source>
        <translation>Abrir Archivo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2790"/>
        <source>sigrok Sessions (*.sr);;All Files (*)</source>
        <translation>Sesiones sigrok (*.sr);;Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2838"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="2854"/>
        <source>PulseView Session Setups (*.pvs);;All Files (*)</source>
        <translation>Configurciones de Sesión de PulseView (*.pvs);;Todos los Archivos (*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2924"/>
        <source>Total Sampling Count: %1</source>
        <translation>Total de muestras: %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="2958"/>
        <source>Device Options</source>
        <translation>Opciones del dispositivo</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3027"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3046"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3058"/>
        <source>&lt;b&gt;Software Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Versión de software:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3047"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3059"/>
        <source>&lt;b&gt;Firmware Version:&lt;/b&gt; %1&lt;br/&gt;</source>
        <translation>&lt;b&gt;Versión de firmware:&lt;/b&gt; %1&lt;br/&gt;</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3060"/>
        <source>&lt;b&gt;FPGA Version:&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;Versión de FPGA:&lt;/b&gt; %1</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3105"/>
        <source>The current device does not support firmware upgrade.
Please connect the CH32H417 device and try again.</source>
        <translation>El dispositivo actual no soporta la actualización de firmware.
Por favor, conecte el dispositivo CH32H417 y intente de nuevo.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3112"/>
        <source>Select firmware file</source>
        <translation>Seleccionar archivo de firmware</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3114"/>
        <source>Firmware File (*.bin *.BIN *.hex *.HEX);;All Files (*.*)</source>
        <translation>Archivo de firmware (*.bin *.BIN *.hex *.HEX);;Todos los archivos (*.*)</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3129"/>
        <source>Firmware update in progress, do not disconnect the USB connection</source>
        <translation>Actualización de firmware en progreso, no desconecte la conexión USB</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3129"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3130"/>
        <source>Firmware Update</source>
        <translation>Actualización de firmware</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="535"/>
        <location filename="../pv/toolbars/mainbar.cpp" line="3231"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3200"/>
        <source>Information</source>
        <translation>Información</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3201"/>
        <source>PDF file not found:
%1

Make sure the doc directory and the user manual are present.</source>
        <translation>No se encontró el archivo PDF:
%1

Asegúrese de que el directorio doc y el manual de usuario estén presentes.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3212"/>
        <source>The PDF file could not be opened. Make sure a PDF reader is installed.</source>
        <translation>No se pudo abrir el archivo PDF. Asegúrese de que haya un lector de PDF instalado.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3255"/>
        <source>Update Success</source>
        <translation>Actualización exitosa</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3256"/>
        <source>Firmware Update Success!
Device already restart.</source>
        <translation>Actualización exitosa de firmware
Dispositivo ya se reinició</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3174"/>
        <source>Update Failed</source>
        <translation>Actualización fallida</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3175"/>
        <source>Firmware Update Failed!
Please check the device connection and firmware file.</source>
        <translation>Actualización fallida de firmware
Por favor, compruebe la conexión del dispositivo y el archivo de firmware.</translation>
    </message>
    <message>
        <location filename="../pv/toolbars/mainbar.cpp" line="3211"/>
        <source>Open Failed</source>
        <translation>Abriendo falló</translation>
    </message>
    <message>
        <location filename="../pv/util.cpp" line="474"/>
        <source>Confirm</source>
        <translation>Confirmar</translation>
    </message>
</context>
<context>
    <name>pv::views</name>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="42"/>
        <source>Trace View</source>
        <translation>Vista de trazo</translation>
    </message>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="44"/>
        <source>Binary Decoder Output View</source>
        <translation>Vista de salida del decodificador binario</translation>
    </message>
    <message>
        <location filename="../pv/views/viewbase.cpp" line="45"/>
        <source>Tabular Decoder Output View</source>
        <translation>Vista de salida del decodificador tabular</translation>
    </message>
</context>
<context>
    <name>pv::views::decoder_binary::View</name>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="57"/>
        <source>Binary</source>
        <translation>Binario</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="58"/>
        <source>Hex Dump, plain</source>
        <translation>Volcado hexadecimal, sin desplazamiento</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="59"/>
        <source>Hex Dump, with offset</source>
        <translation>Volcado hexadecimal, con desplazamiento</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="60"/>
        <source>Hex Dump, canonical</source>
        <translation>Volcado hexadecimal, canónico</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="93"/>
        <source>Decoder:</source>
        <translation>Decodificador:</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="101"/>
        <source>Show data as</source>
        <translation>Muestra datos como</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="109"/>
        <source>Hexdump</source>
        <translation>Hexdump</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="123"/>
        <source>&amp;Save...</source>
        <translation>&amp;Guardar...</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="346"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="386"/>
        <source>Save Binary Data</source>
        <translation>Guardar Datos Binarios</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="346"/>
        <source>Binary Data Files (*.bin);;All Files (*)</source>
        <translation>Archivos de Datos Binarios (*.bin);;Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="365"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="417"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="365"/>
        <location filename="../pv/views/decoder_binary/view.cpp" line="417"/>
        <source>File %1 could not be written to.</source>
        <translation>No se pudo escribir en el archivo%1.</translation>
    </message>
    <message>
        <location filename="../pv/views/decoder_binary/view.cpp" line="386"/>
        <source>Hex Dumps (*.txt);;All Files (*)</source>
        <translation>Hex Dumps (*.txt);;Todos los archivos (*)</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::AnnotationCollectionModel</name>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="58"/>
        <source>Sample</source>
        <translation>Muestra</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="59"/>
        <source>Time</source>
        <translation>Tiempo</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="60"/>
        <source>Decoder</source>
        <translation>Decoder</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="61"/>
        <source>Ann Row</source>
        <translation>Categoría</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="62"/>
        <source>Ann Class</source>
        <translation>Subcategoría</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="63"/>
        <source>Value</source>
        <translation>Valor</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="66"/>
        <source>End Sample</source>
        <translation>Muestra final</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/model.cpp" line="67"/>
        <source>Duration</source>
        <translation>Duración</translation>
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
        <translation>Filtrar por esta columna</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::ColumnFilterPopup</name>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="593"/>
        <source>Clear filter</source>
        <translation>Borrar filtro</translation>
    </message>
</context>
<context>
    <name>pv::views::tabular_decoder::View</name>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="74"/>
        <source>CSV, commas escaped</source>
        <translation>CSV, comas escapadas</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="75"/>
        <source>CSV, fields quoted</source>
        <translation>CSV, campos entre comillas</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="79"/>
        <source>Show all</source>
        <translation>Mostrar todo</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="80"/>
        <source>Show all and focus on newest</source>
        <translation>Mostrar todo y enfocar lo más reciente</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="82"/>
        <source>Show visible in main view</source>
        <translation>Mostrar lo visible en la vista principal</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="708"/>
        <source>Decoder:</source>
        <translation>Decodificador:</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="735"/>
        <source>Filter rows</source>
        <translation>Filtrar filas</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="774"/>
        <source>Colors</source>
        <translation>Colores</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="775"/>
        <source>Colored annotation rows</source>
        <translation>Filas de anotación coloreadas</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="781"/>
        <source>&amp;Save</source>
        <translation>&amp;Guardar</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1110"/>
        <source>%1 rows</source>
        <translation>%1 filas</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1111"/>
        <source>%1 of %2 rows</source>
        <translation>%1 de %2 filas</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1146"/>
        <source>Save Annotations as CSV</source>
        <translation>Guardar anotaciones como CSV</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1146"/>
        <source>CSV Files (*.csv);;Text Files (*.txt);;All Files (*)</source>
        <translation>Archivos CSV (*.csv);;Archivos de texto (*.txt);;Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1214"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../pv/views/tabular_decoder/view.cpp" line="1214"/>
        <source>File %1 could not be written to.</source>
        <translation>No se pudo escribir en el archivo%1.</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::AnalogSignal</name>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="890"/>
        <source>Number of pos vertical divs</source>
        <translation>Número de divisiones verticales pos</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="898"/>
        <source>Number of neg vertical divs</source>
        <translation>Número de divisiones verticales neg</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="903"/>
        <source> pixels</source>
        <translation> píxeles</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="907"/>
        <source>Div height</source>
        <translation>Altura de div</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="926"/>
        <source>V/div</source>
        <translation>V/div</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="930"/>
        <source>Vertical resolution</source>
        <translation>Resolución vertical</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="939"/>
        <source>Autoranging</source>
        <translation>Autorango</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="944"/>
        <source>none</source>
        <translation>ninguna</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="946"/>
        <source>to logic via threshold</source>
        <translation>a nivel lógico a partir de umbral</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="948"/>
        <source>to logic via schmitt-trigger</source>
        <translation>a nivel lógico a partir de schmitt trigger</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="954"/>
        <source>Conversion</source>
        <translation>Conversión</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="963"/>
        <source>Conversion threshold(s)</source>
        <translation>Umbral(es) de conversión</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="973"/>
        <source>analog</source>
        <translation>análogo</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="974"/>
        <source>converted</source>
        <translation>convertida</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="975"/>
        <source>analog+converted</source>
        <translation>Analógico+convertido</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/analogsignal.cpp" line="980"/>
        <source>Show traces for</source>
        <translation>Mostrar trazos para</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Cursor</name>
    <message>
        <location filename="../pv/views/trace/cursor.cpp" line="105"/>
        <source>Disable snapping</source>
        <translation>Deshabilita snapping</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::CursorPair</name>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="131"/>
        <source>Display interval</source>
        <translation>Muestra intervalo</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="143"/>
        <source>Display frequency</source>
        <translation>Muestra frecuencia</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/cursorpair.cpp" line="155"/>
        <source>Display samples</source>
        <translation>Muestra samples</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::DecodeTrace</name>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="480"/>
        <source>&lt;p&gt;&lt;i&gt;No decoders in the stack&lt;/i&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;i&gt;No hay decodificadores en el stack.&lt;/i&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="491"/>
        <source>&lt;i&gt;* Required channels&lt;/i&gt;</source>
        <translation>&lt;i&gt;* Canales requeridos&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="495"/>
        <source>Stack Decoder</source>
        <translation>Decodificadores</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="496"/>
        <source>Stack a higher-level decoder on top of this one</source>
        <translation>Apilar un decodificador de nivel superior encima de este</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="598"/>
        <source>Delete</source>
        <translation>Eliminar</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="640"/>
        <source>Resume decoding</source>
        <translation>Reanudar decodificación</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="646"/>
        <source>Pause decoding</source>
        <translation>Pausar decodificación</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="653"/>
        <source>Copy annotation text to clipboard</source>
        <translation>Copiar texto de anotación al clipboard</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="661"/>
        <source>Export all annotations</source>
        <translation>Exportar todas las anotaciones</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="667"/>
        <source>Export all annotations for this row</source>
        <translation>Exportar todas las anotaciones para esta fila</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="675"/>
        <source>Export all annotations, starting here</source>
        <translation>Exportar todas las anotaciones, comenzando aquí</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="681"/>
        <source>Export annotations for this row, starting here</source>
        <translation>Exportar todas las anotaciones para esta fila, comenzando aquí</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="689"/>
        <source>Export all annotations within cursor range</source>
        <translation>Exportar todas las anotaciones dentro del rango del cursor</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="695"/>
        <source>Export annotations for this row within cursor range</source>
        <translation>Exportar todas las anotaciones para esta fila dentro del rango del cursor</translation>
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
        <translation>Exportar anotaciones</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1300"/>
        <source>Text Files (*.txt);;All Files (*)</source>
        <translation>Archivos de texto (*.txt *.log);;Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1365"/>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1374"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1365"/>
        <source>File %1 could not be written to.</source>
        <translation>No se pudo escribir en el archivo%1.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1434"/>
        <source>Show this row</source>
        <translation>Muestra esta fila</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1445"/>
        <source>Show All</source>
        <translation>Muestra todo</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/decodetrace.cpp" line="1453"/>
        <source>Hide All</source>
        <translation>Oculta todo</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Flag</name>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="146"/>
        <source>Text</source>
        <translation>Texto</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="155"/>
        <source>Delete</source>
        <translation>Eliminar</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/flag.cpp" line="160"/>
        <source>Disable snapping</source>
        <translation>Deshabilita snapping</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Header</name>
    <message>
        <location filename="../pv/views/trace/header.cpp" line="138"/>
        <source>Group</source>
        <translation>Grupo</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::LogicSignal</name>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="553"/>
        <source>No trigger</source>
        <translation>Sin trigger</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="566"/>
        <source>Trigger on rising edge</source>
        <translation>Trigger en flanco de subida</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="579"/>
        <source>Trigger on high level</source>
        <translation>Trigger en nivel alto</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="592"/>
        <source>Trigger on falling edge</source>
        <translation>Trigger en flanco de bajada</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="605"/>
        <source>Trigger on low level</source>
        <translation>Trigger en nivel bajo</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="618"/>
        <source>Trigger on rising or falling edge</source>
        <translation>Trigger en flanco de subida o bajada</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="706"/>
        <source> pixels</source>
        <translation> pixeles</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/logicsignal.cpp" line="710"/>
        <source>Trace height</source>
        <translation>Altura del trazo</translation>
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
        <translation>Editor de expresiones matemáticas</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="95"/>
        <source>Inputs:</source>
        <translation>Entradas:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="101"/>
        <source>Variables:</source>
        <translation>Variables:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="103"/>
        <source>Basic operators:</source>
        <translation>Operadores básicos:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="110"/>
        <source>Assignments:</source>
        <translation>Asignaciones:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="120"/>
        <source>General purpose functions:</source>
        <translation>Funciones de propósito general:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="121"/>
        <source>abs(x)		Absolute value of x</source>
        <translation>abs(x)		Valor absoluto de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="122"/>
        <source>avg(x, y, ...)	Average of all input values</source>
        <translation>avg(x, y, ...)	Promedio de todos los valores de entrada</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="123"/>
        <source>ceil(x)		Smallest integer that is greater than or equal to x</source>
        <translation>ceil(x)		Entero más pequeño mayor o igual que x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="124"/>
        <source>clamp(lb, x, ub)	Clamp x in range between lb and ub, where lb &lt; ub</source>
        <translation>clamp(lb, x, ub)	Limita x al rango entre lb y ub, donde lb &lt; ub</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="125"/>
        <source>equal(x, y)	Equality test between x and y using normalised epsilon</source>
        <translation>equal(x, y)	Prueba de igualdad entre x y y usando épsilon normalizado</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="126"/>
        <source>erf(x)		Error function of x</source>
        <translation>erf(x)		Función de error de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="127"/>
        <source>erfc(x)		Complimentary error function of x</source>
        <translation>erfc(x)		Función de error complementaria de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="128"/>
        <source>exp(x)		e to the power of x</source>
        <translation>exp(x)		e elevado a x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="129"/>
        <source>expm1(x)	e to the power of x minus 1, where x is very small.</source>
        <translation>expm1(x)	e elevado a x menos 1, para x muy pequeño.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="130"/>
        <source>floor(x)		Largest integer that is less than or equal to x</source>
        <translation>floor(x)		Entero más grande menor o igual que x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="131"/>
        <source>frac(x)		Fractional portion of x</source>
        <translation>frac(x)		Parte fraccionaria de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="132"/>
        <source>hypot(x)		Hypotenuse of x and y (i.e. sqrt(x*x + y*y))</source>
        <translation>hypot(x)		Hipotenusa de x y y (es decir, sqrt(x*x + y*y))</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="133"/>
        <source>iclamp(lb, x, ub)	Inverse-clamp x outside of the range lb and ub, where lb &lt; ub.
		If x is within the range it will snap to the closest bound</source>
        <translation>iclamp(lb, x, ub)	Limita x hacia fuera del rango lb y ub, donde lb &lt; ub.
		Si x está dentro del rango, se ajusta al límite más cercano</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="134"/>
        <source>inrange(lb, x, ub)	In-range returns true when x is within the range lb and ub, where lb &lt; ub.</source>
        <translation>inrange(lb, x, ub)	In-range devuelve verdadero cuando x está dentro del rango lb y ub, donde lb &lt; ub.</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="135"/>
        <source>log(x)		Natural logarithm of x</source>
        <translation>log(x)		Logaritmo natural de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="136"/>
        <source>log10(x)		Base 10 logarithm of x</source>
        <translation>log10(x)		Logaritmo base 10 de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="140"/>
        <source>log1p(x)		Natural logarithm of 1 + x, where x is very small</source>
        <translation>log1p(x)		Logaritmo natural de 1 + x, para x muy pequeño</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="141"/>
        <source>log2(x)		Base 2 logarithm of x</source>
        <translation>log2(x)		Logaritmo base 2 de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="142"/>
        <source>logn(x)		Base N logarithm of x, where n is a positive integer</source>
        <translation>logn(x)		Logaritmo base N de x, donde n es un entero positivo</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="143"/>
        <source>max(x, y, ...)	Largest value of all the inputs</source>
        <translation>max(x, y, ...)	Valor más grande de todas las entradas</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="144"/>
        <source>min(x, y, ...)	Smallest value of all the inputs</source>
        <translation>min(x, y, ...)	Valor más pequeño de todas las entradas</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="145"/>
        <source>mul(x, y, ...)	Product of all the inputs</source>
        <translation>mul(x, y, ...)	Producto de todas las entradas</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="146"/>
        <source>ncdf(x)		Normal cumulative distribution function</source>
        <translation>ncdf(x)		Función de distribución acumulada normal</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="147"/>
        <source>nequal(x, y)	Not-equal test between x and y using normalised epsilon</source>
        <translation>nequal(x, y)	Prueba de desigualdad entre x y y usando épsilon normalizado</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="148"/>
        <source>pow(x, y)	x to the power of y</source>
        <translation>pow(x, y)	x elevado a y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="149"/>
        <source>root(x, n)	Nth-Root of x, where n is a positive integer</source>
        <translation>root(x, n)	Raíz n-ésima de x, donde n es un entero positivo</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="150"/>
        <source>round(x)		Round x to the nearest integer</source>
        <translation>round(x)		Redondea x al entero más cercano</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="151"/>
        <source>roundn(x, n)	Round x to n decimal places, where n &gt; 0 and is an integer</source>
        <translation>roundn(x, n)	Redondea x a n decimales, donde n &gt; 0 y es un entero</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="152"/>
        <source>sgn(x)		Sign of x; -1 if x &lt; 0, +1 if x &gt; 0, else zero</source>
        <translation>sgn(x)		Signo de x; -1 si x &lt; 0, +1 si x &gt; 0, si no cero</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="153"/>
        <source>sqrt(x)		Square root of x, where x &gt;= 0</source>
        <translation>sqrt(x)		Raíz cuadrada de x, donde x &gt;= 0</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="154"/>
        <source>sum(x, y, ..,)	Sum of all the inputs</source>
        <translation>sum(x, y, ..,)	Suma de todas las entradas</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="155"/>
        <source>swap(x, y)	Swap the values of the variables x and y and return the current value of y</source>
        <translation>swap(x, y)	Intercambia los valores de las variables x y y y devuelve el valor actual de y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="156"/>
        <source>trunc(x)		Integer portion of x</source>
        <translation>trunc(x)		Parte entera de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="160"/>
        <source>Trigonometry functions:</source>
        <translation>Funciones trigonométricas:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="161"/>
        <source>acos(x)		Arc cosine of x expressed in radians. Interval [-1,+1]</source>
        <translation>acos(x)		Arco coseno de x expresado en radianes. Intervalo [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="162"/>
        <source>acosh(x)		Inverse hyperbolic cosine of x expressed in radians</source>
        <translation>acosh(x)		Coseno hiperbólico inverso de x expresado en radianes</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="163"/>
        <source>asin(x)		Arc sine of x expressed in radians. Interval [-1,+1]</source>
        <translation>asin(x)		Arco seno de x expresado en radianes. Intervalo [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="164"/>
        <source>asinh(x)		Inverse hyperbolic sine of x expressed in radians</source>
        <translation>asinh(x)		Seno hiperbólico inverso de x expresado en radianes</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="165"/>
        <source>atan(x)		Arc tangent of x expressed in radians. Interval [-1,+1]</source>
        <translation>atan(x)		Arco tangente de x expresado en radianes. Intervalo [-1,+1]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="166"/>
        <source>atan2(x, y)	Arc tangent of (x / y) expressed in radians. [-pi,+pi]  </source>
        <translation>atan2(x, y)	Arco tangente de (x / y) expresado en radianes. [-pi,+pi]</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="167"/>
        <source>atanh(x)		Inverse hyperbolic tangent of x expressed in radians</source>
        <translation>atanh(x)		Tangente hiperbólica inversa de x expresado en radianes</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="168"/>
        <source>cos(x)		Cosine of x</source>
        <translation>cos(x)		Coseno de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="169"/>
        <source>cosh(x)		Hyperbolic cosine of x</source>
        <translation>cosh(x)		Coseno hiperbólico de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="170"/>
        <source>cot(x)		Cotangent of x</source>
        <translation>cot(x)		Cotangente de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="171"/>
        <source>csc(x)		Cosectant of x</source>
        <translation>csc(x)		Cosecante de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="172"/>
        <source>sec(x)		Secant of x</source>
        <translation>sec(x)		Secante de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="173"/>
        <source>sin(x)		Sine of x</source>
        <translation>sin(x)		Seno de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="174"/>
        <source>sinc(x)		Sine cardinal of x</source>
        <translation>sinc(x)		Seno cardinal de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="175"/>
        <source>sinh(x)		Hyperbolic sine of x</source>
        <translation>sinh(x)		Seno hiperbólico de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="176"/>
        <source>tan(x)		Tangent of x</source>
        <translation>tan(x)		Tangente de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="177"/>
        <source>tanh(x)		Hyperbolic tangent of x</source>
        <translation>tanh(x)		Tangente hiperbólica de x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="178"/>
        <source>deg2rad(x)	Convert x from degrees to radians</source>
        <translation>deg2rad(x)	Convierte x de grados a radianes</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="179"/>
        <source>deg2grad(x)	Convert x from degrees to gradians</source>
        <translation>deg2grad(x)	Convierte x de grados a gradianes</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="180"/>
        <source>rad2deg(x)	Convert x from radians to degrees</source>
        <translation>rad2deg(x)	Convierte x de radianes a grados</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="181"/>
        <source>grad2deg(x)	Convert x from gradians to degrees</source>
        <translation>grad2deg(x)	Convierte x de gradianes a grados</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="185"/>
        <source>Logic operators:</source>
        <translation>Operadores lógicos:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="202"/>
        <source>Comparisons:</source>
        <translation>Comparaciones:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="203"/>
        <source>x = y or x == y	True only if x is strictly equal to y</source>
        <translation>x = y o x == y	Verdadero solo si x es exactamente igual a y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="204"/>
        <source>x &lt;&gt; y or x != y	True only if x does not equal y</source>
        <translation>x &lt;&gt; y o x != y	Verdadero solo si x no es igual a y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="205"/>
        <source>x &lt; y		True only if x is less than y</source>
        <translation>x &lt; y		Verdadero solo si x es menor que y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="206"/>
        <source>x &lt;= y		True only if x is less than or equal to y</source>
        <translation>x &lt;= y		Verdadero solo si x es menor o igual que y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="207"/>
        <source>x &gt; y		True only if x is greater than y</source>
        <translation>x &gt; y		Verdadero solo si x es mayor que y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="208"/>
        <source>x &gt;= y		True only if x is greater than or equal to y</source>
        <translation>x &gt;= y		Verdadero solo si x es mayor o igual que y</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="209"/>
        <source>Flow control:</source>
        <translation>Control de flujo:</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="210"/>
        <source>{ ... }		Beginning and end of instruction block</source>
        <translation>{ ... }		Inicio y fin del bloque de instrucciones</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="211"/>
        <source>if (x, y, z)	If x is true then return y else return z
if (x) y;			variant without implied else
if (x) { y };		variant with an instruction block
if (x) y; else z;		variant with explicit else
if (x) { y } else { z };	variant with instruction blocks</source>
        <translation>if (x, y, z)	Si x es verdadero devuelve y, si no devuelve z
if (x) y;			variante sin else implícito
if (x) { y };		variante con bloque de instrucciones
if (x) y; else z;		variante con else explícito
if (x) { y } else { z };	variante con bloques de instrucciones</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="212"/>
        <source>x ? y : z	Ternary operator, equivalent to &apos;if (x, y, z)&apos;</source>
        <translation>x ? y : z	Operador ternario, equivalente a &apos;if (x, y, z)&apos;</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="213"/>
        <source>switch {		The first true case condition that is encountered will
 case x &gt; 1: a;	determine the result of the switch. If none of the case
 case x &lt; 1: b;	conditions hold true, the default action is used
 default:     c;	to determine the return value
}</source>
        <translation>switch {		La primera condición case verdadera que se encuentre
 case x &gt; 1: a;	determina el resultado del switch. Si no se cumple
 case x &lt; 1: b;	ninguna de las condiciones case, se usa la acción
 default:     c;	default para determinar el valor de retorno
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="217"/>
        <source>while (conditon) {	Evaluates expression repeatedly as long as condition is true,
 expression;	returning the last value of expression
}</source>
        <translation>while (conditon) {	Evalúa la expresión repetidamente mientras la condición sea verdadera,
 expression;	devolviendo el último valor de la expresión
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="218"/>
        <source>repeat		Evalues expression repeatedly as long as condition is false,
 expression;	returning the last value of expression
until (condition)
</source>
        <translation>repeat		Evalúa la expresión repetidamente mientras la condición sea falsa,
 expression;	devolviendo el último valor de la expresión
until (condition)
</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="219"/>
        <source>for (var x := 0; condition; x += 1) {	Repeatedly evaluates expression while the condition is true,
 expression			while evaluating the &apos;increment&apos; expression on each loop
}</source>
        <translation>for (var x := 0; condition; x += 1) {	Evalúa la expresión repetidamente mientras la condición sea verdadera,
 expression			evaluando la expresión &apos;increment&apos; en cada iteración
}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="220"/>
        <source>break		Terminates the execution of the nearest enclosed loop, returning NaN</source>
        <translation>break		Termina la ejecución del bucle cerrado más cercano, devolviendo NaN</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="221"/>
        <source>break[x]		Terminates the execution of the nearest enclosed loop, returning x</source>
        <translation>break[x]		Termina la ejecución del bucle cerrado más cercano, devolviendo x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="222"/>
        <source>continue		Interrupts loop execution and resumes with the next loop iteration</source>
        <translation>continue		Interrumpe la ejecución del bucle y continúa con la siguiente iteración</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="223"/>
        <source>return[x]		Returns immediately from within the current expression, returning x</source>
        <translation>return[x]		Regresa inmediatamente desde la expresión actual, devolviendo x</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="224"/>
        <source>~(expr; expr; ...)	Evaluates each sub-expression and returns the value of the last one
~{expr; expr; ...}</source>
        <translation>~(expr; expr; ...)	Evalúa cada subexpresión y devuelve el valor de la última
~{expr; expr; ...}</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="236"/>
        <source>Copy to expression</source>
        <translation>Copiar a la expresión</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="249"/>
        <source>Basics</source>
        <translation>Básicos</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="250"/>
        <source>Functions 1</source>
        <translation>Funciones 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="251"/>
        <source>Functions 2</source>
        <translation>Funciones 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="252"/>
        <source>Trigonometry</source>
        <translation>Trigonometría</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="253"/>
        <source>Logic</source>
        <translation>Lógico</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="254"/>
        <source>Flow Control 1</source>
        <translation>Control de flujo 1</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="255"/>
        <source>Flow Control 2</source>
        <translation>Control de flujo 2</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="256"/>
        <source>Examples</source>
        <translation>Ejemplos</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::MathSignal</name>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="315"/>
        <source>Expression</source>
        <translation>Expresión</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="319"/>
        <location filename="../pv/views/trace/mathsignal.cpp" line="329"/>
        <source>same as session</source>
        <translation>igual que la sesión</translation>
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
        <translation>Número de muestras</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/mathsignal.cpp" line="333"/>
        <source>Sample rate</source>
        <translation>Tasa de muestreo</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Ruler</name>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="165"/>
        <source>Create marker here</source>
        <translation>Crear marcador aqui</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="169"/>
        <source>Set as zero point</source>
        <translation>Establecer como punto cero</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="174"/>
        <source>Reset zero point</source>
        <translation>Reiniciar punto cero</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="187"/>
        <source>Disable mouse hover marker</source>
        <translation>Deshabilitar marcador de desplazamiento del mouse</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/ruler.cpp" line="187"/>
        <source>Enable mouse hover marker</source>
        <translation>Habilitar marcador de desplazamiento del mouse</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Signal</name>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="155"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="169"/>
        <source>Remove</source>
        <translation>Quitar</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/signal.cpp" line="171"/>
        <source>Disable</source>
        <translation>Deshabilitar</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::StandardBar</name>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="174"/>
        <source>Zoom &amp;In</source>
        <translation>&amp;Acercar</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="175"/>
        <source>Zoom &amp;Out</source>
        <translation>A&amp;lejar</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="176"/>
        <location filename="../pv/views/trace/standardbar.cpp" line="177"/>
        <source>Zoom to &amp;Fit</source>
        <translation>Hacer Zoom para &amp;Encajar</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="178"/>
        <source>Show &amp;Cursors</source>
        <translation>Mostrar &amp;Cursores</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="179"/>
        <source>Show Cursors</source>
        <translation>Mostrar cursores</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="180"/>
        <source>Display last segment only</source>
        <translation>Mostrar solo el último segmento</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="181"/>
        <source>Display last complete segment only</source>
        <translation>Mostrar solo el último segmento completo</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/standardbar.cpp" line="182"/>
        <source>Display a single segment</source>
        <translation>Mostrar solo un segmento</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TimeMarker</name>
    <message>
        <location filename="../pv/views/trace/timemarker.cpp" line="223"/>
        <source>Time</source>
        <translation>Tiempo</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Trace</name>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="238"/>
        <source>Create marker here</source>
        <translation>Crear marcador aqui</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="400"/>
        <source>Color</source>
        <translation>Color</translation>
    </message>
    <message>
        <location filename="../pv/views/trace/trace.cpp" line="465"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::TraceGroup</name>
    <message>
        <location filename="../pv/views/trace/tracegroup.cpp" line="144"/>
        <source>Ungroup</source>
        <translation>Desagrupar</translation>
    </message>
</context>
<context>
    <name>pv::views::trace::Viewport</name>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="303"/>
        <location filename="../pv/views/trace/viewport.cpp" line="349"/>
        <source>Width: </source>
        <translation>Ancho: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="350"/>
        <source>Period: </source>
        <translation>Periodo: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="351"/>
        <source>Frequency: </source>
        <translation>Frecuencia: </translation>
    </message>
    <message>
        <location filename="../pv/views/trace/viewport.cpp" line="352"/>
        <source>Duty Cycle: </source>
        <translation>Ciclo de trabajo: </translation>
    </message>
</context>
<context>
    <name>pv::widgets::DecoderGroupBox</name>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="48"/>
        <source>Show/hide this decoder trace</source>
        <translation>Mostrar / ocultar este trazo de decodificador</translation>
    </message>
    <message>
        <location filename="../pv/widgets/decodergroupbox.cpp" line="58"/>
        <source>Delete this decoder trace</source>
        <translation>Eliminar este trazo de decodificador</translation>
    </message>
</context>
<context>
    <name>pv::widgets::DeviceToolButton</name>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="83"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="339"/>
        <source>Refresh Device List</source>
        <translation>Actualizar lista de dispositivos</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="89"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="105"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="112"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="221"/>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="228"/>
        <source>&lt;No Device&gt;</source>
        <translation>&lt;Sin dispositivo&gt;</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="298"/>
        <source>This device is in use by another session</source>
        <translation>Otra sesión está usando este dispositivo</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="311"/>
        <source>This device is already open in another session</source>
        <translation>Este dispositivo ya está abierto en otra sesión</translation>
    </message>
    <message>
        <location filename="../pv/widgets/devicetoolbutton.cpp" line="312"/>
        <source>Open this device in a new session</source>
        <translation>Abrir este dispositivo en una sesión nueva</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ExportMenu</name>
    <message>
        <location filename="../pv/widgets/exportmenu.cpp" line="73"/>
        <source>Export %1...</source>
        <translation>Exporta %1...</translation>
    </message>
</context>
<context>
    <name>pv::widgets::ImportMenu</name>
    <message>
        <location filename="../pv/widgets/importmenu.cpp" line="70"/>
        <source>Import %1...</source>
        <translation>Importa %1...</translation>
    </message>
</context>
</TS>
