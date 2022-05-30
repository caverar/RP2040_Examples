/** 
  ******************************************************************************
  * @file    spi_sd.c
  * @author  Camilo Vera
  * @brief   this library allow simple usage of SD card over SPI with FATFS.
  * 

  ****************************************************************************** 
*/

#include "spi_sd.h"

// Standard C libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


// SDK Libraries
#include "pico/stdlib.h"


// Project libraries
#include "ff.h"


/**
 * @brief Mount a microSD card to FATFS system
 * 
 * @param spi_sd pointer to object instance
 * @return MOUNT_ACK mount process result
 */
static MOUNT_ACK mount(SpiSD* spi_sd){
    fr = f_mount(&(spi_sd->fs), "", 1);
    return (fr == FR_OK) ? MOUNT_OK : MOUNT_ERROR;
}





void SpiSD_read(SpiSD* spi_sd, 
             const char* const file_name, 
             uint8_t* read_buffer, 
             uint8_t size){

    static uint32_t read_bytes;
    static FILINFO file_detect_info;
    

    if(mount(spi_sd) == MOUNT_ERROR){
        spi_sd->read_ack = READ_MOUNT_ERROR;
    
    }else{
        if(f_stat(file_name, &file_detect_info)){
            spi_sd->read_ack = READ_MISSING_FILE_ERROR;
        }
        else if( f_open(&(spi_sd->fil), file_name, FA_READ) != FR_OK ){
            spi_sd->read_ack = READ_OPEN_ERROR;

        }else if(f_read(&(spi_sd->fil), read_buffer, size, (UINT*)&read_bytes)
                != FR_OK){
            spi_sd->read_ack = READ_ERROR;
            f_close(&(spi_sd->fil));

        }else if(read_bytes != size){
            spi_sd->read_ack = READ_SIZE_ERROR;
            f_close(&(spi_sd->fil));

        }else{    
            f_close(&(spi_sd->fil));
            spi_sd->read_ack = READ_OK;
        }
        f_unmount("");
    }
}

void SpiSD_write(SpiSD* spi_sd, 
             const char* const file_name, 
             uint8_t* write_buffer, 
             uint8_t size){

    static uint32_t written_bytes;
    static FILINFO file_detect_info;
    
    if(mount(spi_sd) == MOUNT_ERROR){
        spi_sd->write_ack = WRITE_MOUNT_ERROR;
    
    }else{
        if(f_open(&(spi_sd->fil), file_name, FA_READ | FA_WRITE 
                  | FA_CREATE_ALWAYS) != FR_OK){
            spi_sd->write_ack = WRITE_OPEN_ERROR;

        }else if(f_write(&(spi_sd->fil), write_buffer, size, 
                         (UINT*)&written_bytes) != FR_OK){
            spi_sd->write_ack = WRITE_ERROR;
            f_close(&(spi_sd->fil));

        }else if(written_bytes != size){
            spi_sd->write_ack = WRITE_SIZE_ERROR;
            f_close(&(spi_sd->fil));

        }else{    
            f_close(&(spi_sd->fil));
            spi_sd->read_ack = WRITE_OK;
        }
        f_unmount("");
    }
}
/************************ Camilo Vera **************************END OF FILE****/