#include "fmc_test.h" 

#ifndef HCLK
 #define HCLK 180000000UL
#endif

/* All values are derived from the IS42S16400J datasheet */

char sram_data[SDRAM_LENGTH_SHORT];

/* Give an array of pins with last pin as -1 and will initialize the pins */
static void __attribute__((optimize("O0")))
fmc_gpio_config_helper(GPIO_TypeDef *gpio, int *pins)
{
    while (*pins != -1) {
        /* reset mode reg */
        gpio->MODER &= ~(0x3 << (2 * (*pins)));
        /* set mode reg to "alternate mode" */
        gpio->MODER |= 0x2 << (2 * (*pins));
        /* reset speed reg */
        gpio->OSPEEDR &= ~(0x3 << (2 * (*pins)));
        /* set speed to "fast" */
        gpio->OSPEEDR |= 0x2 << (2 * (*pins));
        /* reset output type register, which sets it to push pull */
        gpio->OTYPER &= ~(0x1 << *pins);
        /* reset pull up / pull down reg */
        gpio->PUPDR &= ~(0x3 << (2 * (*pins)));
        if (*pins > 7) {
            /* reset high alternate function register */
            gpio->AFR[1] &= ~(0xf << (4 * (*pins - 8)));
            /* set to AF12 */
            gpio->AFR[1] |= 0xc << (4 * (*pins - 8));
        } else {
            /* reset low alternate function register */
            gpio->AFR[0] &= ~(0xf << (4 * (*pins)));
            /* set to AF12 */
            gpio->AFR[0] |= 0xc << (4 * (*pins));
        }
        pins++;
    }
}

