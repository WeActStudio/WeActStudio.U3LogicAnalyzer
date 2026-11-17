# Using U3LogicAnalyzer

[中文](usage.zh_CN.md) · [Back to README](../README.md)

This guide covers the everyday use of U3LogicAnalyzer: connecting a device, capturing, reading the waveform, adding protocol decoders and saving the result.

![Main window](images/en/main-window.png)

## Window layout

| Area | Contents |
|---|---|
| Tool bar | Open / Save / device selector / device options / capture length / sample rate / capture mode / start / zoom to fit / measure / decoder / decoding result / export |
| Waveform view | One row per channel, channel labels on the left, time axis on top |
| Protocol decoder panel | Search box, decoder tree and the description of the selected decoder |
| Tab bar (bottom) | One tab per session: one device or one file |
| Status bar | Device name and connection id, e.g. `ch375/5-2` |

The interface follows the system language. To select one manually: Help → Settings → "User interface language"; the choice applies immediately.

## Connecting a device

The application scans for devices as soon as one is plugged in, and the device selector shows the model and port, e.g. `WeAct Studio Logic Analyzer V2 ch375/5-2`.

- **One device**: it is the only entry in the list; start capturing directly.
- **Several devices**: the list holds one row per port. Each row offers "open in a new session", so every device can capture into a session of its own at the same time.
- **Hot plug**: unplugging a capturing device leaves that page behind an offline mask, and no capture can be started there until the device is reconnected. Plugging or unplugging other devices during a capture does not affect the device being captured.

On Windows the CH375DLL driver is the default. To use the libusb back end instead (or an FX2-based analyzer), rebind the device to WinUSB with Zadig first; see the README of the U3LogicAnalyzer repository.

## Device options

The device options button (gear icon) on the tool bar opens the device options panel:

![Device options](images/en/device-options.png)

| Item | Description |
|---|---|
| Threshold level | Range of the high/low decision; follows the input level |
| Input level | Voltage of the signal under test, 1.0–3.3 V. **It must match the level actually applied**: select 3.3 V for 3.3 V logic |
| Pre-trigger | Share of the capture window reserved for the time before the trigger (0–100%); see Capturing |
| Channel count | 2 / 4 / 8 / 16 channels. **With 8 channels** each sample takes one byte, so the same memory holds twice as many samples |
| Channel boxes | Channels that take part in the capture (affects display and decoding only; the data is still packed by channel count) |
| Indicator brightness | Five steps (Dim / Default / Bright / Brighter / Maximum). Remembered per device and restored when it is reconnected |
| Board key start/stop | Ticks the key on the board as a start/stop button: one short press starts a capture, another ends it; a two-second hold still switches slots. **The change takes effect when this panel is closed** |

A wrong input level places the threshold incorrectly: set too high, genuine highs read as low; set too low, noise reads as high.

> Changing the channel count rebuilds the table of available sample rates. This reflects the device itself rather than an application defect: settle the channel count first, then select the rate.

## Sample rate and capture length

- **Sample rate**: 25 steps from 4 MHz to 200 MHz on a USB 3.0 link; the table follows the link and the channel count (4–20 MHz at 16 channels and 4–40 MHz at 8 channels on USB 2.0).
  ![Sample rate](images/en/sample-rate.png)
- **Capture length**: from 10 ms. The software uses an 18 GB data figure to define the length ceiling (samples = rate × time, 2 bytes per sample at 16 channels): about 45 s at 200 MHz / 16 channels, 90 s at 8 channels (the actual length is limited by free PC memory).

Rules of thumb:

| Use case | Rate | Duration |
|---|---|---|
| UART / I²C / SPI and other slow buses | At least 5× the baud rate or clock; 10–50 MHz is usual | Long enough for the message |
| Fast parallel buses | 100–200 MHz | As short as possible; keep memory use in view |
| Slow level changes | 4–10 MHz | A few seconds |

## Capturing

