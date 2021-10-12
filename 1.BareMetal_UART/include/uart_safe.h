/** 
  ******************************************************************************
  * @file    uart_safe.c
  * @author  Camilo Vera
  * @brief   Serial Protocol with CRC16 verification, and linked structs.
  * 
  *          This file contains the implementation of a serial protocol, 
  *          calling the HAL of the MCU, using UART with DMA, and linked 
  *          structures to send 32bytes CRC16-CCITT verified packages.
  * 
  *          Linked structures, are used to have a FIFO of packages, so in case 
  *          that the slave do not behave as RTOS system (Windows, Linux, etc), 
  *          there is a windows of opportunity to request past corrupted data,
  *          saved in the structure. 
  *
  @verbatim

  ==============================================================================
                     ##### UART SAFE CRC16 PROTOCOL #####
  ==============================================================================

  STRUCTS:

  FUNCTIONS:

  OBJECTS:
  @endverbatim
  ****************************************************************************** 
*/

#ifndef UART_SAFE_H
#define UART_SAFE_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Defines -------------------------------------------------------------------*/

#define LINKED_LIST_SIZE 6

#define tx_sampling_period_ms 50


struct package_struct;


typedef enum {
    IDLE,
    WAITING_FOR_DMA,
    WAITING_FOR_LAST_TRANFER
}tx_handler_state;

typedef enum {
    IDLE,
    WAITING_FOR_DMA,
    CRC_VERIFICATION
}rx_handler_state;

/* Object definitions and parameters -----------------------------------------*/

__attribute__((aligned(32))) typedef struct package_struct{
    uint32_t sample;                        // 0 - 1 bytes
    uint32_t sensor2;                       // 4 - 7 bytes
    uint32_t sensor3;                       // 8 -11 bytes
    uint32_t sensor4;                       // 12-15 bytes
    uint32_t sensor5;                       // 16-19 bytes
    uint32_t sensor6;                       // 20-23 bytes
    uint16_t rq_sample;                     // 26-27 bytes
    uint16_t control_signals;               // 28-29 bytes
    uint16_t CRC16;                         // 30-31 bytes
    struct package_struct* next_structure;  // address to the next structure
}package;



typedef struct UartSafe_data_struct{
    package tx_packages_array[LINKED_LIST_SIZE];
    package* pending_tx_package;
    package* current_sample_tx_package;
    package rx_package;
    
    // UartSafe_tx_handler data:
    tx_handler_state tx_handler_state;      // state of the handler.
    bool tx_handler_send_data;              // signal to init data transfer.
    rx_handler_state rx_handler_state;


}UartSafe;

void UartSafe_constructor(UartSafe* const self);
void UartSafe_init_uart(UartSafe* const self);

void UartSafe_start_TX(UartSafe* const self, uint16_t number_of_transfers);
void UartSafe_start_RX(UartSafe* const self, uint16_t number_of_transfers);


bool UartSafe_retreive_data_rq(UartSafe* const self, 
                               uint16_t *sempahore ,uint16_t sample);



/**
 * @brief This function has to be called continuously by a task to send data
 * over UART, including initial DMA transfer, plus CRC16 manual transfer, with
 * the data either from the DMA sniffer, CRC peripheral or manual calculation.
 * 
 * @param self: Reference to the UartSafe object
 */
void UartSafe_tx_handler(UartSafe* const self);


void UartSafe_package_scheduler(UartSafe* const self);



#ifdef __cplusplus
}
#endif

#endif /* UART_SAFE_H */

/************************ Camilo Vera **************************END OF FILE****/