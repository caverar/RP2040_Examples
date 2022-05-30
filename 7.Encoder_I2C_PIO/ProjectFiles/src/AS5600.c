/** 
  ******************************************************************************
  * @file    AS5600.c
  * @author  Camilo Vera
  * @brief   AS5600 encoder library
  *          Description of the file functions, etc. 
  ****************************************************************************** 
*/


#include "AS5600.h"

/* Includes ------------------------------------------------------------------*/

// Standard C libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// SDK Libraries
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pio_i2c.h"



void read_register(i2c_inst_t* i2c_instance, 
                   uint8_t device_addr,
                   uint8_t register_address,
                   uint8_t* read_buffer, 
                   uint8_t size){

    
    pio_i2c_write_blocking(pio0, 0, device_address, &register_address, 1);
    pio_i2c_read_blocking(pio0, 0, device_address, read_buffer, size);
}

uint16_t read_angle(i2c_inst_t* i2c){
    uint8_t read_data[4];
    read_register(i2c, device_address, raw_angle_address+1, read_data, 4);
    //return (read_data[1]);
    return (read_data[1]<<8) + read_data[2];
}