Press Start on the tool bar, or the space bar, to begin; press again to stop. When the capture ends, the view zooms to fit the data.

- **Mode → Single**: stops once the configured number of samples has been collected.
- **Mode → Repeat**: captures continuously until stopped; each round replaces the previous one.
- **Pre-trigger**: at 50%, half of the window holds samples from before the trigger and the rest samples after it. The device has no hardware trigger unit; the setting divides the capture window, which is what reveals the state preceding an event.
- **Long captures**: all data stays in memory. The application checks free memory while data arrives and stops the capture below 512 MB with a message, keeping the data already captured. Fewer channels, a lower rate or a shorter duration allow a longer capture.

## Reading the waveform

| Action | Procedure |
|---|---|
| Zoom the time axis | Scroll the mouse wheel over the waveform view, or use Zoom to Fit for the whole capture |
| Scroll | Drag the waveform, or use the scroll bar |
| Measure | The Measure button opens two cursors; drag them to read the time difference and frequency |
| Channel actions | Right-click a channel label to rename it, show or hide it, or change its height |
| Sample markers | The view options can draw sample points, which shows how dense the sampling is |

## Protocol decoders

The decoder button (or `D`) opens the panel on the right:

1. Type a protocol name into the search box, e.g. `uart`, `i2c` or `spi`.
2. Open a category, or double-click the decoder in the search results.
3. In the configuration dialog, assign the channels and set the options (the RX channel and baud rate for UART, for instance), then confirm.
4. The decoded rows appear below the waveform; Decoding Result switches to a list view of them.

A session can stack several decoders, for example SPI flash commands and data on one bus. There are 132 decoders; the full list is in the `decoders` folder of the installation directory (e.g. `C:\Program Files\WeAct Studio\U3LogicAnalyzer\decoders`).

Decoded results can be exported: the Export button writes several text formats, and "Export to a New Session" copies the waveform and its decoders into a new tab, which leaves the original capture unchanged.

## Saving and exporting

- **Save / Save As**: a sigrok session file (`.sr`, a zip container) holding channel data and the sample rate. An 8-channel capture stays 8 channels wide and is not read back as 16.
- **Save Selected Range As**: only the samples in the current view.
- **Save Session Setup**: the configuration only (device options, decoders, view), without data.
- **Open**: from the menu, or on the command line:

```bash
./U3LogicAnalyzer capture.sr        # Windows: U3LogicAnalyzer.exe capture.sr
./U3LogicAnalyzer -i capture.sr
```

Opening a file builds the session from the channels it actually holds: only those channels get a row, and the channel dialog lists those alone.

![Session file](images/en/waveform.png)

## Sessions and multiple devices

- The tab bar at the bottom holds several sessions at once: one per device, or one per `.sr` file.
- The first session cannot be closed, and opening a file always adds a tab.
- "Export to a New Session" copies the result into a tab of its own; the capture it came from stays unchanged.
- Closing the last file tab restores the device session.

## Firmware update

Help menu → **FW Update**.

- The update runs over the device IAP. The tab cannot be closed while it runs, so an interrupted write still has a tab from which to retry.
- Progress is shown on the page; the device reconnects and is read again when the update finishes.
- Do not unplug the device during the write. A failed update can be retried while the device is still in IAP mode.

## Troubleshooting

**The device is not in the device list**
- Windows: make sure no other program holds the device; the libusb back end requires a WinUSB rebind with Zadig first.
- Linux: `lsusb` lists the device but the application does not. This is a permission problem: run `sudo ./install_udev_rules.sh` and reconnect the device.

**The capture stops at once with 0 samples**
- The sample rate exceeds what the link supports, or the sample count is too large. Lower the rate or shorten the duration and try again.
- The device may still be held by a previous program: unplug and reconnect it.

**The interface is in English**
- The language was set manually. Go to Help → Settings → "User interface language" and select the required language.

**A low-memory notice appears at startup**
- The application checks free memory at startup and asks for confirmation below 4 GB. Close some programs or confirm to continue.
