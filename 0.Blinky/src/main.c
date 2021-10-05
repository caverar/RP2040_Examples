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
  Example 01: Blink
 
  This code shows the basic usage of an RP2040 CMake project, with FreeRTOS
  included.
   
  RP2040 (Two Cortex M0+)

  @endverbatim
  ****************************************************************************** 
*/

/* Includes ------------------------------------------------------------------*/

// Standard C libraries
#include <stdio.h>

// Hardware API libraries
#include "pico/stdlib.h"

// High level API

// FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"



/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/



/* Code Separator: Description -----------------------------------------------*/
/*----------------------------------------------------------------------------*/

    /* Subcode Separator: Description ----------------------------------------*/
    /*------------------------------------------------------------------------*/

        /* SubSubcode Separator: Description ---------------------------------*/
        /*--------------------------------------------------------------------*/


int main() {

    const uint led_pin = 25;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // Initialize chosen serial port
    stdio_init_all();

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
