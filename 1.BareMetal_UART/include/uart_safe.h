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
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Defines -------------------------------------------------------------------*/

#define LINKED_LIST_SIZE 6

#define tx_sampling_period_ms 50


struct package_struct;
struct UartSafe_data_struct;

//volatile uint8_t test_var;
typedef enum {
    TX_IDLE,
    TX_DATA_ORGANIZATION,
    CRC16_APPEND,
    WAITING_FOR_LAST_TRANFER
}tx_handler_state;

typedef enum {
    RX_IDLE,
    RX_DATA_ORGANIZATION,
    CRC_VERIFICATION,
    HOST_SAMPLE_REQUEST_VERIFICATION,
    CONTROL_SIGNALS_DECODING
}rx_handler_state;

typedef enum {
    SCHEDULER_IDLE,
    RETREIVE_DATA,
    SCHEDULE_TX


}package_scheduler_state;

/* Object definitions and parameters -----------------------------------------*/

__attribute__((aligned(32))) typedef struct package_struct{
    uint32_t sample;                        // 0 - 3 bytes
    uint32_t sensor2;                       // 4 - 7 bytes
    uint32_t sensor3;                       // 8 -11 bytes
    uint32_t sensor4;                       // 12-15 bytes
    uint32_t sensor5;                       // 16-19 bytes
    uint32_t sensor6;                       // 20-23 bytes
    uint32_t rq_sample;                     // 24-27 bytes
    uint16_t control_signals;               // 28-29 bytes
    uint16_t CRC16;                         // 30-31 bytes
    // 0x00: \Null

    // At the end of the package, the library must send 0x00 as null character.
    // The MCU uses timing to synchronize, so the user is must set baud rate,
    // in such a way that there is time between sample to synchronize data, and
    // attend request of corrupted data.

    struct package_struct* next_structure;  // address to the next structure
}package;

// Note: the host code must keep in mind the endianess of the MCU. In little 
// endian, the host must swap bytes for each datatype. This aplies 
//
// control_signals_masks:
// library signals:
#define retreive_data_rq_bit        (1 << 8)    // Host->MCU 15
#define retreive_data_ack_bit       (1 << 14)   // MCU->HOST
#define retreive_data_state_bit     (1 << 13)   // MCU->HOST, 0:data lose, 1: ok
// user signals:


// signals callbacks
typedef void (*UartSafe_signal_callback)(struct UartSafe_data_struct*); 



typedef struct UartSafe_data_struct{
    package tx_packages_array[LINKED_LIST_SIZE];
    package tx_raw_buffer;

    package* pending_tx_package;
    uint32_t pending_tx_package_position;
    package* current_sample_tx_package;


    package rx_package;                         // organized package
    package rx_raw_buffer;                      // desorganized data

    // UartSafe_tx_handler data:
    tx_handler_state tx_handler_state;          // State of the handler.
    bool tx_handler_send_data;                  // Init data transfer.

    // UartSafe_rx_handler data:
    rx_handler_state rx_handler_state;

    // Callbacks, control signals.
    UartSafe_signal_callback function_callbacks[13]; 

    // Tc Scheduler
    uint16_t tx_semaphore;
    bool retreive_data_rq;
    package_scheduler_state package_scheduler_state;

}UartSafe;

void UartSafe_constructor(UartSafe* const self);
void UartSafe_init_uart(UartSafe* const self);

void UartSafe_start_TX(UartSafe* const self, uint16_t number_of_transfers);
void UartSafe_start_RX(UartSafe* const self, uint16_t number_of_transfers);

void void_call_back(UartSafe* self);


/**
 * @brief This function has to be called continuously by a task to send data
 * over UART, including initial DMA transfer, plus CRC16 manual transfer, with
 * the data either from the DMA sniffer, CRC peripheral or manual calculation.
 *
 * @param self: Reference to the UartSafe object
 */
void UartSafe_tx_handler(UartSafe* const self);

/**
 * @brief This function has to be called continuously by a task to receive over 
 * UART. handles error detection, and callbacks triggering by the specified 
 * control signal.
 *
 * @param self: Reference to the UartSafe object
 */
void UartSafe_rx_handler(UartSafe* const self);

/**
 * @brief This function handle the request from the host to send old samples
 * due to data corruption detected by CRC error. The function calculates if is
 * posible to resend the data, changing the current_sample_tx_package pointer, 
 * and triggering multiples transfers, to reach the curret sample.
 *
 * If there is the case that is not posible to resend the data, a package with 
 * the bit "retreive_data_state" equal to zero, in the "control signals" , is 
 * going to be sent to the host, and then the current_sample_tx_package pointer 
 * will reach the current sample.
 *
 * @param self: Reference to the UartSafe object
 */
void UartSafe_retreive_data_rq(UartSafe* const self);


void UartSafe_package_scheduler(UartSafe* const self);

void UartSafe_new_sample(UartSafe* const self, package* sample);

#ifdef __cplusplus
}
#endif

#endif /* UART_SAFE_H */

/************************ Camilo Vera **************************END OF FILE****/