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

#define FLASH_SIMPLE_WRITE 0
#define FLASH_EXAMPLE_WRITE 0
#define FLASH_EXAMPLE_WRITE_2 0
#define FLASH_EXAMPLE_WRITE_LONG 0
#define FLASH_EXAMPLE_READ  0
#define SDRAM_EXAMPLE_1 1 

int main (void)
{
#if FLASH_SIMPLE_WRITE
   flash_setup();
   simple_flash_write();
#elif FLASH_EXAMPLE_WRITE
   flash_setup();
   char stuff[] = "zombocom";
   flash_erase();
   flash_write(stuff,8);
#elif FLASH_EXAMPLE_WRITE_2
   flash_setup();
   char stuff[] = "mega-bong";
   flash_erase();
   flash_write(stuff,8);
#elif FLASH_EXAMPLE_READ
   flash_setup();
   char stuff[] = {0,0,0,0,0,0,0,0};
   flash_read(stuff,8);
#elif FLASH_EXAMPLE_WRITE_LONG
   flash_setup();
   flash_erase();
   long_flash_write();
#elif SDRAM_EXAMPLE_1
   int result;
   fmc_setup();
   result = fmc_read_write_test();
#endif  
   while(1);
}
