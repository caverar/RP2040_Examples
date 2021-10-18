/* Includes ------------------------------------------------------------------*/

#include "uart_safe.h"

// Standard C libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include <math.h>

// Personal Standard libraries

#include "bitwise.h"
// High level API
//#include "pico/stdlib.h"

// Hardware API libraries
//#include "hardware/dma.h"

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

    // Initialize the rx_package, rx_raw_buffer and tx_error_package with zero.
    for(uint8_t j = 0; j < 8; j++){
        ((uint32_t*)(&(self->rx_package)))[j] = 0;
        ((uint32_t*)(&(self->rx_raw_buffer)))[j] = 0;
        ((uint32_t*)(&(self->tx_error_package)))[j] = 0;
    } 

    // Initialize callbacks array
    for(uint8_t j = 0; j < 13; j++){
        self->function_callbacks[j] = (UartSafe_signal_callback)void_call_back;
    } 

    // Initial states
    self->tx_handler_state = TX_IDLE;
    bool tx_handler_send_data = false;
    self->rx_handler_state = RX_IDLE;
    self->tx_semaphore = 0;
    self->pending_tx_package_position = 0;
    self->retreive_data_rq = 0;
    self->current_sample_tx_package_position = 0;

    // Error Package

    self->tx_error_package.control_signals = 0x0000 | retreive_data_ack_bit;
}

void void_call_back(UartSafe* self){

}
void UartSafe_init_uart(UartSafe* const self){

    bsp_uart_configure();
    bsp_dma_configure_uart_tx();
    bsp_dma_configure_uart_tx_organizer();
    bsp_dma_configure_uart_rx();
    bsp_dma_configure_uart_rx_organizer();
    // start receiving data:
    bsp_dma_start_uart_rx(&self->rx_raw_buffer, 32);
}



void UartSafe_start_TX(UartSafe* const self, uint16_t number_of_transfers){
    bsp_dma_start_uart_tx(self->pending_tx_package, number_of_transfers);
}

void UartSafe_start_RX(UartSafe* const self, uint16_t number_of_transfers){
    bsp_dma_start_uart_rx(&(self->rx_package), number_of_transfers);
}



bool UartSafe_new_sample(UartSafe* const self, package* sample){


    // TODO: Take in account scheduler, maybe a request signal
    self->current_sample_tx_package = self->current_sample_tx_package->
                                  next_structure;
    memcpy((void*)self->current_sample_tx_package,(void*)sample, 32);
    //*self->current_sample_tx_package = *sample;

    self->tx_semaphore++;
    if(self->current_sample_tx_package_position<(LINKED_LIST_SIZE-1)){
        self->current_sample_tx_package_position++;
    }else{
        self->current_sample_tx_package_position = 0;
    }

    return true;
}

/**
 * @brief Calculates the number of jumps in the linked list, betwen to packges,
 * through its sample ID.
 * 
 * @param lead_pointer_ID ID of the leading pointer.
 * @param behind_pointer_ID If of the pointer behind.
 * @return uint32_t The numbers of jumps between the elements.
 */
static uint32_t jumps_between_pointers(uint32_t front_pointer_ID, 
                                       uint32_t rear_pointer_ID){
    if(front_pointer_ID < rear_pointer_ID){
        return (1 + front_pointer_ID + (UINT32_MAX -rear_pointer_ID));
    }else{
        return rear_pointer_ID - front_pointer_ID;
    }
}

static uint32_t requested_sample_id;
static uint32_t current_tx_sample_id;
static uint32_t pending_tx_sample_id;
static uint32_t jumps_between_pending_to_current;
static uint32_t jumps_between_requested_to_current;
static int32_t array_position;

static bool pending_tx_behind_current_sample(UartSafe* const self){
    if(self->pending_tx_package_position == 
      self->current_sample_tx_package_position){
        return false;
    }else{
        return true;
    }
}

