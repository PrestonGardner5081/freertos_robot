#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "btstack.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico_bt_connection.h"
#include "hardware/pwm.h"
#include "time.h"

const uint32_t COMMAND_POLE_INTERVAL = 1000; // Pole for commands every 1000 us = 0.001 s

int picow_bt_init(void) {
    // initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    return 0;
}

void led_task()
{
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(125);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        vTaskDelay(125);
    }
}

/*! \brief Initialise a set of GPIO pins for PWM
 *
 * \param pins array of pin numbers 
 * \param num_pins length of pins
 */
void pwm_setup(int *pins, int num_pins){
    for(int i = 0; i < num_pins; i++){
        gpio_set_function(pins[i], GPIO_FUNC_PWM);
    }
}

void process_input_task(){
    uint32_t start_time, stop_time, remaining_time;

    while(true){
        start_time = time_us_32();
        
        struct CommandState cur_state = bt_connection_get_internal_command_state();

        

        stop_time = time_us_64();
        remaining_time = COMMAND_POLE_INTERVAL - stop_time + start_time;
        if (remaining_time > 0)
            usleep(remaining_time); //sleep for remaining portion of 10ms
    }
}

void pwm_task(){
    const uint motor_driver_pin_B = 2;
    const uint motor_driver_pin_A = 28;
    const uint motor_driver_BI1 = 26;
    const uint motor_driver_BI2 = 27;

    const uint motor_driver_AI1 = 21;
    const uint motor_driver_AI2 = 20;
    
    //num cycles to split pwm into
    uint16_t max_brightness = 65535;

    // Tell GPIO pin it is allocated to PWM
    gpio_set_function(motor_driver_pin_B, GPIO_FUNC_PWM);
    gpio_set_function(motor_driver_pin_A, GPIO_FUNC_PWM);
    
    gpio_init(motor_driver_BI1);
    gpio_init(motor_driver_BI2);
    gpio_init(motor_driver_AI1);
    gpio_init(motor_driver_AI2);
    gpio_set_dir(motor_driver_BI1, GPIO_OUT);
    gpio_set_dir(motor_driver_BI2, GPIO_OUT);    
    gpio_set_dir(motor_driver_AI1, GPIO_OUT);
    gpio_set_dir(motor_driver_AI2, GPIO_OUT);

    // Find out which PWM slice is connected to motor_driver_pin_B
    uint motor_pin_b_slice_num = pwm_gpio_to_slice_num(motor_driver_pin_B);
    uint motor_pin_a_slice_num = pwm_gpio_to_slice_num(motor_driver_pin_A);
    uint motor_pin_b_channel = pwm_gpio_to_channel(motor_driver_pin_B);
    uint motor_pin_a_channel = pwm_gpio_to_channel(motor_driver_pin_A);

    // init pwm to level 0
    pwm_set_gpio_level(motor_driver_pin_B, 0);
    pwm_set_gpio_level(motor_driver_pin_A, 0);
    
    // Start the PWM 
    pwm_config config = pwm_get_default_config();
    pwm_init(motor_pin_b_slice_num, &config, true);
    pwm_init(motor_pin_a_slice_num, &config, true);


    while (true) {
        gpio_put(motor_driver_BI1, 1);
        gpio_put(motor_driver_BI2, 0);        
        gpio_put(motor_driver_AI1, 0);
        gpio_put(motor_driver_AI2, 1);

        // Fade up from 0 -> 100 percent 
        for(int i = max_brightness / 2; i < max_brightness; i++){
            pwm_set_gpio_level(motor_driver_pin_B, i);
            pwm_set_gpio_level(motor_driver_pin_A, i);
            sleep_us(50);  
        }      
        // Fade down from 100 -> 0 percent 
        for(int i = max_brightness - 1; i >= max_brightness / 2; i--){
            pwm_set_gpio_level(motor_driver_pin_B, i);
            pwm_set_gpio_level(motor_driver_pin_A, i);
            sleep_us(50);  
        }     

        gpio_put(motor_driver_BI1, 0);
        gpio_put(motor_driver_BI2, 1);
        gpio_put(motor_driver_AI1, 1);
        gpio_put(motor_driver_AI2, 0);

        // Fade up from 0 -> 100 percent 
        for(int i = max_brightness / 2; i < max_brightness; i++){
            pwm_set_gpio_level(motor_driver_pin_B, i);
            pwm_set_gpio_level(motor_driver_pin_A, i);
            sleep_us(50);  
        }      
        // Fade down from 100 -> 0 percent 
        for(int i = max_brightness - 1; i >= max_brightness / 2; i--){
            pwm_set_gpio_level(motor_driver_pin_B, i);
            pwm_set_gpio_level(motor_driver_pin_A, i);
            sleep_us(50);  
        }      
    }
}

int main()
{
    stdio_init_all();
    picow_bt_init();

    spp_service_setup();

    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(pwm_task, "Input_Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}
