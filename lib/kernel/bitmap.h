#ifndef _LIB_KERNEL_BITMAP_H
#define _LIB_KERNEL_BITMAP_H
#include "stdint.h"

#define BIT_FREE            0
#define BIT_OCCUPIED        1        

typedef struct bitmap{
    uint32_t size;
    uint8_t* bits;
} BITMAP;


void bitmap_init(BITMAP* bitmap);
bool bit_status(BITMAP* bitmap, uint32_t bit_index);
int bitmap_malloc(BITMAP* bitmap, uint32_t bit_count);
void bit_set(BITMAP* bitmap, uint8_t value);
#endif
