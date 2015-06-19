#include "timer_example.h" 
#include "stm32f4xx.h" 

#define GPIOG_PIN 9

uint32_t ntimeouts = 0;

void timer_setup(void)
{
    /* Enable timer peripheral clock */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    /* Set to upcounting mode */
    TIM2->CR1 &= ~TIM_CR1_DIR;
    /* Set overflow value */
    TIM2->ARR = 89; /* If the clock speed is 90MHz, this should overflow at 1MHz */
    /* Enable interrupt generation */
    TIM2->DIER |= TIM_DIER_UIE;
    /* Enable interrupts in NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);
    /* Enable GPIO to toggle pin */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    GPIOG->MODER |= (1 << (GPIOG_PIN*2));
    /* Enable timer */
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    if (TIM2->SR & TIM_SR_UIF) {
        /* Clear pending interrupt */
        TIM2->SR &= ~TIM_SR_UIF;
        /* Toggle GPIO */
        GPIOG->ODR ^= (1 << GPIOG_PIN);
        ntimeouts++;
    }
}
