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
  Example 01: BareMetal_UART.
 
  This code is a prove of dma uart usage, with a comunication protocol with 
  CRC16 Algorithm.
   
  MCU: RP2040 (Two Cortex M0+).

  @endverbatim
  ****************************************************************************** 
*/

/* Includes ------------------------------------------------------------------*/

// Standard C libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <math.h>

// Hardware API libraries

//#include "hardware/uart.h"
//#include "hardware/gpio.h"
#include "hardware/dma.h"

// High level API
#include "pico/stdlib.h"

// FreeRTOS
//#include <FreeRTOS.h>
//#include "FreeRTOSConfig.h"
//#include "task.h"

// Project libraries
#include "bsp.h"
#include "uart_safe.h"


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

    



UartSafe objectTest;
extern volatile uint8_t test_var;
void callback_func(UartSafe* self){
    test_var++;
}

int main(){


    /* Initialize System -----------------------------------------------------*/ 

    // set_sys_clock_khz()


    /* Initialize LED --------------------------------------------------------*/ 

    const uint32_t led_pin = 25;
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_set_outover(led_pin, GPIO_OVERRIDE_INVERT);


    /* Application code ------------------------------------------------------*/ 

    // UartSafe Object for robust comunication
    UartSafe_constructor(&objectTest);
    objectTest.function_callbacks[0] = callback_func;
    //UartSafe_init_uart(&objectTest);

    // This signals tell how many transfers had to be done by the DMA 
    // uint8_t UartSafe_tx_semaphore = 0;
    // uint8_t UartSafe_rx_semaphore = 0;

    // UartSafe_rx_semaphore = 1;
    // UartSafe_tx_semaphore = 1;

    //bool state = true; // lectura

    char array[32]="Esta es una prueba de funcionami";
    uint32_t charTw[8];
    charTw[0] = ('E' << 24) + ('s' << 16)+ ('t' << 8)+ ('a' << 0);
    charTw[1] = (' ' << 24) + ('e' << 16)+ ('s' << 8)+ (' ' << 0);
    charTw[2] = ('u' << 24) + ('n' << 16)+ ('a' << 8)+ (' ' << 0);
    charTw[3] = ('p' << 24) + ('r' << 16)+ ('u' << 8)+ ('e' << 0);
    charTw[4] = ('b' << 24) + ('a' << 16)+ (' ' << 8)+ ('d' << 0);
    charTw[5] = ('e' << 24) + (' ' << 16)+ ('f' << 8)+ ('u' << 0);
    charTw[6] = ('n' << 24) + ('c' << 16)+ ('i' << 8)+ ('o' << 0);

    uint16_t charTw1 = ('n' << 8) + ('a' << 0);
    uint16_t charTw2 = ('m' << 8) + ('i' << 0);
    
    for (uint8_t i = 0; i < 5; i++){
        for (size_t j = 0; j < 8; j++){

            ((uint32_t*)(&objectTest.tx_packages_array[i]))[j] = charTw[j];
        }
        ((uint16_t*)(uint32_t*)(&objectTest.tx_packages_array[i]))[14] = 
            charTw1;
        ((uint16_t*)(uint32_t*)(&objectTest.tx_packages_array[i]))[15] = 
            charTw2;
        
    }


    //
    UartSafe_init_uart(&objectTest);
    uint64_t  current_time = 0;
    uint64_t  last_time =  time_us_64();


    //objectTest.tx_handler_send_data = true;

    package test_package;
    test_package.sample = 0;
    test_package.sensor1 = 0;
    test_package.sensor2 = 0;
    test_package.sensor3 = 0;
    test_package.sensor4 = 0;
    test_package.sensor5 = 0;
    test_package.rq_sample = 0;
    test_package.control_signals = 0; 
    //UartSafe_new_sample(&objectTest, &test_package);
    for(;;){

        UartSafe_rx_handler(&objectTest);
        UartSafe_package_scheduler(&objectTest);
        UartSafe_tx_handler(&objectTest);

        current_time = time_us_64();

        if((current_time- last_time) > (100*1000)){        // Mensaje cada 100ms
            //objectTest.tx_handler_send_data = true;
            
            last_time =  time_us_64();

            UartSafe_new_sample(&objectTest, &test_package);
            test_package.sensor1++;
            test_package.sample++;
            gpio_set_mask(1<<led_pin);
        }
    }
    
/*     for(;;){
        if(UartSafe_tx_semaphore > 0 && 
            !bsp_dma_channel_is_busy(DMA_UART_TX_WRITE_CHANNEL) &&
            !bsp_uart_tx_is_busy()){


            UartSafe_start_TX(&objectTest, 32); 
        }
        if(UartSafe_rx_semaphore > 0 && 
            !bsp_dma_channel_is_busy(DMA_UART_RX_READ_CHANNEL)){

            
            UartSafe_start_RX(&objectTest, 32);
            //UartSafe_tx_handler(&objectTest);
        }
    } */



/* 
    for (;;){
        gpio_put(led_pin, true);
        sleep_ms(1000);
    }
 */
    for (;;){
        tight_loop_contents();
    }
}

/************************ Camilo Vera **************************END OF FILE****/