#include "stdint.h"
#include "page.h"
#include "schedule.h"
#include "print.h"
#include "debug.h"
#include "malloc.h"

static uint32_t get_pdt_index(void *linear);
static uint32_t get_pet_index(void *linear);



void handler_page_fault(uint8_t vector){
    void *linear = NULL;
    asm volatile("movl %%cr2, %%eax":"=a"(linear));

    printk("\npage fault happens!\n");
    printk("cause linear address: ");
    put_hex(linear);
    printk("  error code: ");
    put_hex(current_task->stackframe.error_code);
    printk("\n");

    page_table_t *pdt = (page_table_t*)((1023 << 22) + (1023 << 12));
    uint32_t pdt_index = get_pdt_index(linear);
    if(pdt->pages[pdt_index].present == 0){
        printk("no page entry table present in page dir table!\n");
    }else{
        printk("no 4k page present in page entry table. i will find one!\n");
        page_table_t *pet = (page_table_t*)((1023 << 22) + (pdt_index << 12));
        uint32_t pet_index = get_pet_index(linear);
        
        pet->pages[pet_index].present = 1;
        pet->pages[pet_index].rw      = 1;
        pet->pages[pet_index].us      = 1;
        pet->pages[pet_index].frame_address = (uint32_t)pmalloc() >> 12;
        printk("malloc page address: %x\n", pet->pages[pet_index].frame_address<<12);
        return;
    }
    while(1){}
}


static uint32_t get_pdt_index(void *linear){
    ASSERT(linear >=0 && linear <= 0xffffffff);
    uint32_t index = (uint32_t)(linear) >> 22;
    return index;
}

static uint32_t get_pet_index(void *linear){
    ASSERT(linear >= 0 && linear <= 0xffffffff);
    uint32_t index = ((uint32_t)(linear) >> 12) & 0x003ff;
    return index;
}




// just for a test
void show_page_map(){
    page_table_t *pdt = (page_table_t*)((1023 << 22) + (1023 << 12));
    int pdt_index = 0;
    while(pdt_index < 1024){
        if(pdt->pages[pdt_index].present == 0 || pdt_index == 0){
            pdt_index++;
            continue;
        }
        put_int(pdt_index);
        put_str("       ");
        put_hex(pdt->pages[pdt_index].frame_address << 12);
        put_str("  :\n");
        page_table_t *pet = (page_table_t*)((1023 << 22) + (pdt_index << 12));
        int pet_index = 0;
        while(pet_index < 1024){
            if(pet->pages[pet_index].present == 0){
                pet_index++;
                continue;
            }
            put_str("(");
            put_hex((pdt_index << 22) + (pet_index << 12));
            put_str("--->");
            put_hex(pet->pages[pet_index].frame_address << 12);
            put_str(")");
            pet_index++;
            if(pet_index % 3 == 0){
                put_str("\n");
            }else{
                put_str(" ");
            }
        }
        break;
        put_char('\n');
    }


    while(1){}
}