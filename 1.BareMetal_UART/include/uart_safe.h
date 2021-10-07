/** 
  ******************************************************************************
  * @file    uart_safe.c
  * @author  Camilo Vera
  * @brief   Serial Protocol with CRC16 verification, and linked structs.
  *          This file contains the implementation of a serial protocol, calling
  *          the HAL of the MCU, using UART with DMA, and linked structures to
  *          send 32bytes, CRC16-CCITT verified, packages.
  *
  @verbatim

  ==============================================================================
                     ##### BOARD SUPPORT PACKAGE #####
  ==============================================================================
  STRUCTS:

  FUNCTIONS:

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

struct package_struct;
struct UartSafe_data_struct;
struct UartSafe_interface_struct;
struct UartSafe_struct;


__attribute__((aligned(32))) typedef struct package_struct{
    uint32_t sensor1;                     // 0-3   bytes
    uint32_t sensor2;                     // 4-7   bytes
    uint32_t sensor3;                     // 8-11  bytes
    uint32_t sensor4;                     // 12-15 bytes
    uint32_t sensor5;                     // 16-19 bytes
    uint32_t sensor6;                     // 20-23 bytes
    uint32_t sensor7;                     // 24-27 bytes
    uint16_t control_signals;             // 28-29 bytes
    uint16_t CRC16;                       // 30-31 bytes
    struct package_struct* next_structure; // address to the next structure 

}package;


typedef struct UartSafe_data_struct{
    package str0;
    package str1;
    package str2;
    package str3;
    package str4;
    package str5;
}UartSafe_data;

typedef struct UartSafe_interface_struct{

    void (*UartSafe_constructor)(UartSafe_data* const self);
    void (*testMethod)(struct UartSafe_struct* const self, char msg[]);

}UartSafe_interface;


typedef struct UartSafe_struct{
    UartSafe_data* data;
    UartSafe_interface interface;
}UartSafe;

void void testMethod(UartSafe* const self, char msg[]);
void testMethod(UartSafe* const self);






#ifdef __cplusplus
    }
#endif

#endif /* UART_SAFE_H */