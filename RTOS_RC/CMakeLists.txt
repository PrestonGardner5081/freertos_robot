add_executable(rtos_rc
        main.c
        pico_bt_connection.c
)

target_link_libraries(rtos_rc 
        pico_stdlib 
        freertos     
        pico_cyw43_arch_none
        pico_btstack_ble
        pico_btstack_classic
        pico_btstack_cyw43
        hardware_pwm
)
target_include_directories(rtos_rc PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} # For btstack config
)

pico_add_extra_outputs(rtos_rc)

pico_enable_stdio_usb(rtos_rc 1)
pico_enable_stdio_uart(rtos_rc 0)