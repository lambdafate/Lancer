#ifndef _KERNEL_MALLOC_H
#define _KERNEL_MALLOC_H
#include "bitmap.h"

#define BITMAP_BASE_ADDRESS         0x7c00
#define MM_BUFFER_BASE_ADDRESS      0x00a00000   // malloc from 10MB
#define PAGE_SIZE   4096

typedef struct memory_buffer{
    BITMAP bitmap;
    uint32_t bytes;
    void* base_address;
}MM_BUFFER; 

BITMAP    bitmap;
MM_BUFFER mm_buffer;
void* malloc(uint32_t size);
void  memory_init();
#endif