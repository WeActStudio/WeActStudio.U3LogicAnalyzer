# MCP Server

[中文](mcp-server.zh_CN.md) · [Back to README](../README.md)

`u3logicanalyzer-mcp-server` is a stdio MCP server that exposes the acquisition and decoding
capabilities of the analyzer to AI clients speaking the Model Context Protocol. It does not link
libsigrok itself but drives the `u3logicanalyzer-cli` that ships with the package, so it
uses the same
drivers, decoders and firmware as the application.

Protocol and tool set follow KenosInc/sigrok-mcp-server (MIT licence). This project reimplements it
in C and adds three tools; see "Differences from upstream" below.

## Location

| Platform | Path |
|---|---|
| Windows | `C:\Program Files\WeAct Studio\U3LogicAnalyzer\u3logicanalyzer-mcp-server.exe` |
| Linux (deb / tar.gz) | `/opt/U3LogicAnalyzer/u3logicanalyzer-mcp-server` |
| Linux (AppImage) | `usr/bin/u3logicanalyzer-mcp-server` inside the mounted image |
| macOS | `/Applications/U3LogicAnalyzer.app/Contents/MacOS/u3logicanalyzer-mcp-server` |

The server looks for `u3logicanalyzer-cli` in its own directory; decoders and firmware are
resolved by that program itself, so no environment variable is needed.

## Client configuration

A step by step guide for end users (the full `mcp.json` of each platform, activation,
troubleshooting) is in [the agent setup section](usage.md#agent-setup) of the user guide, which
also ships as a PDF with the packages. Only the parts specific to this implementation are
repeated below.

Add one entry to the MCP configuration of the client and point `command` at the installed binary:

```json
{
  "mcpServers": {
    "u3logicanalyzer": {
      "command": "/Applications/U3LogicAnalyzer.app/Contents/MacOS/u3logicanalyzer-mcp-server",
      "args": [],
      "env": {
        "U3LOGICANALYZER_TIMEOUT_SECONDS": "600",
        "U3LOGICANALYZER_WORKING_DIR": "/Users/<name>/U3LogicAnalyzer/workdir"
      },
      "disabled": false
    }
  }
}
```

Restart the client afterwards and trust the server in its connector page; it is not started before
that.

### Environment variables

| Variable | Default | Purpose |
|---|---|---|
| `U3LOGICANALYZER_CLI_PATH` | `u3logicanalyzer-cli` next to the server | Only needed when the program lives elsewhere |
| `U3LOGICANALYZER_TIMEOUT_SECONDS` | `30` | Per-call timeout; raise it for long acquisitions |
| `U3LOGICANALYZER_WORKING_DIR` | inherited | Where captured files are written when `output_file` is omitted. It has to exist and be writable by the current user; do not point it at the installation directory |

## Tools

| Tool | Purpose |
|---|---|
| `scan_devices` | Scan for devices, optionally narrowed to one driver and connection |
| `show_driver_details` | Driver functions, scan options, channel groups, sample rates, triggers |
| `capture_data` | Acquire and save to a file; either `samples` or `time` |
| `decode_protocol` | Run protocol decoders over a saved file |
| `show_decoder_details` | Decoder documentation, options, channels, annotation classes |
| `list_supported_hardware` | Every hardware driver of this build |
| `list_supported_decoders` | Every decoder; `filter` matches id or description |
| `list_input_formats` / `list_output_formats` | Readable / writable file formats |
| `show_version` | Versions of the command line tool, libsigrok and libsigrokdecode |
| `check_firmware_status` | Firmware search directories and file counts, including the bundled one |
| `capture_and_decode` | Capture and decode in one call (addition) |
| `set_device_options` | Apply device options without capturing, to verify them (addition) |

## Typical workflow

```
scan_devices
  → show_driver_details  driver=wch-ch32h417-libusb
  → capture_and_decode  driver=wch-ch32h417-libusb  config=samplerate=4m
                        channels=D0  samples=1000000
                        protocol_decoders=uart:rx=D0:baudrate=115200
```

One round trip yields both the saved file and the decoded result; the file stays on disk, so another
decoder can be applied later without capturing again.

### Device options

The `config` argument of `capture_data`, `capture_and_decode` and `set_device_options` maps to the
`--config` option of the command line:

- Separate options with `:`, for example `samplerate=4m:voltage_threshold=1.2`. A comma is accepted
  too; the server rewrites it.
- An option of a range type takes `low-high`. `voltage_threshold` is one, and a bare number is
  rejected; the server widens a single value into `v-v`, so `voltage_threshold=1.2` and `1.2-1.2`
  are equivalent.
- That threshold is the level on the pins. This board takes 0.60 V to 1.50 V, and the error message
  states the range when a value falls outside it.
- The samplerate has to be one the device offers; `show_driver_details` lists them. The CH32H417 has
  25 steps from 4 MHz to 200 MHz.

## Differences from upstream

- `serial_query` (send text commands over a serial port) and `get_device_profile` (a database of SCPI
  instruments) target bench instruments and are not part of this implementation.
- `capture_and_decode` combines capture and decode into one call.
- `set_device_options` applies options without acquiring, to confirm the device accepts them.
- `list_supported_decoders` gained a `filter` argument; `check_firmware_status` also reports the
  firmware directory that ships with this package.

## Troubleshooting

1. **`u3logicanalyzer-cli could not be started`** — `U3LOGICANALYZER_CLI_PATH` is wrong, or the
   program is not next to the server.
2. **Timeouts** — raise `U3LOGICANALYZER_TIMEOUT_SECONDS`; the 30 second default is short for long captures.
3. **`show_decoder_details` reports `decoder sources not found`** — the server looks for
   `decoders` next to itself; this error means the folder is missing from the installation,
   so reinstall. `SIGROKDECODE_DIR` is only needed when the decoders live elsewhere.
4. **`invalid argument` on capture** — the sample rate is not one the device offers; `show_driver_details`
   lists them. The CH32H417 covers 4 MHz to 200 MHz.
