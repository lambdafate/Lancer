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

// malloc a 4k virtual page by searching pdt and pet.
void* vmalloc(){
    page_table_t *pdt = get_pdt();
    uint32_t pdt_index = 0, pet_index = 0;
    for(; pdt_index < 768; pdt_index++){    // 768(and above is system own)
        if(pdt->pages[pdt_index].present == 0){
            void *linear = get_page_chunk(pdt_index, 0);
            uint32_t just_page_fault = *((uint32_t*)linear);
            printk("\n****fuck********\n");
            return linear;
        }
        page_table_t *pet = get_pet(pdt_index);
        for(pet_index = 0; pet_index < 1024; pet_index++){
            if(pet->pages[pet_index].present == 1){
                continue;
            }
            printk("########  vmalloc: %x,  %x\n", pdt_index, pet_index);
            // we find it.
            void *linear = get_page_chunk(pdt_index, pet_index);
            printk("begin page-fault.\n");
            uint32_t just_page_fault = *((uint32_t*)linear);
            printk("\npage-fault finish\n");
            return linear;
        }
    }
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


