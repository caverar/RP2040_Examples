/* Includes ------------------------------------------------------------------*/

#include "uart_safe.h"

// Standard C libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <stdio.h>

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

    // Initialize the rx_package and rx_raw_buffer with zero.
    for(uint8_t j = 0; j < 8; j++){
        ((uint32_t*)(&(self->rx_package)))[j] = 0;
        ((uint32_t*)(&(self->rx_raw_buffer)))[j] = 0;
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



void UartSafe_new_sample(UartSafe* const self, package* sample){
    *self->current_sample_tx_package = *sample;
    self->current_sample_tx_package = self->current_sample_tx_package->
                                      next_structure;
    self->tx_semaphore++;
}

void UartSafe_package_scheduler(UartSafe* const self){

    // Sen encarga de evaluar los semaforos

    // Debe tener un mecanismo para que pending_tx_package, no revase a 
    // current_sample_tx_package.



    if(self->tx_semaphore > 0){
        self->tx_handler_send_data = true;
    }

}
void UartSafe_retreive_data_rq(UartSafe* const self){


    // 1. Identificar el ID de la muestra solicitada
    // 2. Obtener el ID de la muestra actual
    // 3. Ejecutar aritmetica que tenga en cuenta el overload, para calcular la 
    //    diferencia de muestras, entre ambos, y el ID teorico de la muestra
    // 4. Si esta fuera de rango, asignar un paquete de muestra corrupta no 
    //    recuperable al paquete anterior a current_sample_tx_package, asignar
    //    current_sample_tx_package a dicho paquete, y aumentar en 1, el 
    //    semaforo.
    // 5. Si esta dentro del rango, ir directamente a la muestra requerida, 
    //    retrasar current_sample_tx_package a dicha muestra, y aumentar el 
    //    semaforo, en la cantidad requerida 

    // uint8_t counter = LINKED_LIST_SIZE;
    // bool flag = 1; 
    // package* pointer = self->current_sample_tx_package;

    // while(counter > 0 && flag){
    //     if(pointer->sample == 0){
    //         flag = false;
    //         self->pending_tx_package = pointer;
    //         //&sempahore += counter;
    //         return true;
    //     }else{
    //         pointer = pointer->next_structure;
    //         counter -= 1; 
    //     }

    // }

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
                //CRCdata = {'r', '\n'};
                //bsp_dma_disable_uart_tx();
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
                // pasar a la siguiente muestra
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

            // TODO:
            // Verification is only applied to sensors data, from MCU to Host.
            self->rx_handler_state = HOST_SAMPLE_REQUEST_VERIFICATION;
            break;

        case HOST_SAMPLE_REQUEST_VERIFICATION:
            // Message review, to see if there is a CRC fail in the host system,
            // so new data has to be sended 
            
            if(MASK_DATA(self->rx_package.control_signals, 
                         retreive_data_rq) != 0){
                
                UartSafe_retreive_data_rq(self);
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