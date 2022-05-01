/** 
  ******************************************************************************
  * @file    main.c
  * @author  Camilo Vera
  * @brief   Main program.
  *          Description of the file functions, etc. 
  *
  @verbatim
  ==============================================================================
                     ##### Project Description #####
  ==============================================================================
  Example 02: FreeAct.
 
  This code is a test with FreeACT running on RP2040.
   
  MCU: RP2040 (Two Cortex M0+).
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
//#include "hardware/uart.h"
//#include "hardware/gpio.h"

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

// FreeAct
#include <FreeAct.h>

// Project libraries
#include "bsp.h"

/* Blinker AO ================================================================*/

enum Signals {
    TIMEOUT_SIG = USER_SIG,
    BUTTON_PRESSED_SIG,
    BUTTON_RELEASED_SIG,
};

//object data:
typedef struct {
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    TimeEvent te;
    bool isLedOn;
} BlinkyButton;

// object static instance:
// extern Active *AO_blinkyButton;

//#define portTICK_RATE_MS 1

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint TEST_PIN = 15;
// AO CallBack

static void BlinkyButton_dispatch(BlinkyButton * const this, 
                                  Event const * const e) {
    switch (e->sig) {
        case INIT_SIG: /* intentionally fall through... */
        case TIMEOUT_SIG: {
            if (!this->isLedOn) { /* LED not on */
                gpio_put(LED_PIN, 1);
                gpio_put(TEST_PIN, 1);
                this->isLedOn = true;
                TimeEvent_arm(&this->te, (500 / portTICK_RATE_MS), 0U);
            }
            else {  /* LED is on */
                gpio_put(LED_PIN, 0);
                gpio_put(TEST_PIN, 0);
                this->isLedOn = false;
                TimeEvent_arm(&this->te, (500 / portTICK_RATE_MS), 0U);
            }
            break;
        }
        // case BUTTON_PRESSED_SIG: {
        //     BSP_led1_on();
        //     break;
        // }
        // case BUTTON_RELEASED_SIG: {
        //     BSP_led1_off();
        //     break;
        // }
        default: {
            break;
        }
    }
}

// Constructor

void BlinkyButton_ctor(BlinkyButton * const this) {
    Active_ctor(&this->super, (DispatchHandler)&BlinkyButton_dispatch);
    TimeEvent_ctor(&this->te, TIMEOUT_SIG, &this->super);
    this->isLedOn = false;

    // Init code
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(TEST_PIN);
    gpio_set_dir(TEST_PIN, GPIO_OUT);
}

// Task Data
static StackType_t blinkyButton_stack[configMINIMAL_STACK_SIZE]; // task stack
static Event *blinkyButton_queue[10];
static BlinkyButton blinkyButton;

//object static instance and inheritance from Active class:
Active *AO_blinkyButton = &blinkyButton.super;




int main() {
    stdio_init_all();



    //xTaskCreate(led_task, "LED_TASK",256, NULL, 1, NULL);
    //xTaskCreate(echo, "ECHO",256, NULL, 1, NULL);
    //vTaskStartScheduler();

    /* create and start the BlinkyButton AO */
    BlinkyButton_ctor(&blinkyButton);
    Active_start(AO_blinkyButton,
                 1U,
                 blinkyButton_queue,
                 sizeof(blinkyButton_queue)/sizeof(blinkyButton_queue[0]),
                 blinkyButton_stack,
                 sizeof(blinkyButton_stack),
                 0U);

    //BSP_start(); /* configure and start interrupts */

    vTaskStartScheduler(); /* start the FreeRTOS scheduler... */
    return 0; /* NOTE: the scheduler does NOT return */
    
    //while(true); 
    
}