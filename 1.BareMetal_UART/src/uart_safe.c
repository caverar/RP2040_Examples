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
        // Initialize the tx_packages_array with zero.
        for(uint8_t j = 0; j < 8; j++){
            ((uint32_t*)(&(self->tx_packages_array[i].sample)))[j] = 0;
        } 
    }

    // Initialize the rx_package with zero.
    for(uint8_t j = 0; j < 8; j++){
        ((uint32_t*)(&(self->rx_package.sample)))[j] = 0;
    } 


    self->tx_handler_state = IDLE;
    bool tx_handler_send_data = false;


}

void UartSafe_init_uart(UartSafe* const self){

    bsp_uart_configure();
    bsp_dma_configure_uart_tx();
    bsp_dma_configure_uart_rx();

}



void UartSafe_start_TX(UartSafe* const self, uint16_t number_of_transfers){
    bsp_dma_start_uart_tx(self->pending_tx_package, number_of_transfers);
}

void UartSafe_start_RX(UartSafe* const self, uint16_t number_of_transfers){
    bsp_dma_start_uart_rx(&(self->rx_package), number_of_transfers);
}




void UartSafe_package_scheduler(UartSafe* const self){

    int x = 5;
}

char CRCdata[2] = {'r', '\n'};




void UartSafe_tx_handler(UartSafe* const self){

    switch (self->tx_handler_state){
        case IDLE:
            // In this state, the handler see if there is a request to send a
            // package.
            if(self->tx_handler_send_data){

                self->tx_handler_send_data = false;
                bsp_dma_start_uart_tx(self->pending_tx_package, 30);
                self->tx_handler_state = WAITING_FOR_DMA;
            }
            break;
        case WAITING_FOR_DMA:
            // In this state, the handler waits for the DMA trasfer, to finish,
            // and then, sends the CRC16 data. 
            if((!bsp_uart_tx_is_busy()) && (!bsp_dma_is_busy_uart_tx())){
                //CRCdata = {'r', '\n'};
                bsp_dma_disable_uart_tx();
                bsp_uart_send_buffer(CRCdata, 2);
                //bsp_dma_start_uart_tx(&CRCdata, 2);
                self->tx_handler_state = WAITING_FOR_LAST_TRANFER;
            }
            break;
        case WAITING_FOR_LAST_TRANFER:
            if(!bsp_uart_tx_is_busy()){
                self->tx_handler_state = IDLE;
            }
            break;
        
        default:
            self->tx_handler_send_data = 0;
            self->tx_handler_state = IDLE;
            break;
    }
}

void UartSafe_rx_handler(UartSafe* const self){

    //change bsp_uart_rx_is_busy to the trasfer data on the dma channel
    
    switch (self->rx_handler_state){
        case IDLE:
            // In this state, the handler is waiting for a new transfer
            if(bsp_uart_rx_is_busy()){
                // there is a new trasfer detected
                self->tx_handler_state = WAITING_FOR_DMA;
            }
            break;
        case WAITING_FOR_DMA:
            if(bsp_uart_rx_is_busy()){

            }

            break;
        case CRC_VERIFICATION:
            break;
        default:
            self->tx_handler_send_data = 0;
            break;

    }
}


bool UartSafe_retreive_data_rq(UartSafe* const self, uint16_t *sempahore,
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

    /*     for(uint8_t i = 0; i < LINKED_LIST_SIZE; i++){
        
        
        if(sample == self->data->tx_packages_array[i].sample){
            *sempahore +=  
            return true;
        }
    } */

    return false;
}

/************************ Camilo Vera **************************END OF FILE****/