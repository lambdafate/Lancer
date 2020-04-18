#ifndef _KERNEL_MALLOC_H
#define _KERNEL_MALLOC_H
#include "bitmap.h"

#define BITMAP_BASE_ADDRESS         0x00060000
#define MM_BUFFER_BASE_ADDRESS      0x00a00000   // malloc from 10MB
#define PAGE_SIZE   4096

typedef struct memory_buffer{
    BITMAP bitmap;
    uint32_t bytes;                             // all momery bytes 
    uint32_t base_address;                         // malloc pages in here
}MM_BUFFER; 


void* malloc(uint32_t size);
void  memory_init();


void* pmalloc();
void* kmalloc();
#endif