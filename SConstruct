

src_sxldrv = [
	'src/hid.c',
	'src/mytools.c',
	'src/stratusxl.c',
	'src/main.c'
]

src_service = [
	'src/sxlservice.c'
]

libs = [ 'udev'
]

Program('bin/sxldrv',
        src_sxldrv,
	LIBS=libs)

Program('bin/sxlservice',
	src_service,
	LIBS=libs)
