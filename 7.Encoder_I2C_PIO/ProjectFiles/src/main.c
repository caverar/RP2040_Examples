/** 
  ******************************************************************************
  * @file    main.c
  * @author  Camilo Vera
  * @brief   Main program.
  *          Description of the file functions, etc. 
  *
  @verbatim

  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  Example 04: LCD_I2C 
  This code shows the basic usage of an RP2040 CMake project, with an I2C LCD 
  screen
   
  RP2040 (Two Cortex M0+)

  @endverbatim
  ****************************************************************************** 
*/

/* Includes ------------------------------------------------------------------*/

// Standard C libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// SDK Libraries
#include "pio_i2c.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
//#include "hardware/uart.h"
//#include "hardware/gpio.h"


// Project libraries
#include "AS5600.h"
//#include "bsp.h"



#define ENCODER1_SDA_PIN 12
#define ENCODER1_SCL_PIN 13


int main() {

    const uint led_pin = 25;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // Initialize chosen serial port
    stdio_init_all();



    //i2c_init(i2c0, 1000 * 1000);


    u_int32_t offset = pio_add_program(pio0, &i2c_program);
    i2c_program_init(pio0, 0, offset, ENCODER1_SDA_PIN, ENCODER1_SCL_PIN);

    //gpio_set_function(ENCODER1_SDA_PIN, GPIO_FUNC_I2C);
    //gpio_set_function(ENCODER1_SCL_PIN, GPIO_FUNC_I2C);
    //gpio_pull_up(ENCODER1_SDA_PIN);
    //gpio_pull_up(ENCODER1_SCL_PIN);




    // Loop forever
    while (true) {

        // Blink LED

        u_int16_t angle= read_angle(i2c0);
        printf("%d\n",angle);
        sleep_ms(10);

    }
}


/************************ Camilo Vera **************************END OF FILE****/