void __attribute__((optimize("O0"))) fmc_setup(void)
{  
    uint32_t tmpr = 0;
    volatile uint32_t timeout;

    /* GPIO configuration ------------------------------------------------------*/ 
    /* Enable GPIOs clock */
    /*
       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB
       | RCC_AHB1Periph_GPIOC
       | RCC_AHB1Periph_GPIOD
       | RCC_AHB1Periph_GPIOE
       | RCC_AHB1Periph_GPIOF
       | RCC_AHB1Periph_GPIOG
       ,ENABLE);
       */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN
        | RCC_AHB1ENR_GPIOCEN
        | RCC_AHB1ENR_GPIODEN
        | RCC_AHB1ENR_GPIOEEN
        | RCC_AHB1ENR_GPIOFEN
        | RCC_AHB1ENR_GPIOGEN;

    /* Common GPIO configuration */
    /*
       GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
       GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  |GPIO_Pin_1  |GPIO_Pin_8 |GPIO_Pin_9 |
       GPIO_Pin_10 |GPIO_Pin_14 |GPIO_Pin_15;
       */

    int gpiod_pins[] = {0,1,8,9,10,14,15,-1};
    fmc_gpio_config_helper(GPIOD,gpiod_pins);
    int gpioe_pins[] = {0,1,7,8,9,10,11,12,13,14,15,-1};
    fmc_gpio_config_helper(GPIOE,gpioe_pins);
    int gpiof_pins[] = {0,1,2,3,4,5,11,12,13,14,15,-1};
    fmc_gpio_config_helper(GPIOF,gpiof_pins);
    int gpiog_pins[] = {0,1,4,5,8,15,-1};
    fmc_gpio_config_helper(GPIOG,gpiog_pins);
    int gpiob_pins[] = {5,6,-1};
    fmc_gpio_config_helper(GPIOB,gpiob_pins);
    int gpioc_pins[] = {0,-1};
    fmc_gpio_config_helper(GPIOC,gpioc_pins);

    /* Enable FMC clock */
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;
    
    /* Configure SDRAM */
    /* SDCLK RBURST and RPIPE must be programmed in bank 1's configuration
     * register */
    /* Reset */
    FMC_Bank5_6->SDCR[0] = 0x00000000;
    /* Read pipe delay = 1 (from ST's example),
     * No bursts
     * SDRAMCLK divider = 3 (HCLK = 180Mhz, SDRAMCLK = 60Mhz)*/
    FMC_Bank5_6->SDCR[0] |=  (0x1 << 13)
                            |(0x0 << 12)
                            |(0x3 << 10);
    /* CAS latency = 3 cycles
     * Number of banks = 4
     * Memory data bus width = 16bits
     * Row addressing = 12bits
     * Column addressing = 8 bits
     */
    FMC_Bank5_6->SDCR[1] = 0x00000000;
    FMC_Bank5_6->SDCR[1] |= (0x3 << 7)
                            |(0x1 << 6)
                            |(0x1 << 4)
                            |(0x1 << 2)
                            |(0x0 << 0);

    /* Set up timing on SDRAM bank 2 */
    FMC_Bank5_6->SDTR[0] = 0x00000000;
    /* TRP and TRC must be programmed in bank 1's timing register */
    FMC_Bank5_6->SDTR[0] |=  ((1 - 1) << 20)
                            |((4 - 1) << 12);
    /* Reset */
    FMC_Bank5_6->SDTR[1] = 0x00000000;
    /* Load to active delay (TMRD) = 2CLK -> 2 cycles
     * Exit self refresh delay (TXSR) = min 70ns -> ~5 cycles
     * Self refresh time (TRAS) = min 42ns -> ~3 cycles
     * Row cycle delay (TRC) = min 63ns -> ~4 cycles
     * Write recovery time (TWR) = 2CLK -> 2 cycles
     * Row precharge delay (TRP) = 15ns -> ~1 cycle
     * Row column delay (TRCD) = 15ns -> ~1 cycle
     */
    FMC_Bank5_6->SDTR[1] |=  ((1 - 1) << 24)
                            |((2 - 1) << 16)
                            |((3 - 1) << 8)
                            |((5 - 1) << 4)
                            |((2 - 1) << 0);

    /* Wait 100ms */
    timeout = 10000000;
    while (timeout--);

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

    /* Issue clock configuration command to bank 2 */
    FMC_Bank5_6->SDCMR = 0x00000000;
    FMC_Bank5_6->SDCMR |= 0x1 << 3 /* Set bank 2 */ | 0x1; /* Set mode 001 */

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

    /* Issue precharge all command to bank 2 */
    FMC_Bank5_6->SDCMR = 0x00000000;
    FMC_Bank5_6->SDCMR |= 0x1 << 3 /* Set bank 2 */ | 0x2; /* Set mode 010 */

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));
    
    /* Issue command configuring number of auto refresh commands, the
     * IS42S16400J datasheet specifies a minimum of 2 */
    FMC_Bank5_6->SDCMR = 0x00000000;
    /* Set bank 2, num refresh commands 4*/
    FMC_Bank5_6->SDCMR |= (0x1 << 3) | ((4 - 1) << 5) | 0x3; /* Set mode 011 */

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

    /* Issue the "load mode register" command setting the burst length to 1 and
     * the CAS latency defined above. The ST example shows setting the write
     * burst mode to "single location access" */
    FMC_Bank5_6->SDCMR = 0x00000000;
    /* Set bank 2, burst length = 1, CAS latency = 3 */
    FMC_Bank5_6->SDCMR |= (0x1 << 3)
        | (((0x1 << 9)|(0x3 << 4)|(0x1 << 0)) << 9)
        | 0x4; /* Set mode 100 */
    
    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));
    
    /* Set the refresh rate counter. Counter value calculated as follows:
     * (64ms)/(4096 rows)*60Mhz - 20 (see the reference manual for this
     * calculation, well sort of; the 64ms comes from the SDRAM datasheet) */
    /* Reset Refresh timer register */
    /* Assign directly because zeroing beforehand forces a minimum count value
     * of 0x28, which will screw up a bitwise or. */
    FMC_Bank5_6->SDRTR = (((60000000/1000)*64)/4096 - 20) << 1;

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));
}

/* Similar to fmc_setup but uses a smaller clock divider to run the ram at
 * higher speed */
