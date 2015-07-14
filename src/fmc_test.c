#include "fmc_test.h" 
/* All values are derived from the IS42S16400J datasheet */

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
        gpio->MODER |= 0x2 << (2 * (*pins));
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
    /* RBURST and RPIPE must be programmed in bank 1's configuration register */
    /* Reset */
    FMC_Bank5_6->SDCR[0] = 0x00000000;
    /* Read pipe delay = 1 (from ST's example),
     * No bursts */
    FMC_Bank5_6->SDCR[0] |=  (0x1 << 13)
                            |(0x0 << 12);
    /* HCLK = 180Mhz
     * SDRAMCLK divider = 3
     * SDRAMCLK = 60Mhz
     * CAS latency = 3 cycles
     * Number of banks = 4
     * Memory data bus width = 16bits
     * Row addressing = 12bits
     * Column addressing = 8 bits
     */
    FMC_Bank5_6->SDCR[1] = 0x00000000;
    FMC_Bank5_6->SDCR[1] |=  (0x3 << 10)
                            |(0x3 << 7)
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
    FMC_Bank5_6->SDRTR = 0x00000000;
    FMC_Bank5_6->SDRTR |= (((60000000/1000)*64)/4096 - 20) << 1;

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

//    /* Step 2 ----------------------------------------------------*/
//
//    /* FMC SDRAM control configuration */
//    FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank2_SDRAM;
//
//    /* Row addressing: [7:0] */
//    FMC_SDRAMInitStructure.FMC_ColumnBitsNumber   = FMC_ColumnBits_Number_8b;
//    /* Column addressing: [10:0] */
//    FMC_SDRAMInitStructure.FMC_RowBitsNumber      = FMC_RowBits_Number_12b;
//    FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth  = SDRAM_MEMORY_WIDTH;
//    FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
//    /* CL: Cas Latency = 3 clock cycles */
//    FMC_SDRAMInitStructure.FMC_CASLatency         = FMC_CAS_Latency_3; 
//    FMC_SDRAMInitStructure.FMC_WriteProtection    = FMC_Write_Protection_Disable;
//    FMC_SDRAMInitStructure.FMC_SDClockPeriod      = SDCLOCK_PERIOD;  
//    FMC_SDRAMInitStructure.FMC_ReadBurst          = FMC_Read_Burst_Disable;
//    FMC_SDRAMInitStructure.FMC_ReadPipeDelay      = FMC_ReadPipe_Delay_1;
//    FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct  = &FMC_SDRAMTimingInitStructure;
//    /* FMC SDRAM bank initialization */
//    FMC_SDRAMInit(&FMC_SDRAMInitStructure);
//
//    /* Step 3 --------------------------------------------------------------------*/
//    /* Configure a clock configuration enable command */
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_CLK_Enabled;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 1;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
//    /* Wait until the SDRAM controller is ready */ 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  
//
//    /* Step 4 --------------------------------------------------------------------*/
//    /* Insert 100 ms delay */
//    Delay(10);
//
//    /* Step 5 --------------------------------------------------------------------*/
//    /* Configure a PALL (precharge all) command */ 
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_PALL;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 1;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
//
//    /* Wait until the SDRAM controller is ready */  
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
//
//    /* Step 6 --------------------------------------------------------------------*/
//    /* Configure a Auto-Refresh command */ 
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_AutoRefresh;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 4;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
//
//    /* Wait until the SDRAM controller is ready */ 
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
//
//    /* Step 7 --------------------------------------------------------------------*/
//    /* Program the external memory mode register */
//    tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
//        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
//        SDRAM_MODEREG_CAS_LATENCY_3           |
//        SDRAM_MODEREG_OPERATING_MODE_STANDARD |
//        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
//
//    /* Configure a load Mode register command*/ 
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_LoadMode;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 1;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
//
//    /* Wait until the SDRAM controller is ready */ 
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
//
//    /* Step 8 --------------------------------------------------------------------*/
//
//    /* Set the refresh rate counter */
//    /* (7.81 us x Freq) - 20 */
//    /* Set the device refresh counter */
//    FMC_SetRefreshCount(REFRESH_COUNT);
//
//    /* Wait until the SDRAM controller is ready */ 
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    } 
}

int __attribute__((optimize("O0"))) fmc_read_write_test(void)
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
            return -1;
        }
        dest++;
    }
    return 0;
}

