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


void read_register(i2c_inst_t* i2c_instance, 
                   uint8_t device_addr,
                   uint8_t register_address,
                   uint8_t* read_buffer, 
                   uint8_t size){

    i2c_write_timeout_us(i2c_instance,device_addr,&register_address,1,false,500);
    
    i2c_read_timeout_us(i2c_instance,device_address,read_buffer,size,false,500);
}

uint16_t read_angle(i2c_inst_t* i2c){
    uint8_t read_data[2];
    read_register(i2c, device_address, raw_angle_address,read_data, 2);
    return (read_data[0] << 8) + read_data[1];
}
