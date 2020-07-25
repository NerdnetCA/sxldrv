# sxldrv
Linux uinput-based driver for SteelSeries Stratus XL gamepad.

Purpose: Connect to the SteelSeries Stratus XL gamepad and translate its idiosyncratic functionality to appear as an X-Box controller.

Status: Rough.

# How to Use

scons

Usage:
[sudo] sxldrv

The -d option flag will dump the HID report descriptor associated with the device.  This is a cruft feature.

Additionally, the -p option can be used to specify the /dev/hidrawX node.

Tip: use `evtest /dev/input/eventX --grab >/dev/null` to squelch input from the kernel joystick device.
