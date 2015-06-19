#include "uart_example.h" 
#include "system_init.h" 
#include "stm32f4xx.h" 

int uart_puts(const char *str)
{
    while (*str != '\0') {
        /* Wait for previous transfer to complete */
        while (!(UART5->SR & USART_SR_TXE));
        /* Transmit something */
        UART5->DR = *str; 
        str++;
    }
}

void uart_write_raw(const char *data, size_t length)
{
    while (length--) {
        while (!(UART5->SR & USART_SR_TXE));
        UART5->DR = *data;
        data++;
    }
}

void uart_setup(void)
{
    SystemCoreClockUpdate();
    /* Enable PORT C clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    /* Enable UART5 clock */
    RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
    /* Set to alternate function */
    GPIOC->MODER &= ~(0x3 << (12*2));
    GPIOC->MODER |= (1 << ((12*2)+1));
    GPIOC->AFR[1] &= ~(0xf << 16);
    GPIOC->AFR[1] |= (0x8 << 16);
    /* Enable UART */
    UART5->CR1 |= USART_CR1_UE;
    UART5->CR1 &= ~USART_CR1_M;
    /* Set Baud Rate */
    /* To setup UART with baudrate of 9600 we do the following calculation
     * DIV = (CPU_FREQ / AHB_PRESCALAR / APBx_PRESCALAR)/(9600)
     * as we don't use the 8x oversampling */
    UART5->BRR = get_SystemCoreClock()
                    / get_AHBPresc()
                    / get_APBPresc(1)
                    / BAUD_RATE;
    UART5->CR1 |= USART_CR1_TE;
}
