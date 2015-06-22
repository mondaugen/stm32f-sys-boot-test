#include "adc_example.h" 
#include "stm32f4xx.h" 

#define GPIOG_PIN 9

#define NUM_ADC3_CHANS 2 
#define ADC3_AVG_SIZE 32 
#define NUM_ADC3_CONV (NUM_ADC3_CHANS*ADC3_AVG_SIZE)
#define TOTAL_NUM_AVGS 1000 

#define ADC_NO_SCAN 

#ifdef ADC_NO_SCAN
 uint16_t adc3_vals[ADC3_AVG_SIZE];
#else
 uint16_t adc3_vals[NUM_ADC3_CONV];
#endif  
uint16_t adc3_avgs[NUM_ADC3_CHANS];
uint16_t total_avgs[TOTAL_NUM_AVGS*NUM_ADC3_CHANS];
int total_avgs_index;

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
    /* PF6 is ADC3 IN 4 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    GPIOF->MODER &= ~GPIO_MODER_MODER6;
    GPIOF->MODER |= 0x3 << (6*2);
    /* Enable ADC Clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    /* Set sample time to 15 cycles */
    ADC3->SMPR2 &= ~ADC_SMPR2_SMP4;
    ADC3->SMPR2 |= 0x1 << (4*3);
    /* Set only one conversion in conversion sequence */
    ADC3->SQR1 &= ~ADC_SQR1_L;
    /* First conversion is of channel 4 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 0x4 << (0*5);
    /* Don't set end of conversion flag after every conversion */
    ADC3->CR2 &= ~ADC_CR2_EOCS;
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
     * 16-bits, transfer complete interrupt enable, memory increment . */
    DMA2_Stream0->CR |= (2 << 25)
        | (0x3 << 16)
        | (0x1 << 13)
        | (0x1 << 11)
        | (0x1 << 4)
        | DMA_SxCR_MINC;
    /* Set peripheral address to ADC3's data register */
    DMA2_Stream0->PAR = (uint32_t)&(ADC3->DR);
    /* Set memory address to some place */
    DMA2_Stream0->M0AR = (uint32_t)&adc3_vals;
    /* Set number of items to transfer */
    DMA2_Stream0->NDTR = NUM_ADC3_CONV;
    
    /* Enable DMA2_Stream0 interrupt */
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    /* Enable DMA2 */
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    /* Start conversion */
    ADC3->CR2 |= ADC_CR2_SWSTART;
}

/* This mode will scan two ADC channels and fill the DMA buffer with these
 * interleaved values */
void adc_setup_dma_scan_2_inputs(void)
{
    total_avgs_index = 0;

    /* Enable GPIO to toggle pin */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    GPIOG->MODER |= (1 << (GPIOG_PIN*2));

    /* PF6 is ADC3 IN 4 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    GPIOF->MODER &= ~GPIO_MODER_MODER6;
    GPIOF->MODER |= 0x3 << (6*2);
    /* Enable ADC Clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    /* Set clock prescalar to 4 */
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= (0x1 << 16);
    /* Set sample time to 480 cycles */
    ADC3->SMPR2 &= ~ADC_SMPR2_SMP4;
    ADC3->SMPR2 |= 0x7 << (4*3);
    /* PC3 is ADC123 IN 13 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= ~GPIO_MODER_MODER3;
    GPIOC->MODER |= 0x3 << (3*2);
    /* Set sample time to 480 cycles */
    ADC3->SMPR1 &= ~ADC_SMPR1_SMP13;
    ADC3->SMPR1 |= 0x7 << 9;
    /* Set two conversions in conversion sequence */
    ADC3->SQR1 &= ~ADC_SQR1_L;
    ADC3->SQR1 |= (1 << 20);
    /* First conversion is of channel 4 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 4 << (0*5);
    /* Second conversion is of channel 13 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ2;
    ADC3->SQR3 |= 13 << (1*5);
    /* Don't set end of conversion flag after every conversion */
    ADC3->CR2 &= ~ADC_CR2_EOCS;
    /* Set continuous conversion */
    ADC3->CR2 |= ADC_CR2_CONT;
    /* Set scan mode */
    ADC3->CR1 |= ADC_CR1_SCAN;
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
     * 16-bits, transfer complete interrupt enable, memory increment . */
    DMA2_Stream0->CR |= (2 << 25)
        | (0x3 << 16)
        | (0x1 << 13)
        | (0x1 << 11)
        | (0x1 << 4)
        | DMA_SxCR_MINC;
    /* Set peripheral address to ADC3's data register */
    DMA2_Stream0->PAR = (uint32_t)&(ADC3->DR);
    /* Set memory address to some place */
    DMA2_Stream0->M0AR = (uint32_t)&adc3_vals;
    /* Set number of items to transfer */
    DMA2_Stream0->NDTR = NUM_ADC3_CONV;
    
    /* Enable DMA2_Stream0 interrupt */
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    /* Enable DMA2 */
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    /* Start conversion */
    ADC3->CR2 |= ADC_CR2_SWSTART;
}

