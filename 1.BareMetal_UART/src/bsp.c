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

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); 

    // On my system there is one junk byte on boot
    //uart_getc(uart0);   
}


/* UART ----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/




void bsp_dma_configure(uint8_t channel, enum bsp_dma_transfer_size data_size,
                       uint16_t number_of_transfers, volatile void *write_addr,
                       const volatile void *read_addr, 
                       uint8_t data_request_signal,
                       enum bsp_dma_data_direction data_direction){

    dma_channel_config config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, data_size);

    // Data direction:
    switch(data_direction){
        case 0:     // BSP_DMA_MEM_TO_PERIPHERAL
            channel_config_set_read_increment(&config, true);
            channel_config_set_write_increment(&config, false);
            channel_config_set_ring(&config, false, 
                                    (uint32_t)log2f(number_of_transfers));

            break;
        case 1:     // BSP_DMA_PERIPHERAL_TO_MEM
            channel_config_set_read_increment(&config, false);
            channel_config_set_write_increment(&config, true);
            channel_config_set_ring(&config, true, 
                                    (uint32_t)log2f(number_of_transfers));
            break;
        case 2:     // BSP_DMA_MEM_TO_MEM
            channel_config_set_read_increment(&config, true);
            channel_config_set_write_increment(&config, true);
            break;
        default:
            break;
    }

    // Trigger selection
    channel_config_set_dreq(&config, data_request_signal);

    // Apply configuration to actual channel
    dma_channel_configure(
        channel,                // channel
        &config,                // config pointer
        write_addr,             // write address
        read_addr,              // read address
        number_of_transfers,    // number of transfers
        false);                 // trigger (true to start transfer)


}

void bsp_dma_update(uint8_t channel, uint16_t number_of_transfers, 
                    volatile void *write_addr, 
                    const volatile void *read_addr){

    dma_channel_config config = dma_get_channel_config(channel);
    
    dma_channel_configure(
        channel,                // channel
        &config,                // config pointer
        write_addr,             // write address
        read_addr,              // read address
        number_of_transfers,    // number of transfers
        true);                  // trigger (true to start transfer)

}

void bsp_dma_enable(uint32_t channel){
    dma_channel_start(channel);
}