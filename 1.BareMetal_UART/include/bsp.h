/** 
  ******************************************************************************
  * @file    bsp.c
  * @author  Camilo Vera
  * @brief   Board support package.
  *          This file contains the HAL to control all board peripherals. 
  *
  @verbatim

  ==============================================================================
                     ##### BOARD SUPPORT PACKAGE #####
  ==============================================================================
  UART:

  DMA:

  @endverbatim
  ****************************************************************************** 
*/

#ifndef BSP_H
#define BSP_H


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

// Standard C libraries
#include <stdint.h>
#include <stdbool.h>


/* UART ----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
// Pins must be "uart0" usable
#define UART_TX_PIN 0
#define UART_RX_PIN 1



void bsp_uart_configure();


/* BSP -----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


enum bsp_dma_transfer_size {
    BSP_DMA_SIZE_8 = 0,    ///< Byte transfer (8 bits)
    BSP_DMA_SIZE_16 = 1,   ///< Half word transfer (16 bits)
    BSP_DMA_SIZE_32 = 2    ///< Word transfer (32 bits)
};

enum bsp_dma_data_direction {
    BSP_DMA_MEM_TO_PERIPHERAL = 0,   
    BSP_DMA_PERIPHERAL_TO_MEM = 1,
    BSP_DMA_MEM_TO_MEM = 2
};

void bsp_dma_configure(uint8_t channel, enum bsp_dma_transfer_size data_size,
                       uint16_t number_of_transfers, volatile void *write_addr,
                       const volatile void *read_addr, 
                       uint8_t data_request_signal,
                       enum bsp_dma_data_direction data_direction);

void bsp_dma_update(uint8_t channel, uint16_t number_of_transfers, 
                    volatile void *write_addr, const volatile void *read_addr);

void bsp_dma_enable(uint32_t channel);


#ifdef __cplusplus
    }
#endif

#endif /* FREERTOS_CONFIG_H */