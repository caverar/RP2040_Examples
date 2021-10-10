/** 
  ******************************************************************************
  * @file    uart_safe.c
  * @author  Camilo Vera
  * @brief   Serial Protocol with CRC16 verification, and linked structs.
  *          This file contains the implementation of a serial protocol, calling
  *          the HAL of the MCU, using UART with DMA, and linked structures to
  *          send 32bytes, CRC16-CCITT verified, packages.
  *          Linked structures, are used to have a buffer of packages, so there 
  *          is posible that the slave do not behave as RTOS system, so there is
  *          a windows to request past corrupted data. 
  *
  @verbatim

  ==============================================================================
                     ##### UART SAFE CRC16 PROTOCOL #####
  ==============================================================================
  Only one instance of the object is allowed, this is to use global 
  allocation of the data object, and avoid using the heap with malloc.
  STRUCTS:

  FUNCTIONS:

  OBJECTS:
  @endverbatim
  ****************************************************************************** 
*/


#ifndef UART_SAFE_H
#define UART_SAFE_H


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/



#include <stdint.h>
#include <stdbool.h>


#define LINKED_LIST_SIZE 6 
#define READ_CHANNEL 1
#define WRITE_CHANNEL 0


struct package_struct;
struct UartSafe_data_struct;
struct UartSafe_struct;


__attribute__((aligned(32))) typedef struct package_struct{
    uint32_t sample;                        // 0 - 1 bytes
    uint32_t sensor2;                       // 4 - 7 bytes
    uint32_t sensor3;                       // 8 -11 bytes
    uint32_t sensor4;                       // 12-15 bytes
    uint32_t sensor5;                       // 16-19 bytes
    uint32_t sensor6;                       // 20-23 bytes
    uint32_t rq_sample;                     // 26-27 bytes
    uint16_t control_signals;               // 28-29 bytes
    uint16_t CRC16;                         // 30-31 bytes
    struct package_struct* next_structure;  // address to the next structure 

}package;





typedef struct UartSafe_data_struct{
    package tx_packages_array[LINKED_LIST_SIZE];
    package* pending_tx_package;
    package* current_sample_tx_package;
    package rx_package;



}UartSafe;





void UartSafe_constructor(UartSafe* const self);
void UartSafe_init_uart(UartSafe* const self);


void UartSafe_enable_TX(UartSafe* const self);
void UartSafe_enable_RX(UartSafe* const self);

void UartSafe_reconfigure_TX(UartSafe* const self);
void UartSafe_reConfigure_RX(UartSafe* const self);

bool UartSafe_retreive_data_rq(UartSafe* const self, 
                               uint16_t *sempahore ,uint16_t sample);


#ifdef __cplusplus
    }
#endif

#endif /* UART_SAFE_H */