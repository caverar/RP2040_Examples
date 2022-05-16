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
  Example 04: LCD_I2C 
  This code shows the basic usage of an RP2040 CMake project, with buttons over
  ADC
   
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
#include "hardware/adc.h"
//#include "hardware/uart.h"
//#include "hardware/gpio.h"


// Project libraries

//#include "bsp.h"



#define SW1_MIN_VAL 0
#define SW1_MAX_VAL 50
#define SW2_MIN_VAL 100
#define SW2_MAX_VAL 200
#define SW3_MIN_VAL 300
#define SW3_MAX_VAL 450
#define SW4_MIN_VAL 600
#define SW4_MAX_VAL 750
#define SW5_MIN_VAL 1400
#define SW5_MAX_VAL 1500

 

#define DEBOUNCE_LOW_LEVEL 10
#define DEBOUNCE_HIGH_LEVEL 50

bool sw_debounce(bool current_state, u_int16_t* level){

    if(current_state){  // load and discharge of software capacitor 
        *level = (*level >= DEBOUNCE_HIGH_LEVEL) ? DEBOUNCE_HIGH_LEVEL : *level+1;
    }else{
        *level = (*level <= DEBOUNCE_LOW_LEVEL) ? DEBOUNCE_LOW_LEVEL : *level-1;
    }
    return *level >= DEBOUNCE_HIGH_LEVEL;
}

void debounce_all(uint16_t adc_val,
                  uint16_t* buttons_levels,
                  uint8_t* buttons_states){ 
    
    if(SW1_MIN_VAL<adc_val && adc_val<SW1_MAX_VAL){
        *buttons_states &= ~(0x01<<0);
        *buttons_states |= (0x01 && sw_debounce(true, &buttons_levels[0]))<<0;

    }else if(SW2_MIN_VAL<adc_val && adc_val<SW2_MAX_VAL){
        *buttons_states &= ~(0x01<<1);
        *buttons_states |= (0x01 && sw_debounce(true, &buttons_levels[1]))<<1;

    }else if(SW3_MIN_VAL<adc_val && adc_val<SW3_MAX_VAL){
        *buttons_states &= ~(0x01<<2);
        *buttons_states |= (0x01 && sw_debounce(true, &buttons_levels[2]))<<2;

    }else if(SW4_MIN_VAL<adc_val && adc_val<SW4_MAX_VAL){
        *buttons_states &= ~(0x01<<3);
        *buttons_states |= (0x01 && sw_debounce(true, &buttons_levels[3]))<<3;

    }else if(SW5_MIN_VAL<adc_val && adc_val<SW5_MAX_VAL){
        *buttons_states &= ~(0x01<<4);
        *buttons_states |= (0x01 && sw_debounce(true, &buttons_levels[4]))<<4;

    }else{
        buttons_states[0] = sw_debounce(false, &buttons_levels[0]);
        buttons_states[1] = sw_debounce(false, &buttons_levels[1]);
        buttons_states[2] = sw_debounce(false, &buttons_levels[2]);
        buttons_states[3] = sw_debounce(false, &buttons_levels[3]);
        buttons_states[4] = sw_debounce(false, &buttons_levels[4]);
    }
}


int main() {


    adc_init();
    adc_set_clkdiv(4799);  //10Ks/s
    // Initialize ADC pin
    adc_gpio_init(28);
    adc_select_input(2);
    adc_run(true);
    

    // Initialize chosen serial port
    stdio_init_all();
    
    uint16_t result;
    int time;

    uint8_t buttons_past_states = 0x00;
    uint8_t buttons_states = 0x00;
    uint16_t buttons_levels[5] = {0,0,0,0,0};



    
    // Loop forever
    while (true) {

        result = adc_hw->result;
        debounce_all(result, buttons_levels, &buttons_states);

        uint8_t buttons_pressed = buttons_states & ~buttons_past_states;
        buttons_past_states = buttons_states;

        printf("states: %d\n", buttons_pressed);
        
        sleep_ms(1);     // simulate FreeRTOS ticks 
    }
}


/************************ Camilo Vera **************************END OF FILE****/
