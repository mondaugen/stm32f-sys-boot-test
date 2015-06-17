/* Minimal program */
#include "stm32f4xx.h" 
#include <stdint.h> 
#include <stdio.h> 
#include <math.h> 

#define GPIO_G_init(pin)\
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;\
    GPIOG->MODER |= (1 << (pin*2))

#define GPIO_G_toggle(pin)\
    GPIOG->ODR ^= (1 << pin)

int main (void)
{
    GPIO_G_init(13);
    GPIO_G_toggle(13);
    GPIO_G_init(14);
    GPIO_G_toggle(14);
    while(1);
}
