cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE Debug)

set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(Pico-FreeRTOS)

pico_sdk_init()

add_subdirectory(freertos)
add_subdirectory(RTOS_RC)
