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
#include "pico/stdlib.h"
#include "hardware/i2c.h"
//#include "hardware/uart.h"
//#include "hardware/gpio.h"


// Project libraries
#include "dev_hd44780.h"
//#include "bsp.h"



#define LCD_SDA_PIN 16
#define LCD_SCL_PIN 17


int main() {

    const uint led_pin = 25;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // Initialize chosen serial port
    stdio_init_all();


    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(LCD_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(LCD_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(LCD_SDA_PIN);
    gpio_pull_up(LCD_SCL_PIN);


    dev_hd44780_init(i2c0, 0x27);
    dev_hd44780_text(i2c0, 0x27, 0, true, "Testeando");
    sleep_ms(2000);
    dev_hd44780_text(i2c0, 0x27, 1, true, "--------------------");
    sleep_ms(500);
    dev_hd44780_text(i2c0, 0x27, 2, true, "pantalla");
    sleep_ms(500);
    dev_hd44780_text(i2c0, 0x27, 3, true, "2022");


    //dev_hd44780_bargraph(i2c0, 0x27, 0, true, 50);

    // Loop forever
    while (true) {

        // Blink LED
        printf("Blinking!\r\n");
        gpio_put(led_pin, true);
        sleep_ms(1000);
        gpio_put(led_pin, false);
        sleep_ms(1000);
    }
}


/************************ Camilo Vera **************************END OF FILE****/
