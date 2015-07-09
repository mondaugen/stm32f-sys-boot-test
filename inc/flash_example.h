#ifndef FLASH_EXAMPLE_H
#define FLASH_EXAMPLE_H 
#include <stddef.h>
int flash_read(char *dest, size_t size);
void flash_erase(void);
int flash_write(char *src, size_t size);
void flash_setup(void);
void simple_flash_write(void);
void long_flash_write(void);
#endif /* FLASH_EXAMPLE_H */
