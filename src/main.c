/* Minimal program */
#include "stm32f4xx.h" 
#include <stdint.h> 
#include <stdio.h> 
#include <math.h> 
#include "timer_example.h" 
#include "uart_example.h" 
#include "adc_example.h" 
#include <stdlib.h> 
#include <string.h> 

#define TIMEOUT_LIMIT 1000000 

#define GPIO_G_init(pin)\
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;\
    GPIOG->MODER |= (1 << (pin*2))

#define GPIO_G_toggle(pin)\
    GPIOG->ODR ^= (1 << pin)

int main (void)
{
    uart_setup();
    timer_setup();
//    adc_setup_2();
    adc_setup_dma_1_datum();
    float f;
    while (1) {
//        f = sinf(1.);
//        adc_loop_2(TIMEOUT_LIMIT);
    }
}
