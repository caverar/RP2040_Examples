#include "uart_safe.h"

// Standard C libraries
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Hardware API libraries






void UartSafe_constructor(UartSafe* const self){
    self->data->str0.next_structure = &(self->data->str1);
    self->data->str1.next_structure = &(self->data->str2);
    self->data->str2.next_structure = &(self->data->str3);
    self->data->str3.next_structure = &(self->data->str4);
    self->data->str4.next_structure = &(self->data->str5);
    self->data->str5.next_structure = &(self->data->str0);

    self->interface.testMethod = testMethod;
    //void testMethod(UartSafe* const self);    

}

void testMethod(UartSafe* const self, char msg[]){
    printf(msg);
    stdio_uart_init(); 
}