void __attribute__((optimize("O0"))) fmc_setup_fast(void)
{  
    uint32_t tmpr = 0;
    volatile uint32_t timeout;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN
        | RCC_AHB1ENR_GPIOCEN
        | RCC_AHB1ENR_GPIODEN
        | RCC_AHB1ENR_GPIOEEN
        | RCC_AHB1ENR_GPIOFEN
        | RCC_AHB1ENR_GPIOGEN;

    int gpiod_pins[] = {0,1,8,9,10,14,15,-1};
    fmc_gpio_config_helper(GPIOD,gpiod_pins);
    int gpioe_pins[] = {0,1,7,8,9,10,11,12,13,14,15,-1};
    fmc_gpio_config_helper(GPIOE,gpioe_pins);
    int gpiof_pins[] = {0,1,2,3,4,5,11,12,13,14,15,-1};
    fmc_gpio_config_helper(GPIOF,gpiof_pins);
    int gpiog_pins[] = {0,1,4,5,8,15,-1};
    fmc_gpio_config_helper(GPIOG,gpiog_pins);
    int gpiob_pins[] = {5,6,-1};
    fmc_gpio_config_helper(GPIOB,gpiob_pins);
    int gpioc_pins[] = {0,-1};
    fmc_gpio_config_helper(GPIOC,gpioc_pins);

    /* Enable FMC clock */
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;
    
    /* Configure SDRAM */
    /* SDCLK RBURST and RPIPE must be programmed in bank 1's configuration
     * register */
    /* Reset */
    FMC_Bank5_6->SDCR[0] = 0x00000000;
    /* Read pipe delay = 2,
     * Manage single reads as bursts
     * SDRAMCLK divider = 2 (HCLK = 180Mhz, SDRAMCLK = 90Mhz)*/
    FMC_Bank5_6->SDCR[0] |=  (0x2 << 13)
                            |(0x1 << 12)
                            |(0x2 << 10);
    /* CAS latency = 3 cycles
     * Number of banks = 4
     * Memory data bus width = 16bits
     * Row addressing = 12bits
     * Column addressing = 8 bits
     */
    FMC_Bank5_6->SDCR[1] = 0x00000000;
    FMC_Bank5_6->SDCR[1] |= (0x3 << 7)
                            |(0x1 << 6)
                            |(0x1 << 4)
                            |(0x1 << 2)
                            |(0x0 << 0);

    /* Set up timing on SDRAM bank 2 */
    FMC_Bank5_6->SDTR[0] = 0x00000000;
    /* TRP and TRC must be programmed in bank 1's timing register */
    FMC_Bank5_6->SDTR[0] |=  ((2 - 1) << 20)
                            |((6 - 1) << 12);
    /* Reset */
    FMC_Bank5_6->SDTR[1] = 0x00000000;
    /* Load to active delay (TMRD) = 2CLK -> 2 cycles
     * Exit self refresh delay (TXSR) = min 70ns -> ~7 cycles
     * Self refresh time (TRAS) = min 42ns -> ~4 cycles
     * Row cycle delay (TRC) = min 63ns -> ~6 cycles
     * Write recovery time (TWR) = 2CLK -> 2 cycles
     * Row precharge delay (TRP) = 15ns -> ~2 cycle
     * Row column delay (TRCD) = 15ns -> ~2 cycle
     */
    FMC_Bank5_6->SDTR[1] |=  ((2 - 1) << 24)
                            |((2 - 1) << 16)
                            |((4 - 1) << 8)
                            |((7 - 1) << 4)
                            |((2 - 1) << 0);

    /* Wait 100ms */
    timeout = 10000000;
    while (timeout--);

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

    /* Issue clock configuration command to bank 2 */
    FMC_Bank5_6->SDCMR = 0x00000000;
    FMC_Bank5_6->SDCMR |= 0x1 << 3 /* Set bank 2 */ | 0x1; /* Set mode 001 */

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

    /* Issue precharge all command to bank 2 */
    FMC_Bank5_6->SDCMR = 0x00000000;
    FMC_Bank5_6->SDCMR |= 0x1 << 3 /* Set bank 2 */ | 0x2; /* Set mode 010 */

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));
    
    /* Issue command configuring number of auto refresh commands, the
     * IS42S16400J datasheet specifies a minimum of 2 */
    FMC_Bank5_6->SDCMR = 0x00000000;
    /* Set bank 2, num refresh commands 2*/
    FMC_Bank5_6->SDCMR |= (0x1 << 3) | ((2 - 1) << 5) | 0x3; /* Set mode 011 */

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

    /* Issue the "load mode register" command setting the burst length to "don't
     * care" and
     * the CAS latency defined above. Set
     * burst mode to "single location access" */
    FMC_Bank5_6->SDCMR = 0x00000000;
    /* Set bank 2, burst length = 1, CAS latency = 3 */
    FMC_Bank5_6->SDCMR |= (0x1 << 3)
        | (((0x1 << 9)|(0x3 << 4)|(0x0 << 0)) << 9)
        | 0x4; /* Set mode 100 */
    
    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));
    
    /* Set the refresh rate counter. Counter value calculated as follows:
     * (64ms)/(4096 rows)*60Mhz - 20 (see the reference manual for this
     * calculation, well sort of; the 64ms comes from the SDRAM datasheet) */
    /* Reset Refresh timer register */
    /* Assign directly because zeroing beforehand forces a minimum count value
     * of 0x28, which will screw up a bitwise or. */
    FMC_Bank5_6->SDRTR = ((((HCLK/2)/1000)*64)/4096 - 20) << 1;

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));
}

