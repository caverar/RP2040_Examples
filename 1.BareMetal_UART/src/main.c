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
void callback_func(void){
    test_var++;
}

int main(){

    
    /* Initialize System -----------------------------------------------------*/ 

    // set_sys_clock_khz()

    
    /* Initialize LED --------------------------------------------------------*/ 

    //const uint32_t led_pin = 25;
    //gpio_init(led_pin);
    //gpio_set_dir(led_pin, GPIO_OUT);
    //gpio_set_outover(led_pin, GPIO_OVERRIDE_INVERT);


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
    for (uint8_t i = 0; i < 5; i++){
        for (size_t j = 0; j < 32; j++){
            ((uint8_t*)(uint32_t*)(&objectTest.tx_packages_array[i]))[j] =
                array[j];
        }
        
    }

    //
    UartSafe_init_uart(&objectTest);

    objectTest.tx_handler_send_data = true;
    for(;;){
        UartSafe_tx_handler(&objectTest);
        UartSafe_rx_handler(&objectTest);
        if(objectTest.tx_handler_state==(tx_handler_state)TX_IDLE){
            objectTest.tx_handler_send_data = true;

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