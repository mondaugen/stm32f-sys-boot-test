#ifndef ADC_EXAMPLE_H
#define ADC_EXAMPLE_H 
#include <stdint.h> 
#include <stdio.h> 
#include "uart_example.h" 

#define adc_loop(timeout_limit)\
        uint32_t adc_val;\
        char buf[100];\
        /* Wait for a certain number of timeouts */;\
        while (ntimeouts < timeout_limit);\
        /* Reset timeout counter */\
        ntimeouts = 0;\
        /* Read ADC value */\
        adc_val = adc_read();\
        /* Print into buf */\
        sprintf(buf,"%d\n\r",(int)(1000.0*((float)adc_val/4096.0)));\
        /* Send buf */\
        uart_puts(buf)

#define adc_loop_2(timeout_limit)\
        uint32_t adc_val;\
        char buf[100];\
        /* Wait for a certain number of timeouts */;\
        while (ntimeouts < timeout_limit);\
        /* Reset timeout counter */\
        ntimeouts = 0;\
        /* Enable scan mode */\
        ADC3->CR1 |= ADC_CR1_SCAN;\
        int n;\
        for (n = 0; n < 2; n++) {\
            /* Read ADC value */\
            adc_val = adc_read_2();\
            /* Print into buf */\
            sprintf(buf,"%d\n\r",(int)(1000.0*((float)adc_val/4096.0)));\
            /* Send buf */\
            uart_puts(buf);\
        }

void adc_setup(void);
void adc_setup_2(void);
uint32_t adc_read(void);
uint32_t adc_read_2(void);
#endif /* ADC_EXAMPLE_H */