/* This mode keeps converting a single channel and the channel is changed every
 * DMA interrtupt */
void adc_setup_dma_no_scan(void)
{
    total_avgs_index = 0;

    /* Enable GPIO to toggle pin */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    GPIOG->MODER |= (1 << (GPIOG_PIN*2));

    /* PF6 is ADC3 IN 4 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    GPIOF->MODER &= ~GPIO_MODER_MODER6;
    GPIOF->MODER |= 0x3 << (6*2);
    /* Enable ADC Clock for IN 13 (this will be switched to in DMA interrupt) */
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    /* Set clock prescalar to 4 */
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= (0x1 << 16);
    /* Set sample time to 480 cycles */
    ADC3->SMPR2 &= ~ADC_SMPR2_SMP4;
    ADC3->SMPR2 |= 0x7 << (4*3);
    /* PC3 is ADC123 IN 13 */
    /* Setup GPIO for analog mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= ~GPIO_MODER_MODER3;
    GPIOC->MODER |= 0x3 << (3*2);
    /* Set one conversion in conversion sequence */
    ADC3->SQR1 &= ~ADC_SQR1_L;
    ADC3->SQR1 |= (0 << 20);
    /* First conversion is of channel 4 */
    ADC3->SQR3 &= ~ADC_SQR3_SQ1;
    ADC3->SQR3 |= 4 << (0*5);
    /* Don't set end of conversion flag after every conversion */
    ADC3->CR2 &= ~ADC_CR2_EOCS;
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
     * 16-bits, transfer complete interrupt enable, memory increment . */
    DMA2_Stream0->CR |= (2 << 25)
        | (0x3 << 16)
        | (0x1 << 13)
        | (0x1 << 11)
        | (0x1 << 4)
        | DMA_SxCR_MINC;
    /* Set peripheral address to ADC3's data register */
    DMA2_Stream0->PAR = (uint32_t)&(ADC3->DR);
    /* Set memory address to some place */
    DMA2_Stream0->M0AR = (uint32_t)&adc3_vals;
    /* Set number of items to transfer */
    DMA2_Stream0->NDTR = ADC3_AVG_SIZE;
    
    /* Enable DMA2_Stream0 interrupt */
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    /* Enable DMA2 */
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    /* Start conversion */
    ADC3->CR2 |= ADC_CR2_SWSTART;
}

void DMA2_Stream0_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
    if (DMA2->LISR & DMA_LISR_TCIF0) {
        /* Clear interrupt */
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
#ifdef ADC_NO_SCAN 
        /* Calculate average of ADC values */
        int n;
        uint32_t tmp = 0;
        for (n = 0; n < ADC3_AVG_SIZE; n++) {
            tmp += adc3_vals[n];
        }
        tmp /= ADC3_AVG_SIZE;
        if (total_avgs_index < NUM_ADC3_CHANS*TOTAL_NUM_AVGS) {
            total_avgs[total_avgs_index] = tmp;
        }
        total_avgs_index++;

        /* Toggle GPIO */
        GPIOG->ODR ^= (1 << GPIOG_PIN);

        /* Set number of items to transfer */
        DMA2_Stream0->NDTR = ADC3_AVG_SIZE;

        /* If last conversion was of channel 4 change to channel 13 */
        if ((ADC3->SQR3 & ADC_SQR3_SQ1) == 4) {
            ADC3->SQR3 &= ~ADC_SQR3_SQ1;
            ADC3->SQR3 |= 13 << (0*5);
        } else {
            /* Change to channel 4 */
            ADC3->SQR3 &= ~ADC_SQR3_SQ1;
            ADC3->SQR3 |= 4 << (0*5);
        }
#else
        /* Calculate average of ADC values */
        int n, m;
        uint32_t tmp[NUM_ADC3_CHANS];
        for (m = 0; m < NUM_ADC3_CHANS; m++) {
            tmp[m] = 0;
            for (n = 0; n < NUM_ADC3_CONV; n += NUM_ADC3_CHANS) {
                tmp[m] += adc3_vals[n+m];
            }
            adc3_avgs[m] = tmp[m] / ADC3_AVG_SIZE;
            if (total_avgs_index < TOTAL_NUM_AVGS) {
                total_avgs[total_avgs_index*NUM_ADC3_CHANS + m] =
                    adc3_avgs[m];
            }
        }
        total_avgs_index++;
        /* Toggle GPIO */
        GPIOG->ODR ^= (1 << GPIOG_PIN);
        /* Set number of items to transfer */
        DMA2_Stream0->NDTR = NUM_ADC3_CONV;
#endif  
        /* Enable DMA2 */
        DMA2_Stream0->CR |= DMA_SxCR_EN;
        /* Reset DMA */
        ADC3->CR2 &= ~ADC_CR2_DMA;
        ADC3->CR2 |= ADC_CR2_DMA;
        /* Start conversion */
        ADC3->CR2 |= ADC_CR2_SWSTART;
    }
}
