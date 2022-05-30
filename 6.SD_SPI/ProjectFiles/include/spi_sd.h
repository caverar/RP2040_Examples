/** 
  ******************************************************************************
  * @file    spi_sd.c
  * @author  Camilo Vera
  * @brief   this library allow simple usage of SD card over SPI with FATFS.
  * 

  ****************************************************************************** 
*/
#ifndef _SPI_SD_H
#define _SPI_SD_H

#ifdef __cplusplus
extern "C" {
#endif

// Standard C libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// SDK Libraries
#include "pico/stdlib.h"


// Project libraries
#include "ff.h"


static FRESULT fr;                 // FatFs return code
static UINT br;                    // Bytes read
static UINT bw;                    // bytes written

typedef enum{
    MOUNT_OK,
    MOUNT_ERROR
}MOUNT_ACK;


typedef enum{
    READ_OK,
    READ_MOUNT_ERROR,
    READ_MISSING_FILE_ERROR,
    READ_OPEN_ERROR,
    READ_FILE_MISSING,
    READ_SIZE_ERROR,
    READ_ERROR
}READ_ACK;

typedef enum{
    WRITE_OK,
    WRITE_MOUNT_ERROR,
    WRITE_OPEN_ERROR,
    WRITE_SIZE_ERROR,
    WRITE_ERROR
}WRITE_ACK;

typedef struct {
    FATFS fs;               // File system
    FIL fil;                // File
    READ_ACK read_ack;
    WRITE_ACK write_ack;
}SpiSD;

static MOUNT_ACK mount(SpiSD* spi_sd);



void SpiSD_read(SpiSD* spi_sd, 
                const char* const file_name, 
                uint8_t* read_buffer, 
                uint8_t size);

void SpiSD_write(SpiSD* spi_sd, 
                 const char* const file_name, 
                 uint8_t* write_buffer, 
                 uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* SPI_SD_H */
/************************ Camilo Vera **************************END OF FILE****/