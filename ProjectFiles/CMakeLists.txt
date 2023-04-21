add_executable(blink
        main.c
)

target_link_libraries(blink 
        pico_stdlib 
        freertos     
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_none
)
target_include_directories(blink PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} # For btstack config
)

pico_add_extra_outputs(blink)

pico_enable_stdio_usb(blink 1)
pico_enable_stdio_uart(blink 0)