void UartSafe_package_scheduler(UartSafe* const self){

    // Se encarga de evaluar los semaforos

    // Debe tener un mecanismo para que pending_tx_package, no revase a 
    // current_sample_tx_package, siempre d0ebe estar al menos, un salto atras


    switch(self->package_scheduler_state){

        case SCHEDULER_IDLE:
            if(self->retreive_data_rq){

                //Save data in error package
                self->tx_error_package.rq_sample = self->rx_package.rq_sample;

                self->package_scheduler_state = RETREIVE_DATA;

            }else if(self->tx_semaphore>0){
                //TODO
                self->package_scheduler_state = SCHEDULE_TX;
            }
            break;

        case RETREIVE_DATA:
            requested_sample_id = self->tx_error_package.rq_sample;
            current_tx_sample_id =self->current_sample_tx_package->sample;
            pending_tx_sample_id = self->pending_tx_package->sample;


            // jumps between pointers by ID:
            jumps_between_pending_to_current = jumps_between_pointers(
                                                current_tx_sample_id, 
                                                pending_tx_sample_id);

            jumps_between_requested_to_current = jumps_between_pointers(
                                                 requested_sample_id, 
                                                 pending_tx_sample_id);

            //TODO: Testing (Slow debugging)
            if((jumps_between_requested_to_current > 
               jumps_between_pending_to_current) &&
               (jumps_between_requested_to_current < LINKED_LIST_SIZE)){
                
                // array position calculation:
                array_position = ((int32_t)self->pending_tx_package_position)-
                                  jumps_between_requested_to_current;
                if(array_position<0){
                    array_position += LINKED_LIST_SIZE - 1;
                }

                // Package loading:

                self->pending_tx_package = &(self->tx_packages_array[
                                             array_position]);
                *(self->pending_tx_package) = self->tx_error_package;

                // tx_semaphore_rq: 
                self->tx_semaphore++;

            }else{

                // array position calculation:
                array_position = ((int32_t)self->pending_tx_package_position)-1;
                if(array_position<0){
                    array_position += LINKED_LIST_SIZE - 1;
                }
                
                // Package loading:
                self->pending_tx_package = &(self->tx_packages_array[self->
                                           pending_tx_package_position-1]);

                *(self->pending_tx_package) = self->tx_error_package;
                
                // tx_semaphore_rq: 
                self->tx_semaphore++;
            }

            self->package_scheduler_state = SCHEDULER_IDLE;
            break; 

        case SCHEDULE_TX:
            //TODO:request
            if(self->tx_semaphore > 0 && 
               pending_tx_behind_current_sample(self)){

                self->tx_handler_send_data = true;
                self->tx_semaphore--;
            }

            self->package_scheduler_state = SCHEDULER_IDLE;
            break;

        default:
            self->tx_semaphore = 0;
            self->package_scheduler_state = SCHEDULER_IDLE;
            break;

    }
}




static uint8_t CRCdata[3] = {'r', '\n', '\0'};

