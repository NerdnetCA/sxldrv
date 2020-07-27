# sxldrv
Linux uinput-based driver for SteelSeries Stratus XL gamepad.

Purpose: Connect to the SteelSeries Stratus XL gamepad and translate its idiosyncratic functionality to appear as an X-Box controller.

Status: Rough.

# How to Use

1. Build with scons.

2. Set up bin/sxlservice to start at boot. (Working directory is important! bin/sxlservice has to be able to find bin/sxldrv)

3. Profit. A virtual device will be created, emulating an X-Box gamepad.

# Caveats

1. Currently requires evtest. (Will fix when I feel like it)

2. Doesn't need to be two separate processes. (Maybe will fix when I feel like it)

3. Uses a hard-coded workaround for the common problem with this controller, that the triggers do not rest at zero. The driver remaps the raw controller range with a huge (50%) dead zone.

