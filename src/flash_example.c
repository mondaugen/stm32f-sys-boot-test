#include "flash_example.h" 
#include "stm32f4xx.h" 

#define FLASH_START_ADDR 0x0810c000
#define FLASH_END_ADDR   0x08110000 
#define FLASH_SECTOR     15
/* The number of bytes that can be read and written at one time */
#define FLASH_ACCESS_SIZE 4 

/* Reads size bytes of data out of flash into dest. Size should be a multiple of
 * FLASH_ACCESS_SIZE.  This is perhaps obvious, but the memory areas mustn't
 * overlap. */
int flash_read(char *dest, size_t size)
{
    if (size % FLASH_ACCESS_SIZE) {
        return -1; /* Must be multiple of FLASH_ACCESS_SIZE */
    }
    size_t n;
    for (n = 0; n < size; n += FLASH_ACCESS_SIZE) {
        /* wait for flash to be free */
        while (FLASH->SR & FLASH_SR_BSY);
#if FLASH_ACCESS_SIZE == 4
        *((uint32_t*)dest) = *((uint32_t*)(FLASH_START_ADDR + n));
#else
 #error "Bad value for FLASH_ACCESS_SIZE."
#endif
        dest += FLASH_ACCESS_SIZE;
    }
    return 0;
}

void flash_erase(void)
{
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    /* Unlock flash */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xcdef89ab;
    /* Set sector erase bit and select sector */
    FLASH->CR &= ~FLASH_CR_SNB;
#if FLASH_SECTOR >= 12
    FLASH->CR |= ((0x1 << 4) | (FLASH_SECTOR - 12)) << 3;
#else
    FLASH->CR |= FLASH_SECTOR << 3;
#endif  
    FLASH->CR |= FLASH_CR_SER;
    /* Start erasing */
    FLASH->CR |= FLASH_CR_STRT;
    /* Wait to be finished */
    while (FLASH->CR & FLASH_SR_BSY);
    /* Reset sector erase bit */
    FLASH->CR &= ~FLASH_CR_SER;
    /* Lock flash */
    FLASH->CR |= FLASH_CR_LOCK;
}

/* Writes size bytes of data into flash. Erase should be called before this
 * unless this is the first time flash has been written since the last erase.
 * Size should be a multiple of FLASH_ACCESS_SIZE */
int flash_write(char *src, size_t size)
{
    if (size % FLASH_ACCESS_SIZE) {
        return -1; /* Must be multiple of FLASH_ACCESS_SIZE */
    }
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    /* Unlock flash */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xcdef89ab;
    /* Set program bit */
    FLASH->CR |= FLASH_CR_PG;
    size_t n;
    for (n = 0; n < size; n += FLASH_ACCESS_SIZE) {
        /* wait for flash to be free */
        while (FLASH->SR & FLASH_SR_BSY);
#if FLASH_ACCESS_SIZE == 4
        *((uint32_t*)(FLASH_START_ADDR + n)) = *((uint32_t*)src);
#else
 #error "Bad value for FLASH_ACCESS_SIZE."
#endif
        src += FLASH_ACCESS_SIZE;
    }
    /* Reset program bit */
    FLASH->CR &= ~FLASH_CR_PG;
    /* lock that flash */
    FLASH->CR |= FLASH_CR_LOCK;
    return 0;
}

void flash_setup(void)
{
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    /* Unlock flash */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xcdef89ab;
    /* Reset program size register */
    FLASH->CR &= ~FLASH_CR_PSIZE;
#if FLASH_ACCESS_SIZE == 4
    FLASH->CR = 0x2 << 8;
#else
 #error "Bad value for FLASH_ACCESS_SIZE."
#endif
    /* lock that flash */
    FLASH->CR |= FLASH_CR_LOCK;
}

void simple_flash_write(void)
{
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    /* Unlock flash */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xcdef89ab;
    /* Set program bit */
    FLASH->CR |= FLASH_CR_PG;
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    *((uint32_t*)FLASH_START_ADDR) = 0xdeadbeef;
}

void long_flash_write(void)
{
    /* Setup indicator LEDS */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    GPIOG->MODER |= (1 << (14*2)) | (1 << (13*2));
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    /* Unlock flash */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xcdef89ab;
    /* Set program bit */
    FLASH->CR |= FLASH_CR_PG;
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    GPIOG->ODR |= (1 << 13);
    uint32_t *ptr, val;
    val = 0;
    ptr = (uint32_t*)FLASH_START_ADDR;
    while (ptr < (uint32_t*)FLASH_END_ADDR) {
        *ptr++ = val++;
    }
    /* wait for flash to be free */
    while (FLASH->SR & FLASH_SR_BSY);
    /* Reset program bit */
    FLASH->CR &= ~FLASH_CR_PG;
    /* lock that flash */
    FLASH->CR |= FLASH_CR_LOCK;
    GPIOG->ODR |= (1 << 14);
}

void flash_dma_write(void)
{
}
