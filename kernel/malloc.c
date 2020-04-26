#include "stdint.h"
#include "bitmap.h"
#include "malloc.h"
#include "print.h"
#include "page.h"
#include "debug.h"

// BITMAP    p_bitmap;
MM_BUFFER mm_buffer;

void memory_init(){
    uint32_t *mm_size_p = (uint32_t*)(0x90000);            // setup.asm store memory size here
    uint32_t  avail_bytes = *mm_size_p - MM_BUFFER_BASE_ADDRESS;
    uint32_t  avail_pages = avail_bytes / PAGE_SIZE;

    mm_buffer.bitmap.size = avail_pages;
    mm_buffer.bitmap.bits = (uint8_t*)BITMAP_BASE_ADDRESS;

    mm_buffer.bytes  = *mm_size_p;
    mm_buffer.base_address = MM_BUFFER_BASE_ADDRESS;

    bitmap_init(&mm_buffer.bitmap);
}


// just malloc a 4k page when page fault happens.
void* pmalloc(){
    int32_t bit_index = bitmap_malloc(&mm_buffer.bitmap, 1);
    // ASSERT(bit_index != -1);
    if(bit_index == -1){
        return NULL;
    }
    void *page = (void*)(mm_buffer.base_address + bit_index * PAGE_SIZE);
    return page;
}

// it will molloc a 4K page(virtual) from kernel's first page table
void* kmalloc(){
    // page_table_t *pdt = get_pdt();
    uint32_t pdt_index = 768;
    page_table_t *pet = get_pet(pdt_index);
    uint32_t pet_index = 256;   // 0~255 <=> 0~1MB
    for (; pet_index < 1024; pet_index++){
        if(pet->pages[pet_index].present == 0){
            break;
        }
    }
    ASSERT(pet_index != 1024);
    return get_page_chunk(pdt_index, pet_index);
}




// malloc 4k virtual pages by searching pdt and pet.
// use in kernel mode.
void* vmalloc(uint32_t size){
    uint32_t page_num = size / 4096 + (size % 4096 ? 1: 0);
    // printk("pahe_num: %x,  sector: %x, size: %x\n", page_num, size/512, size);
    ASSERT(page_num > 0 && page_num < 1024);

    page_table_t *pdt = get_pdt();
    uint32_t pdt_index, pet_index = 0, free = 0;
    for(pdt_index = 769; pdt_index < 1024; pdt_index++){ 
        if(pdt->pages[pdt_index].present == 0){
            break;
        }
        page_table_t *pet = get_pet(pdt_index);
        for(pet_index = 0; pet_index < 1024; pet_index++){
            if(pet->pages[pet_index].present == 1){
                continue;
            }
            uint32_t i = pet_index + 1;
            free = 1;
            while(free < page_num && i < 1024 && pet->pages[i].present == 0){
                i++;
                free++;
            }
            if(free == page_num){
                break;
            }
            free = 0;
            pet_index = i;
        }
        if(free == page_num){
            break;
        }
    }
    ASSERT(pdt_index != 1024);
    page_table_t *pet = get_pet(pdt_index);
    for(uint32_t i = 0; i < page_num; i++){
        uint32_t just_page_fault = *(uint32_t*)(get_page_chunk(pdt_index, pet_index+i));
    }
    return get_page_chunk(pdt_index, pet_index);
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


