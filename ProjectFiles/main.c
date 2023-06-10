#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "btstack.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico_bt_connection.h"
#include "hardware/pwm.h"

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

void pwm_task(){
    const uint motor_driver_pin_B = 2;
    
    //num cycles to split pwm into
    uint16_t max_brightness = 65535;

    // Tell GPIO pin it is allocated to PWM
    gpio_set_function(motor_driver_pin_B, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to motor_driver_pin_B
    uint motor_pin_b_slice_num = pwm_gpio_to_slice_num(motor_driver_pin_B);
    uint motor_pin_b_channel = pwm_gpio_to_channel(motor_driver_pin_B);

    // Set period of max_brightness cycles (0 to (max_brightness - 1) inclusive)
    // pwm_set_wrap(motor_pin_b_slice_num, max_brightness - 1);

    // init pwm to level 0
    pwm_set_gpio_level(motor_driver_pin_B, 4);
    
    // Start the PWM 
    pwm_config config = pwm_get_default_config();
    pwm_init(motor_pin_b_slice_num, &config, true);


    while (true) {
        // Fade up from 0 -> 100 percent 
        for(int i = 0; i < max_brightness; i++){
            pwm_set_gpio_level(motor_driver_pin_B, i);
            sleep_us(50);  
        }      
        // Fade down from 100 -> 0 percent 
        for(int i = max_brightness - 1; i >= 0; i--){
            pwm_set_gpio_level(motor_driver_pin_B, i);
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
