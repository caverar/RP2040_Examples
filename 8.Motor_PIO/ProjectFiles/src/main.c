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
  Example 08: PIO_STEPPER 
  This code shows the basic usage of an RP2040 CMake project, with a stepper 
  motor with PIO
   
  RP2040 (Two Cortex M0+)

  @endverbatim
  ****************************************************************************** 
*/

/* Includes ------------------------------------------------------------------*/

// Standard C libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// SDK Libraries
#include "pico/stdlib.h"
#include "hardware/pio.h"

// Project libraries
#include "pio_stepper.h"




#define MOTOR1_STEP_PIN 2
#define MOTOR1_DIR_PIN 3
#define MOTOR1_ENABLE_PIN 4

#define pio_freq 2000

StepperMotor motor1;

int main(){
    stdio_init_all();

    StepperMotor_ctor(&motor1, 
                      pio0, 
                      0, 
                      MOTOR1_DIR_PIN, 
                      MOTOR1_STEP_PIN, 
                      MOTOR1_ENABLE_PIN,
                      400,
                      1);

    while(true){
        StepperMotor_move(&motor1,
                          0, 
                          pio_freq,
                          120);
        sleep_ms(10);
    }

}


/************************ Camilo Vera **************************END OF FILE****/
