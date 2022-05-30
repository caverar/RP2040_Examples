/** 
  ******************************************************************************
  * @file    main.c
  * @author  Camilo Vera
  * @brief   Main program.
  *          Description of the file functions, etc. 
  *
  @verbatim

  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  Example 06: SD_SPI 
  This code shows the basic usage of an RP2040 CMake project, with an SD card
  via SPI using the library FatFS
   
  RP2040 (Two Cortex M0+)

  @endverbatim
  ****************************************************************************** 
*/


// Standard C libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


// SDK Libraries
#include "pico/stdlib.h"


// Project libraries
#include "ff.h"
#include "spi_sd.h"


SpiSD spi_sd;
uint8_t read_buffer[40];

int main(){
    stdio_init_all();

    SpiSD_write(&spi_sd,"TEST.txt","prueba.txt",10);
    printf("%d\n",spi_sd.write_ack);
    
    SpiSD_read(&spi_sd, "bench2.dat", read_buffer, 40);
    printf("%d\n",spi_sd.read_ack);
    printf( (char*)read_buffer);
    
    memset(read_buffer, '\0', sizeof read_buffer); // Clear buffer
    SpiSD_read(&spi_sd, "TEST.txt", read_buffer, 10);
    printf("%d\n",spi_sd.read_ack);
    printf( (char*)read_buffer);
}