/* Includes ------------------------------------------------------------------*/

#include "uart_safe.h"

// Standard C libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <stdio.h>

#include <math.h>

// High level API
#include "pico/stdlib.h"



// Hardware API libraries
#include "hardware/dma.h"

// Project libraries
#include "bsp.h"


void UartSafe_constructor(UartSafe* const self){




    
   

    for(uint8_t i = 0; i < LINKED_LIST_SIZE; i++){
        if(i < LINKED_LIST_SIZE - 1){
            self->tx_packages_array[i].next_structure = 
                &(self->tx_packages_array[i+1]);
        }else{
            self->tx_packages_array[LINKED_LIST_SIZE-1].next_structure = 
                &(self->tx_packages_array[0]);
            
            self->pending_tx_package = &(self->tx_packages_array[0]);
            self->current_sample_tx_package = &(self->tx_packages_array[0]);

        }
        // Initialize the structure with zero
        for(uint8_t j = 0; j < 8; j++){
            ((uint32_t*)(&(self->tx_packages_array[i].sample)))[j] = 0;
        } 
    }

    // Initialize the structure with zero
    for(uint8_t j = 0; j < 8; j++){
        ((uint32_t*)(&(self->rx_package.sample)))[j] = 0;

    } 

    //stdio_uart_init(); 
}

void UartSafe_init_uart(UartSafe* const self){

    bsp_uart_configure();

    

    bsp_dma_configure(READ_CHANNEL, BSP_DMA_SIZE_8, 
                      32, &(self->rx_package), &uart0_hw->dr, 
                      DREQ_UART0_RX, BSP_DMA_PERIPHERAL_TO_MEM);

    bsp_dma_configure(WRITE_CHANNEL, BSP_DMA_SIZE_8, 
                      32, &uart0_hw->dr, self->pending_tx_package, 
                      DREQ_UART0_TX, BSP_DMA_MEM_TO_PERIPHERAL);
}


void UartSafe_enable_TX(UartSafe* const self){
    bsp_dma_enable(WRITE_CHANNEL);
}

void UartSafe_enable_RX(UartSafe* const self){
    bsp_dma_enable(READ_CHANNEL);    
}

void UartSafe_reconfigure_TX(UartSafe* const self){
    bsp_dma_update(WRITE_CHANNEL, 32, &uart0_hw->dr, self->pending_tx_package);
}

void UartSafe_reConfigure_RX(UartSafe* const self){
    bsp_dma_update(READ_CHANNEL, 32, &(self->rx_package), &uart0_hw->dr);

}

bool UartSafe_retreive_data_rq(UartSafe* const self, uint16_t *sempahore ,
                               uint16_t sample){

    // This function request a new tranfer from the slave, due to CRC error,
    // and return a bool if is posible to retreive that value from the linked
    // list. if True, the pointer to the writeable structure, will be changed, 

    uint8_t counter = LINKED_LIST_SIZE;
    bool flag = 1; 
    package* pointer = self->current_sample_tx_package;
    while(counter > 0 && flag){
        if(pointer->sample == sample){
            flag = false;
            self->pending_tx_package = pointer;
            //&sempahore += counter;
            return true;
        }else{
            pointer = pointer->next_structure;
            counter -= 1; 
        }

    }

    //for(uint8_t i = 0; i < LINKED_LIST_SIZE; i++){
        
        
        //if(sample == self->data->tx_packages_array[i].sample){
        //    *sempahore +=  
        //    return true;
        //}
    //}
    return false;
    

}