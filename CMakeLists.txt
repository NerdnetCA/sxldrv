cmake_minimum_required(VERSION 3.0)

project(sxldrv LANGUAGES C)

add_executable(sxldrv main.c mytools.c stratusxl.c)

target_link_libraries(sxldrv
udev
)

install(TARGETS sxldrv RUNTIME DESTINATION local/bin)
