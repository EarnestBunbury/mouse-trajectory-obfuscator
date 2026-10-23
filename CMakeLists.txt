cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(mouse_trajectory_obfuscator C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(mouse_trajectory_obfuscator
    main.c
)

target_link_libraries(mouse_trajectory_obfuscator
    pico_stdlib
    tinyusb_device
    tinyusb_board
)

pico_enable_stdio_usb(mouse_trajectory_obfuscator 1)
pico_enable_stdio_uart(mouse_trajectory_obfuscator 0)

pico_add_extra_outputs(mouse_trajectory_obfuscator)
