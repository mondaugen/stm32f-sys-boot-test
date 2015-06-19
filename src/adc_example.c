#include "adc_example.h" 
#include "stm32f4xx.h" 

#define NUM_ADC3_CHAN4_CONV 8 

uint16_t adc3_channel4_val[NUM_ADC3_CHAN4_CONV];

void adc_setup(void)
{
    /* PF6 is ADC3 IN 4 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    GPIOF->MODER &= ~GPIO_MODER_MODER6;
    GPIOF->MODER |= 0x3 << (6*2);
    /* Enable ADC Clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    /* Set sample time to 15 cycles */
    ADC3->SMPR2 &= ~ADC_SMPR2_SMP6;
    ADC3->SMPR2 |= 0x1 << (6*3);
    /* Set only one conversion in conversion sequence */
    ADC3->SQR1 &= ~ADC_SQR1_L;
    /* First conversion is of channel 4 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 0x4 << (0*5);
    /* Set end of conversion flag after every conversion */
    ADC3->CR2 |= ADC_CR2_EOCS;
    /* Enable ADC */
    ADC3->CR2 |= ADC_CR2_ADON;
}

void adc_setup_b(void)
{
    /* PC3 is ADC123 IN 13 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= ~GPIO_MODER_MODER3;
    GPIOC->MODER |= 0x3 << (3*2);
    /* Enable ADC Clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    /* Set sample time to 15 cycles */
    ADC3->SMPR2 &= ~ADC_SMPR2_SMP3;
    ADC3->SMPR2 |= 0x1 << (3*3);
    /* Set only one conversion in conversion sequence */
    ADC3->SQR1 &= ~ADC_SQR1_L;
    /* First conversion is of channel 13 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 13 << (0*5);
    /* Set end of conversion flag after every conversion */
    ADC3->CR2 |= ADC_CR2_EOCS;
    /* Enable ADC */
    ADC3->CR2 |= ADC_CR2_ADON;
}

void adc_setup_2(void)
{
    /* PF6 is ADC3 IN 4 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    GPIOF->MODER &= ~GPIO_MODER_MODER6;
    GPIOF->MODER |= 0x3 << (6*2);
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= ~GPIO_MODER_MODER3;
    GPIOC->MODER |= 0x3 << (3*2);
    /* Enable ADC Clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    /* Set sample time to 15 cycles */
    ADC3->SMPR2 &= ~ADC_SMPR2_SMP6;
    ADC3->SMPR2 |= 0x1 << (6*3);
    ADC3->SMPR1 &= ~ADC_SMPR1_SMP13;
    ADC3->SMPR1 |= 0x1 << 9;
    /* Set two conversions in conversion sequence */
    ADC3->SQR1 &= ~ADC_SQR1_L;
    ADC3->SQR1 |= 0x1 << 20;
    /* First conversion is of channel 4 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 4 << (0*5);
    /* Second conversion is of channel 13 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 13 << (1*5);
    /* Set end of conversion flag after every conversion */
    ADC3->CR2 |= ADC_CR2_EOCS;
    /* Enable ADC */
    ADC3->CR2 |= ADC_CR2_ADON;
}

uint32_t adc_read(void)
{
    /* Start conversion */
    ADC3->CR2 |= ADC_CR2_SWSTART;
    /* Wait for end of conversion */
    while (!(ADC3->SR & ADC_SR_EOC));
    /* Clear EOC bit */
    ADC3->SR &= ~ADC_SR_EOC;
    /* Return value in ADC register */
    return ADC3->DR & 0x0000ffff;
}

uint32_t adc_read_2(void)
{
    /* Start conversion */
    ADC3->CR2 |= ADC_CR2_SWSTART;
    /* Wait for end of conversion */
    while (!(ADC3->SR & ADC_SR_EOC));
    /* Clear EOC bit */
    ADC3->SR &= ~ADC_SR_EOC;
    /* Return value in ADC register */
    return ADC3->DR & 0x0000ffff;
}

void adc_setup_dma_1_datum(void)
{
    /* Configure a timer that will trigger the conversion */
    /* Enable timer peripheral clock */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    /* Set to upcounting mode */
    TIM3->CR1 &= ~TIM_CR1_DIR;
    /* Set clock divider to 4, if AHB clock is 90MHz, the timer clock is now
     * 22.5MHz */
    TIM3->CR1 &= ~TIM_CR1_CKD;
    TIM3->CR1 |= 0x2 << 8;
    /* Set overflow value.
     * If the clock speed is 90MHz, this should overflow at 1KHz */
    TIM3->ARR = 22500-1; 
    /* Trigger DMA request on update event */
    TIM3->DIER |= TIM_DIER_TDE;
    

    /* PF6 is ADC3 IN 4 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    GPIOF->MODER &= ~GPIO_MODER_MODER6;
    GPIOF->MODER |= 0x3 << (6*2);
    /* Enable ADC Clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    /* Set sample time to 15 cycles */
    ADC3->SMPR2 &= ~ADC_SMPR2_SMP6;
    ADC3->SMPR2 |= 0x1 << (6*3);
    /* Set only one conversion in conversion sequence */
    ADC3->SQR1 &= ~ADC_SQR1_L;
    /* First conversion is of channel 4 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 0x4 << (0*5);
    /* Don't set end of conversion flag after every conversion */
    ADC3->CR2 &= ~ADC_CR2_EOCS;
    /* Timer 3 triggers conversion on rising edge */
    /*
    ADC3->CR2 &= ~ADC_CR2_EXTSEL;
    ADC3->CR2 |= 0x8 << 24;
    ADC3->CR2 &= ~ADC_CR2_EXTEN;
    ADC3->CR2 |= 0x1 << 28;
    */
    /* Set continuous conversion */
    ADC3->CR2 |= ADC_CR2_CONT;
    /* Enable DMA */
    ADC3->CR2 |= ADC_CR2_DMA;
    /* Enable ADC */
    ADC3->CR2 |= ADC_CR2_ADON;

    /* DMA Setup */
    /* Turn on DMA2 clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    /* Reset control register */
    DMA2_Stream0->CR = 0x00000000;
    /* Set to channel 2, very high priority, memory and peripheral datum size
     * 16-bits, transfer complete interrupt enable. No memory increment yet. */
    DMA2_Stream0->CR |= (2 << 25)
        | (0x3 << 16)
        | (0x1 << 13)
        | (0x1 << 11)
        | (0x1 << 4);
    /* Set peripheral address to ADC3's data register */
    DMA2_Stream0->PAR = (uint32_t)&(ADC3->DR);
    /* Set memory address to some place */
    DMA2_Stream0->M0AR = (uint32_t)&adc3_channel4_val;
    /* Set number of items to transfer */
    DMA2_Stream0->NDTR = NUM_ADC3_CHAN4_CONV;
    
    /* Enable DMA2_Stream0 interrupt */
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    /* Enable DMA2 */
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    /* Enable timer */
    TIM3->CR1 |= TIM_CR1_CEN;

    /* Start conversion */
    ADC3->CR2 |= ADC_CR2_SWSTART;
}

void DMA2_Stream0_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
    if (DMA2->LISR & DMA_LISR_TCIF0) {
        /* Clear interrupt */
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
        /* Set number of items to transfer */
        DMA2_Stream0->NDTR = NUM_ADC3_CHAN4_CONV;
        /* Enable DMA2 */
        DMA2_Stream0->CR |= DMA_SxCR_EN;
        /* Start conversion */
        ADC3->CR2 |= ADC_CR2_SWSTART;
    }
}
