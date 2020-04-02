#include "stdint.h"
#include "bitmap.h"
#include "malloc.h"
#include "print.h"

void memory_init(){
    uint32_t *mm_size_p = 0x90000;
    uint32_t  bytes = *mm_size_p - MM_BUFFER_BASE_ADDRESS;
    uint32_t  pages = bytes / PAGE_SIZE;

    bitmap.size = pages;
    bitmap.bits = BITMAP_BASE_ADDRESS;

    mm_buffer.bitmap = bitmap;
    mm_buffer.bytes  = bytes;
    mm_buffer.base_address = MM_BUFFER_BASE_ADDRESS;

    bitmap_init(&mm_buffer.bitmap);
}



void* malloc(uint32_t size){
    if(size <= 0){
        return NULL;
    }
    uint32_t bit_index = bitmap_malloc(&mm_buffer.bitmap, size);
    if(bit_index == -1){
        return NULL;
    }
    uint32_t des = (uint32_t)mm_buffer.base_address + PAGE_SIZE * bit_index;
    return (void*)des;
}


