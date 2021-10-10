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
    UartSafe_init_uart(&objectTest);

    // This signals tell how many transfers had to be done by the DMA 
    uint8_t UartSafe_tx_semaphore = 0;
    uint8_t UartSafe_rx_semaphore = 0;

    UartSafe_rx_semaphore = 1;

    bool state = true; // lectura
    UartSafe_enable_RX(&objectTest);

    

    for(;;){
        if(UartSafe_tx_semaphore > 0 && !dma_channel_is_busy(WRITE_CHANNEL)){


            UartSafe_reconfigure_TX(&objectTest); 
        }
        if(UartSafe_rx_semaphore > 0 && !dma_channel_is_busy(READ_CHANNEL)){

            
            UartSafe_reConfigure_RX(&objectTest);
        }
    }


    // for (;;){


        
    //     if(state){      // lectura
    //         if(!dma_channel_is_busy(READ_CHANNEL)){
                                

    //             for(int i = 0; i<8; i++){
    //                 ((uint32_t*)(&(objectTest.data->current_tx_package->sample)))[i] = ((uint32_t*)(&(objectTest.data->rx_package.sample)))[i]; 
    //             }

    //             state = false; 
    //             objectTest.UartSafe_reconfigure_TX(&objectTest);                
    //         }
    //     }else{          // escritura
    //         if(!dma_channel_is_busy(WRITE_CHANNEL)){

    //             objectTest.UartSafe_reConfigure_RX(&objectTest);
    //             state = true; 
    //         }
    //     }        
    // }


    for (;;){

        // Blink LED
        gpio_put(led_pin, true);
        sleep_ms(1000);
        gpio_put(led_pin, false);
        sleep_ms(1000);
    }

    for (;;){
        tight_loop_contents();
    }
}


/************************ Camilo Vera **************************END OF FILE****/
