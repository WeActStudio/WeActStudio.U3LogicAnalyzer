# Firmware for the FX2 based analyzers

These are the `fx2lafw-*.fw` files of the **sigrok-firmware-fx2lafw** package,
built from the [fx2lafw firmware](https://github.com/sigrokproject/fx2lafw)
(GPL, version 2 or later), the open source firmware that turns a Cypress FX2
into a logic analyzer.

They are here because an FX2 has **no flash of its own**: the firmware is not
kept in the board but downloaded into its RAM by the driver, every time the
board is powered and scanned. The board then re-enumerates as `sigrok fx2lafw`
with an ID of its own, and the driver opens it. Losing power loses the
firmware, so the files have to be there every time - without them an FX2 based
analyzer cannot be used at all.

The build installs this directory to `share/sigrok-firmware/` of the install
tree, which is one of the places libsigrok looks (`sr_resourcepaths_get()`),
and the driver picks the file that goes with the board it found:

| Board | File |
|---|---|
| Saleae Logic and its clones (ESLA100, Robomotic MiniLogic, MCU123) | `fx2lafw-saleae-logic.fw` |
| CWAV USBee AX, ARMFLY AX-Pro / Mini-Logic, ESLA201A, LHT00SU1 | `fx2lafw-cwav-usbeeax.fw` |
| CWAV USBee DX, XZL-Studio DX | `fx2lafw-cwav-usbeedx.fw` |
| CWAV USBee SX / ZX | `fx2lafw-cwav-usbeesx.fw` / `fx2lafw-cwav-usbeezx.fw` |
| Cypress FX2 without an EEPROM | `fx2lafw-cypress-fx2.fw` |
| sigrok fx2grok (8ch / 16ch), usb-c-grok | `fx2lafw-sigrok-fx2-8ch.fw` / `-16ch.fw` |
| Hantek 6022BE / 6022BL, PSO2020, Instrustar ISDS205x | `fx2lafw-hantek-*.fw`, `fx2lafw-instrustar-isds205b.fw` |
| Braintechnology USB-LPS, Sainsmart DDS120, Yixingdianzi MDSO | `fx2lafw-braintechnology-usb-lps.fw`, `fx2lafw-sainsmart-dds120.fw`, `fx2lafw-yixingdianzi-mdso.fw` |

Taking the set from the package rather than from a build of our own is
deliberate: the firmware versions are the ones the released sigrok talks to,
and the driver checks the major version it finds in the device before it uses
it (`FX2LAFW_REQUIRED_VERSION_MAJOR`).
