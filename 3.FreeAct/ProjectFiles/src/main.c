/** 
  ******************************************************************************
  * @file    main.c
  * @author  Camilo Vera
  * @brief   Main program.
  *          Description of the file functions, etc. 
  *
  @verbatim
  ==============================================================================
                     ##### Project Description #####
  ==============================================================================
  Example 02: FreeRTOS.
 
  This code is a test with FreeRTOS running on RP2040.
   
  MCU: RP2040 (Two Cortex M0+).
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
//#include "hardware/uart.h"
//#include "hardware/gpio.h"

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

// FreeAct
#include <FreeAct.h>

// Project libraries
//#include "bsp.h"
//#include "uart_safe.h"

void led_task(){
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(100);    //ticks
        gpio_put(LED_PIN, 0);
        vTaskDelay(100);
        //printf("Hola mundo \r\n");
    }
}


void echo(){

    int val = 0;
    while (true) {
        printf("Hola mundo %d\r\n",val++);
        vTaskDelay(100);
    }
}


int main() {
    stdio_init_all();
    xTaskCreate(led_task, "LED_TASK",256, NULL, 1, NULL);
    xTaskCreate(echo, "ECHO",256, NULL, 1, NULL);
    vTaskStartScheduler();
    while(true); 
    
}