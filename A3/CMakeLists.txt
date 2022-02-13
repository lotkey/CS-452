cmake_minimum_required(VERSION 3.12)
set(PICO_BOARD adafruit_feather_rp2040)
include(pico_sdk_import.cmake)

project(assignment3)

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

add_executable(assignment3
    main.cpp
)

set_target_properties(assignment3 PROPERTIES
    CXX_STANDARD 20
)

pico_enable_stdio_usb(assignment3 1)
pico_enable_stdio_uart(assignment3 1)

target_link_libraries(assignment3 pico_stdlib freertos)
pico_add_extra_outputs(assignment3)

