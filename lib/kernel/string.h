#ifndef _LIB_KERNEL_STRING_H
#include "stdint.h"


void memory_set(uint8_t* dest, uint32_t size, uint8_t value);
void memset(uint8_t* dest, uint32_t size, uint8_t value);
int8_t* memcpy(int8_t *dest, int8_t *src, uint32_t bytes);

uint32_t strlen(int8_t *str);
int8_t* strcpy(int8_t *dest, int8_t *src);
int8_t* str_reverse(int8_t *dest);

#endif