void __attribute__((optimize("O0"))) fmc_setup_st_example(void)
{  
    uint32_t tmpr = 0;
    uint32_t timeout;

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
    /* RBURST and RPIPE must be programmed in bank 1's configuration register */
    /* Reset */
    FMC_Bank5_6->SDCR[0] = 0x00000000;
    /* Read pipe delay = 1 (from ST's example),
     * No bursts */
    FMC_Bank5_6->SDCR[0] |=  (0x1 << 13)
                            |(0x0 << 12);
    /* HCLK = 180Mhz
     * SDRAMCLK divider = 3
     * SDRAMCLK = 60Mhz
     * CAS latency = 3 cycles
     * Number of banks = 4
     * Memory data bus width = 16bits
     * Row addressing = 12bits
     * Column addressing = 8 bits
     */
    FMC_Bank5_6->SDCR[1] = 0x00000000;
    FMC_Bank5_6->SDCR[1] |=  (0x3 << 10)
                            |(0x3 << 7)
                            |(0x1 << 6)
                            |(0x1 << 4)
                            |(0x1 << 2)
                            |(0x0 << 0);

    /* Set up timing on SDRAM bank 2 */
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
    /* TMRD = 2 cyc
     * TXSR = 7
     * TRAS = 4
     * TRC = 7
     * TWR = 2
     * TRP = 2
     * TRCD = 2 */
    FMC_Bank5_6->SDTR[1] |=  ((2 - 1) << 24)
                            |((2 - 1) << 16)
                            |((4 - 1) << 8)
                            |((9 - 1) << 4)
                            |((2 - 1) << 0);
    FMC_Bank5_6->SDTR[0] = 0x00000000;
    /* TRP and TRC must be programmed in bank 1's timing register */
    FMC_Bank5_6->SDTR[0] |=  ((2 - 1) << 20)
                            |((7 - 1) << 12);

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
    FMC_Bank5_6->SDRTR = 0x00000000;
    FMC_Bank5_6->SDRTR |= (((60000000/1000)*64)/4096 - 20) << 1;
//    FMC_Bank5_6->SDRTR |= ((uint32_t)0x0569) << 1;

    /* Wait 100us */
    timeout = SDRAM_TIMEOUT;
    while (timeout-- && (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY));

//    /* Step 2 ----------------------------------------------------*/
//
//    /* FMC SDRAM control configuration */
//    FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank2_SDRAM;
//
//    /* Row addressing: [7:0] */
//    FMC_SDRAMInitStructure.FMC_ColumnBitsNumber   = FMC_ColumnBits_Number_8b;
//    /* Column addressing: [10:0] */
//    FMC_SDRAMInitStructure.FMC_RowBitsNumber      = FMC_RowBits_Number_12b;
//    FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth  = SDRAM_MEMORY_WIDTH;
//    FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
//    /* CL: Cas Latency = 3 clock cycles */
//    FMC_SDRAMInitStructure.FMC_CASLatency         = FMC_CAS_Latency_3; 
//    FMC_SDRAMInitStructure.FMC_WriteProtection    = FMC_Write_Protection_Disable;
//    FMC_SDRAMInitStructure.FMC_SDClockPeriod      = SDCLOCK_PERIOD;  
//    FMC_SDRAMInitStructure.FMC_ReadBurst          = FMC_Read_Burst_Disable;
//    FMC_SDRAMInitStructure.FMC_ReadPipeDelay      = FMC_ReadPipe_Delay_1;
//    FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct  = &FMC_SDRAMTimingInitStructure;
//    /* FMC SDRAM bank initialization */
//    FMC_SDRAMInit(&FMC_SDRAMInitStructure);
//
//    /* Step 3 --------------------------------------------------------------------*/
//    /* Configure a clock configuration enable command */
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_CLK_Enabled;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 1;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
//    /* Wait until the SDRAM controller is ready */ 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  
//
//    /* Step 4 --------------------------------------------------------------------*/
//    /* Insert 100 ms delay */
//    Delay(10);
//
//    /* Step 5 --------------------------------------------------------------------*/
//    /* Configure a PALL (precharge all) command */ 
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_PALL;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 1;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
//
//    /* Wait until the SDRAM controller is ready */  
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
//
//    /* Step 6 --------------------------------------------------------------------*/
//    /* Configure a Auto-Refresh command */ 
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_AutoRefresh;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 4;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
//
//    /* Wait until the SDRAM controller is ready */ 
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
//
//    /* Step 7 --------------------------------------------------------------------*/
//    /* Program the external memory mode register */
//    tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
//        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
//        SDRAM_MODEREG_CAS_LATENCY_3           |
//        SDRAM_MODEREG_OPERATING_MODE_STANDARD |
//        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
//
//    /* Configure a load Mode register command*/ 
//    FMC_SDRAMCommandStructure.FMC_CommandMode            = FMC_Command_Mode_LoadMode;
//    FMC_SDRAMCommandStructure.FMC_CommandTarget          = FMC_Command_Target_bank2;
//    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber      = 1;
//    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
//
//    /* Wait until the SDRAM controller is ready */ 
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    }
//    /* Send the command */
//    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
//
//    /* Step 8 --------------------------------------------------------------------*/
//
//    /* Set the refresh rate counter */
//    /* (7.81 us x Freq) - 20 */
//    /* Set the device refresh counter */
//    FMC_SetRefreshCount(REFRESH_COUNT);
//
//    /* Wait until the SDRAM controller is ready */ 
//    timeout = SDRAM_TIMEOUT; 
//    while((FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
//    {
//        timeout--;
//    } 
}
