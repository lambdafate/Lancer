#ifndef _LIB_KERNEL_STRING_H
#include "stdint.h"


void memory_set(uint8_t* dest, uint32_t size, uint8_t value);
uint32_t strlen(uint8_t *str);
uint8_t* strcpy(uint8_t *dest, uint8_t *src);

#endif