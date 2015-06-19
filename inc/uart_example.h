#ifndef UART_EXAMPLE_H
#define UART_EXAMPLE_H 
#include <stddef.h>
#define BAUD_RATE 9600UL 
int uart_puts(const char *str);
void uart_write_raw(const char *data, size_t length);
void uart_setup(void);
/* Call this in main's while loop to print something every "timeout_limit"
 * ticks.
 * uart_setup() and timer_setup() have to have been called.
 */
#define uart_loop(timeout_limit)\
        /* Wait for a certain number of timeouts */;\
        while (ntimeouts < timeout_limit);\
        /* Reset timeout counter */\
        ntimeouts = 0;\
        /* Send something */\
        uart_puts("hello")
    
#endif /* UART_EXAMPLE_H */
