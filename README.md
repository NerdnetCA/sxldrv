# sxldrv
Linux uinput-based driver for SteelSeries Stratus XL gamepad.

Purpose: Connect to the SteelSeries Stratus XL gamepad and translate its idiosyncratic functionality to appear as an X-Box controller.

Status: Yes, it's possible to make this work...

This is currently VERY rough, but since I'm able to make it work for myself, there is little incentive to clean it up.

Usage:
sxldrv -h[d] </dev/hidrawX>

The -d option flag will dump the HID report descriptor associated with the device.  Afterward, or if not given, the tool will blindly open the raw HID node and assume it knows the report format.

Tip: use `evtest /dev/input/eventX --grab >/dev/null` to squelch input from the kernel joystick device.
