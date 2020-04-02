#ifndef _LIB_KERNEL_BITMAP_H
#define _LIB_KERNEL_BITMAP_H
#include "stdint.h"

#define BIT_FREE            0
#define BIT_OCCUPIED        1        
#define BIT_MASK            1

typedef struct bitmap{
    uint32_t size;
    uint8_t* bits;
} BITMAP;


void bitmap_init(BITMAP* bitmap);
uint8_t bitmap_status(BITMAP* bitmap, uint32_t bit_index);
int32_t bitmap_malloc(BITMAP* bitmap, uint32_t bit_count);
void bitmap_set(BITMAP* bitmap, uint32_t bit_index, uint8_t value);
#endif
