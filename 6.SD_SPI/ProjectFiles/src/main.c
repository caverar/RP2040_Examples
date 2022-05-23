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





// Set PRE_ALLOCATE true to pre-allocate file clusters.
const bool PRE_ALLOCATE = true;

// Set SKIP_FIRST_LATENCY true if the first read/write to the SD can
// be avoid by writing a file header or reading the first record.
const bool SKIP_FIRST_LATENCY = true;

// Size of read/write.
#define BUF_SIZE 512

// File size in MB where MB = 1,000,000 bytes.
#define FILE_SIZE_MB 5

// Write pass count.
#define WRITE_COUNT 2

// Read pass count.
#define READ_COUNT 2
//==============================================================================
// End of configuration constants.
//------------------------------------------------------------------------------
// File size in bytes.
const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;

// Insure 4-byte alignment.
uint32_t buf32[(BUF_SIZE + 3)/4];
uint8_t* buf = (uint8_t*)buf32;



int main()
{
    FATFS fs;
    FIL fil;
    FRESULT fr;     /* FatFs return code */
    UINT br;
    UINT bw;

    float s;
    uint32_t t;
    uint32_t maxLatency;
    uint32_t minLatency;
    uint32_t totalLatency;
    bool skipLatency;


    stdio_init_all();

    printf("=====================\n");
    printf("== pico_fatfs_test ==\n");
    printf("=====================\n");


    //1: Mount unit
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        printf("mount error %d\n", fr);
    }else{
        printf("mount ok\n");
    }

    // 2. Open file for writting

    fr = f_open(&fil, "bench2.dat", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("open error %d\n", fr);
    }else{
        printf("open ok\n");
    }

    // 3.Write something to file
    uint32_t written_bytes;
    fr = f_write(&fil, "This is another test Camilo 2022\r\n",34,(UINT*)&written_bytes);
    if(fr != FR_OK){
        printf("ERROR: Could not write the file {%d}\r\n",fr);
        f_close(&fil);
        while(true);
    }else{
        printf("write ok\n");
    }

    // 5. Close the file to save changes
    f_close(&fil);
    printf("\nDone\n");

    // 6. Open a file for read

    fr = f_open(&fil, "bench2.dat", FA_READ);
    if (fr != FR_OK) {
        printf("open error %d\n", fr);
    }else{
        printf("open ok\n");
    }

    // 7. read a file

    char read_buffer[40];
    uint32_t read_bytes;
    fr = f_read(&fil,read_buffer,40,(UINT*)&read_bytes);

    if(fr != FR_OK){
        printf("ERROR: Could not read the file {%d}\r\n",fr);
        f_close(&fil);
        while(true);
    }else{
        printf("read ok\n");
    }
    printf((char*)&read_buffer);

    // 8. Close the file
    f_close(&fil);
    printf("\nDone\n");




}