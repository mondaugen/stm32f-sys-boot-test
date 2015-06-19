#include "adc_example.h" 
#include "stm32f4xx.h" 

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
