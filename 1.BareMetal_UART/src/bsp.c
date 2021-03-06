/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

// Standard C libraries
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Hardware API libraries

#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <hardware/dma.h>

/* UART ----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


void bsp_uart_configure(){
    // The SDK seems to configure sane values for baudrate, etc.
    // stdio_uart_init(); 
 
    // Set up our UART with a basic baud rate.
    uart_init(UART_ID, 2400);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Enable FIFO
    uart_set_fifo_enabled(UART_ID, true);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as 
    // close as possible to that requested
    uint32_t __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS
    uart_set_hw_flow(UART_ID, false, false);
    // Set  data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); 
}


void bsp_uart_send_buffer(const char* const buffer, uint8_t size){
    for(uint8_t i = 0; i < size; i++){
        UART_INSTANCE->dr = buffer[i];
    }

}

bool bsp_uart_tx_is_busy(void){
    return !(bool)((UART_INSTANCE->fr & 
                    UART_UARTFR_TXFE_BITS)>>UART_UARTFR_TXFE_LSB);


}

// bool bsp_uart_rx_is_busy(void){//
//     return false;
// }

/* DMA: UART------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/



void bsp_dma_configure_uart_tx(void){
    dma_configure(DMA_UART_TX_WRITE_CHANNEL, DMA_SIZE_8, 32,
                  DREQ_UART0_TX, DMA_MEM_TO_PERIPHERAL, false, false, true);
}
void bsp_dma_configure_uart_tx_organizer(void){
    dma_configure(DMA_UART_TX_WRITE_ORGANIZER_CHANNEL, DMA_SIZE_32, 32,
                  DREQ_UART0_RX, DMA_MEM_TO_MEM, false, true, false);
}


void bsp_dma_configure_uart_rx(void){
    dma_configure(DMA_UART_RX_READ_CHANNEL, DMA_SIZE_8, 32,
                  DREQ_UART0_RX, DMA_PERIPHERAL_TO_MEM, false, false, false);
}

void bsp_dma_configure_uart_rx_organizer(void){
    dma_configure(DMA_UART_RX_READ_ORGANIZER_CHANNEL, DMA_SIZE_32, 32,
                  DREQ_UART0_RX, DMA_MEM_TO_MEM, false, true, false);
}



void bsp_dma_start_uart_tx(const volatile void* source_address,
                           uint16_t number_of_transfers){
    dma_start(DMA_UART_TX_WRITE_CHANNEL, number_of_transfers, 
              &UART_INSTANCE->dr, source_address);
}

void bsp_dma_start_uart_tx_organizer(const volatile void* source_address,
                                     volatile void* destiny_address,
                                     uint16_t number_of_transfers){
    dma_start(DMA_UART_TX_WRITE_ORGANIZER_CHANNEL, number_of_transfers, 
              destiny_address, source_address);
}

void bsp_dma_start_uart_rx(volatile void* destiny_address,
                           uint16_t number_of_transfers){
    dma_start(DMA_UART_RX_READ_CHANNEL, number_of_transfers, 
              destiny_address, &UART_INSTANCE->dr);
}

void bsp_dma_start_uart_rx_organizer(const volatile void* source_address,
                                     volatile void* destiny_address,
                                     uint16_t number_of_transfers){
    dma_start(DMA_UART_RX_READ_ORGANIZER_CHANNEL, number_of_transfers, 
              destiny_address, source_address);
}



bool bsp_dma_is_busy_uart_rx(void){
    return dma_channel_is_busy(DMA_UART_RX_READ_CHANNEL);
}
bool bsp_dma_is_busy_uart_rx_organizer(void){
    return dma_channel_is_busy(DMA_UART_RX_READ_ORGANIZER_CHANNEL);    
}

bool bsp_dma_is_busy_uart_tx(void){
    return dma_channel_is_busy(DMA_UART_TX_WRITE_CHANNEL);
}
bool bsp_dma_is_busy_uart_tx_organizer(void){
    return dma_channel_is_busy(DMA_UART_TX_WRITE_ORGANIZER_CHANNEL);    
}


bool bsp_dma_transfer_complete_uart_rx(void){
    if(dma_channel_hw_addr(DMA_UART_RX_READ_CHANNEL)->transfer_count == 0){
        return true;
    }
    return false;
}

bool bsp_dma_transfer_complete_uart_rx_organizer(void){
    if(dma_channel_hw_addr(DMA_UART_RX_READ_ORGANIZER_CHANNEL)->transfer_count 
        == 0){
        return true;
    }
    return false;
}


void bsp_dma_disable_uart_rx(void){
    dma_channel_config config = dma_get_channel_config(
                                DMA_UART_RX_READ_CHANNEL);
    channel_config_set_enable(&config,false);
}
void bsp_dma_disable_uart_tx(void){
    dma_channel_config config = dma_get_channel_config(
                                DMA_UART_TX_WRITE_CHANNEL);
    channel_config_set_enable(&config,false);    
}

void dma_configure(uint8_t channel, enum dma_channel_transfer_size data_size,
                   uint8_t number_of_transfers, uint8_t data_request_signal, 
                   enum bsp_dma_data_direction data_direction, bool ring_mode,
                   bool byte_swaping, bool sniffer_enable){

    dma_channel_config config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, data_size);

    // Data direction:
    switch(data_direction){
        case DMA_MEM_TO_PERIPHERAL:
            channel_config_set_read_increment(&config, true);
            channel_config_set_write_increment(&config, false);
            channel_config_set_dreq(&config, data_request_signal);
            if(ring_mode){
                channel_config_set_ring(&config, false, 
                                        (uint32_t)log2f(number_of_transfers));
            }
            break;

        case DMA_PERIPHERAL_TO_MEM:
            channel_config_set_read_increment(&config, false);
            channel_config_set_write_increment(&config, true);
            channel_config_set_dreq(&config, data_request_signal);
            if(ring_mode){
                channel_config_set_ring(&config, false, 
                                        (uint32_t)log2f(number_of_transfers));
            }
            break;

        case DMA_MEM_TO_MEM:
            channel_config_set_read_increment(&config, true);
            channel_config_set_write_increment(&config, true);
            break;
        default:
            break;
    }
    // byte swaping
    channel_config_set_bswap(&config, byte_swaping);
    

    // Apply configuration to actual channel
    dma_channel_configure(
        channel,                // channel
        &config,                // config pointer
        0x00000000,             // write address
        0x00000000,             // read address
        number_of_transfers,    // number of transfers
        false                   // trigger (true to start transfer)
    );

    // dma sniffer: must be after dma_channel_configure because the 
    // configuration, dont use the "dma_channel_config" struct.


    if(sniffer_enable){
        dma_sniffer_enable(channel,0x2,true);   // Calculate a CRC-16-CCITT
    }

}

void dma_start(uint8_t channel, uint16_t number_of_transfers, 
                    volatile void* write_addr, 
                    const volatile void* read_addr){

    dma_channel_config config = dma_get_channel_config(channel);    
    dma_channel_configure(
        channel,                // channel
        &config,                // config pointer
        write_addr,             // write address
        read_addr,              // read address
        number_of_transfers,    // number of transfers
        true);                  // trigger (true to start transfer)
}

void dma_set_initial_dma_CRC16(uint32_t value){
    dma_hw->sniff_data = value;
}
uint16_t dma_get_CRC16(void){
    return (uint16_t)dma_hw->sniff_data;
}
/************************ Camilo Vera **************************END OF FILE****/