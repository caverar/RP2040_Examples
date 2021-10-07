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
#include <hardware/dma.h>

// High level API
#include "pico/stdlib.h"

// FreeRTOS
//#include <FreeRTOS.h>
//#include "FreeRTOSConfig.h"
//#include "task.h"

// Project libraries
#include "bsp.h"


/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

uint32_t testvar = 0;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/



/* Code Separator: Description -----------------------------------------------*/
/*----------------------------------------------------------------------------*/

    /* Subcode Separator: Description ----------------------------------------*/
    /*------------------------------------------------------------------------*/

        /* SubSubcode Separator: Description ---------------------------------*/
        /*--------------------------------------------------------------------*/



    

/* Protocol defines ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

// arreglo para recepcion de datos, debe estar alineado si o si
__attribute__((aligned(32))) char buffer[32];


int main(){



    // Initialize System  
    // set_sys_clock_khz()

    // Initialize chosen serial port
    // stdio_init_all();
    
    
    // Initialize LED pin

    const uint32_t led_pin = 25;
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_set_outover(led_pin, GPIO_OVERRIDE_INVERT);


    // Rellenar el arreglo con "T"
    memset(buffer, ' ', sizeof(buffer));
    // for(uint8_t i = 0; i<32;i++){
    //     buffer2[i] = 30 + i;
    // }

    const uint32_t read_channel = 1;
    const uint32_t write_channel = 0;

    // UART
    bsp_uart_configure();
    bsp_dma_configure(read_channel, BSP_DMA_SIZE_8, 32, buffer, &uart0_hw->dr, 
                      DREQ_UART0_RX, BSP_DMA_PERIPHERAL_TO_MEM);
    bsp_dma_configure(write_channel, BSP_DMA_SIZE_8, 32, &uart0_hw->dr, buffer, 
                      DREQ_UART0_TX, BSP_DMA_MEM_TO_PERIPHERAL);





    bool state = true; // lectura
    bsp_dma_enable(read_channel);


    for (;;) {

        //sleep_ms(1000);



        
        if(state){      // lectura
            if(!dma_channel_is_busy(read_channel)){
                bsp_dma_update(write_channel, 32, &uart0_hw->dr, buffer);
                state = false;                 
            }
        }else{          // escritura
            if(!dma_channel_is_busy(write_channel)){
                bsp_dma_update(read_channel, 32, buffer, &uart0_hw->dr);
                state = true; 
            }
        }        
    }


    // Loop forever
    while(true){

        // Blink LED
        //printf("Blinking!\r\n");
        gpio_put(led_pin, true);
        sleep_ms(1000);
        gpio_put(led_pin, false);
        sleep_ms(1000);
    }

    while(true){
        tight_loop_contents();
    }
}


/************************ Camilo Vera **************************END OF FILE****/