uint32_t __attribute__((optimize("O3"))) fmc_read_write_test(void)
{
    uint32_t count;
    uint32_t *dest = (uint32_t*)SDRAM_BANK_ADDR;
    /* zero memory */
    for (count = 0; count < (SDRAM_LENGTH/4); count++) {
        *dest++ = 0;
    }
    /* put increments in memory */
    dest = (uint32_t*)SDRAM_BANK_ADDR;
    for (count = 0; count < (SDRAM_LENGTH/4); count++) {
        *dest++ = count;
    }
    /* check that it worked */
    dest = (uint32_t*)SDRAM_BANK_ADDR;
    for (count = 0; count < (SDRAM_LENGTH/4); count++) {
        if (count != *dest) {
            /* didn't work */
            return count;
        }
        dest++;
    }
    return 0;
}

uint32_t __attribute__((optimize("O3"))) fmc_read_write_test_short(void)
{
    uint32_t count;
    uint32_t *dest = (uint32_t*)SDRAM_BANK_ADDR;
    /* zero memory */
    for (count = 0; count < (SDRAM_LENGTH_SHORT/4); count++) {
        *dest++ = 0;
    }
    /* put increments in memory */
    dest = (uint32_t*)SDRAM_BANK_ADDR;
    for (count = 0; count < (SDRAM_LENGTH_SHORT/4); count++) {
        *dest++ = count;
    }
    /* check that it worked */
    dest = (uint32_t*)SDRAM_BANK_ADDR;
    for (count = 0; count < (SDRAM_LENGTH_SHORT/4); count++) {
        if (count != *dest) {
            /* didn't work */
            return count;
        }
        dest++;
    }
    return 0;
}

uint32_t __attribute__((optimize("O3"))) sram_read_write_test_short(void)
{
    uint32_t count;
    uint32_t *dest = (uint32_t*)sram_data;
    /* zero memory */
    for (count = 0; count < (SDRAM_LENGTH_SHORT/4); count++) {
        *dest++ = 0;
    }
    /* put increments in memory */
    dest = (uint32_t*)sram_data;
    for (count = 0; count < (SDRAM_LENGTH_SHORT/4); count++) {
        *dest++ = count;
    }
    /* check that it worked */
    dest = (uint32_t*)sram_data;
    for (count = 0; count < (SDRAM_LENGTH_SHORT/4); count++) {
        if (count != *dest) {
            /* didn't work */
            return count;
        }
        dest++;
    }
    return 0;
}