void UartSafe_tx_handler(UartSafe* const self){

    switch (self->tx_handler_state){
        case TX_IDLE:
            // In this state, the handler see if there is a request to send a
            // package, if its the case, a dma transfer will begin, to organize
            // data taking accont on endianess-
            if(self->tx_handler_send_data){
                self->tx_handler_send_data = false;
                bsp_dma_start_uart_tx_organizer(self->pending_tx_package,
                                                &self->tx_raw_buffer, 7);

                self->tx_raw_buffer.control_signals = (
                    (FIRST_BYTE(self->pending_tx_package->control_signals) << 8)
                    | SECOND_BYTE(self->pending_tx_package->control_signals)
                );
                self->tx_raw_buffer.CRC16 = (
                    (FIRST_BYTE(self->pending_tx_package->CRC16) << 8) |
                     SECOND_BYTE(self->pending_tx_package->CRC16)
                );
                
                self->tx_handler_state = TX_DATA_ORGANIZATION;    
            }
            break;

        case TX_DATA_ORGANIZATION:
            if(!bsp_dma_is_busy_uart_tx_organizer()){
                dma_set_initial_dma_CRC16(0);
                bsp_dma_start_uart_tx(&self->tx_raw_buffer, 30);
                self->tx_handler_state = CRC16_APPEND;
            }
            break;

        case CRC16_APPEND:
            // In this state, the handler waits for the DMA trasfer, to finish,
            // and then, sends the CRC16 data. 
            if(!bsp_uart_tx_is_busy() && !bsp_dma_is_busy_uart_tx()){

                uint8_t CRC = dma_get_CRC16();
                CRCdata[0] = SECOND_BYTE(CRC);
                CRCdata[1] = FIRST_BYTE(CRC);
                bsp_uart_send_buffer(CRCdata, 3);
                self->tx_handler_state = WAITING_FOR_LAST_TRANFER;
            }
            break;

        case WAITING_FOR_LAST_TRANFER:
            if(!bsp_uart_tx_is_busy()){
                self->tx_handler_state = TX_IDLE;

                // pasar a la siguiente muestra:

                if(self->pending_tx_package_position<(LINKED_LIST_SIZE-1)){
                    self->pending_tx_package_position++;
                }else{
                    self->pending_tx_package_position = 0;
                }
                self->pending_tx_package = self->pending_tx_package->
                                           next_structure;
            }
            break;
        
        default:
            self->tx_handler_send_data = 0;
            self->tx_handler_state = TX_IDLE;
            break;
    }
}

volatile uint8_t test_var = 0;
void UartSafe_rx_handler(UartSafe* const self){

    //change bsp_uart_rx_is_busy to the trasfer data on the dma channel
    
    switch (self->rx_handler_state){
        case RX_IDLE:
            // In this state, the handler is waiting for a new transfer to be
            // completed
            if(bsp_dma_transfer_complete_uart_rx()){
                bsp_dma_start_uart_rx_organizer(&self->rx_raw_buffer, 
                                                &self->rx_package, 7);
                self->rx_package.control_signals = (
                    (FIRST_BYTE(self->rx_raw_buffer.control_signals) << 8) |
                     SECOND_BYTE(self->rx_raw_buffer.control_signals)
                );
                self->rx_package.CRC16 = (
                    (FIRST_BYTE(self->rx_raw_buffer.CRC16) << 8) |
                     SECOND_BYTE(self->rx_raw_buffer.CRC16)
                );
                
                self->rx_handler_state = RX_DATA_ORGANIZATION;
            }
            break;
        case RX_DATA_ORGANIZATION:
            if(bsp_dma_transfer_complete_uart_rx_organizer()){
                self->rx_handler_state = CRC_VERIFICATION;
            }

            break;
        case CRC_VERIFICATION:

            // TODO: Implement verification of packages, and rq messages on MCU.
            // Verification is only applied to sensors data, from MCU to Host.
            self->rx_handler_state = HOST_SAMPLE_REQUEST_VERIFICATION;
            break;

        case HOST_SAMPLE_REQUEST_VERIFICATION:
            // Message review, to see if there is a CRC fail in the host system,
            // so new data has to be sended 
            
            if(MASK_DATA(self->rx_package.control_signals, 
                         retreive_data_rq_bit) != 0){
                
                self->retreive_data_rq = true;
                bsp_dma_start_uart_rx(&self->rx_raw_buffer, 32);
                self->rx_handler_state = RX_IDLE;
            }else{
                self->rx_handler_state = CONTROL_SIGNALS_DECODING;
            }
            break;

        case CONTROL_SIGNALS_DECODING:
            // In this section, the control signals from the host are read to 
            // trigger its callbacks
            for(uint8_t i = 0; i < 13; i++){

                if(MASK_DATA(self->rx_package.control_signals, 0x0001<<i)){
                //if(GET_BIT(self->rx_package.control_signals, i)){
                    (self->function_callbacks[i])(self);
                }
            }
            bsp_dma_start_uart_rx(&self->rx_raw_buffer, 32);
            self->rx_handler_state = RX_IDLE;
            break;

        default:
            self->rx_handler_state = RX_IDLE;
            break;

    }
}



/************************ Camilo Vera **************************END OF FILE****/