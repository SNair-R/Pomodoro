cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(lcd)

pico_sdk_init()

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m0plus -mthumb")
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

add_executable(lcd
    main.c
    lcd_1602_i2c.c
)


        
# pull in common dependencies and additional i2c hardware support
target_link_libraries(lcd
    pico_stdlib
    hardware_i2c
    hardware_adc
)

pico_enable_stdio_usb(lcd 1)
pico_enable_stdio_uart(lcd 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(lcd)
