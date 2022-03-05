cmake_minimum_required(VERSION 3.12)
set(PICO_BOARD adafruit_feather_rp2040)
include(pico_sdk_import.cmake)

project(assignment10)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)
pico_sdk_init()

set(PICO_SDK_FREERTOS_SOURCE /home/lotkey/pico/FreeRTOS-Kernel)

add_library(freertos
    ${PICO_SDK_FREERTOS_SOURCE}/event_groups.c
    ${PICO_SDK_FREERTOS_SOURCE}/list.c
    ${PICO_SDK_FREERTOS_SOURCE}/queue.c
    ${PICO_SDK_FREERTOS_SOURCE}/stream_buffer.c
    ${PICO_SDK_FREERTOS_SOURCE}/tasks.c
    ${PICO_SDK_FREERTOS_SOURCE}/timers.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/MemMang/heap_3.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos PUBLIC
    .
    ${PICO_SDK_FREERTOS_SOURCE}/include
    ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0
)

add_executable(assignment10
    main.cpp

    Board/Button.cpp
    Board/Dipswitch.cpp
    Board/Led/Seven_segment.cpp
    Board/Led/D13.cpp
    Board/Led/Pixel.cpp
    Board/Util/Color.cpp

    I2C/I2C.cpp
    I2C/HDC1080/HDC1080.cpp

    Step_motor/Step_motor.cpp
)

set_target_properties(assignment10 PROPERTIES
    CXX_STANDARD 20
)

pico_enable_stdio_usb(assignment10 1)
pico_enable_stdio_uart(assignment10 0)
pico_add_extra_outputs(assignment10)

target_link_libraries(assignment10
    pico_stdlib
    freertos
    pico_binary_info
    hardware_gpio
    hardware_i2c
    hardware_adc
    hardware_pio
)