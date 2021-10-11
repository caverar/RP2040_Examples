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


/* Includes ------------------------------------------------------------------*/

// Standard C libraries
#include <stdint.h>
#include <stdbool.h>

// Hardware API libraries
#include <hardware/dma.h>
#include "hardware/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UART ----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define UART_ID uart0
#define UART_INSTANCE uart0_hw
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
// Pins must be "uart0" usable
#define UART_TX_PIN 0
#define UART_RX_PIN 1


void bsp_uart_configure();

/**
 * @brief This function sends a buffer over UART by polling
 * 
 * @param buffer A pointer to the buffer
 * @param size the size of the buffer
 */
void bsp_uart_send_buffer(const char* const buffer, uint8_t size);

bool bsp_uart_tx_is_busy(void);


/* DMA: UART------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/




enum bsp_dma_data_direction{
    DMA_MEM_TO_PERIPHERAL = 0,   
    DMA_PERIPHERAL_TO_MEM = 1,
    DMA_MEM_TO_MEM = 2
};

#define DMA_UART_RX_READ_CHANNEL 1
#define DMA_UART_TX_WRITE_CHANNEL 0

void bsp_dma_configure_uart_tx(void);
void bsp_dma_configure_uart_rx(void);

bool bsp_dma_is_busy_uart_rx(void);
bool bsp_dma_is_busy_uart_tx(void);

void bsp_dma_start_uart_tx(const volatile void* source_address,
                           uint16_t number_of_transfers);
void bsp_dma_start_uart_rx(volatile void* destiny_address,
                           uint16_t number_of_transfers);

void dma_configure(uint8_t channel, enum dma_channel_transfer_size data_size,
                   uint8_t number_of_transfers, uint8_t data_request_signal, 
                   enum bsp_dma_data_direction data_direction);

void dma_start(uint8_t channel, uint16_t number_of_transfers, 
               volatile void* write_addr, const volatile void* read_addr);

//#define bsp_dma_channel_is_busy(channel) dma_channel_is_busy(channel)


#ifdef __cplusplus
}
#endif

#endif /* BSP_H */

/************************ Camilo Vera **************************END OF FILE****/