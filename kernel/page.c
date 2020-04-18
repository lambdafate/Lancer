#include "stdint.h"
#include "page.h"
#include "print.h"
#include "debug.h"
#include "malloc.h"
#include "string.h"
#include "schedule.h"

uint32_t get_pdt_index(void *linear);
uint32_t get_pet_index(void *linear);
void init_page(page_t* page, uint8_t _rw, uint8_t _us, void* linear);
void init_default_page(page_t *page, void* linear);
static void init_page_from_page(page_t *new_page, page_t *old_page);

void* get_page_chunk(uint32_t pdt_index, uint32_t pet_index);
page_table_t* get_pdt();
page_table_t* get_pet(uint32_t pdt_index);



void handler_page_fault(uint8_t vector){
    void *linear = NULL;
    asm volatile("movl %%cr2, %%eax":"=a"(linear));

    printk("\ncurrent-task: %s\n", current_task->name);
    printk("page fault happens!\n");
    printk("cause linear address: ");
    put_hex(linear);
    printk("  error code: ");
    put_hex(current_task->stackframe.error_code);
    printk("\n");

    page_table_t *pdt = get_pdt();
    uint32_t pdt_index = get_pdt_index(linear);
    uint32_t pet_index = get_pet_index(linear);
    if(pdt_index == 1023){
        pdt_index = pet_index;
        ASSERT(pdt->pages[pdt_index].present == 0);
    }
    page_table_t *pet = get_pet(pdt_index);

    void *frame = pmalloc();
    printk("pdt_index: %x, pet_index: %x\n", pdt_index, pet_index);
    
    if(pdt->pages[pdt_index].present == 0){
        // if(pdt_index == 1023){
        //     pdt_index = get_pet_index(linear);
        //     pet = get_pet(pdt_index);
        // }
        printk("no page entry table present in page dir table!\n");
        // void *frame = pmalloc();
        init_default_page(&pdt->pages[pdt_index], frame);
        memory_set((void*)(pet), PAGE_SIZE, 0);

        // pdt->pages[pdt_index].present = 1;
        // pdt->pages[pdt_index].rw      = 1;
        // pdt->pages[pdt_index].us      = 1;
        // pdt->pages[pdt_index].frame_address = (uint32_t)pmalloc() >> 12;
        // memory_set((void*)((1023<<22) + (pdt_index<<12)), PAGE_SIZE, 0);
    }else if(pet->pages[pet_index].present == 0){
        printk("no 4k page present in page entry table. i will find one!\n");
        // page_table_t *pet = (page_table_t*)((1023 << 22) + (pdt_index << 12));
    
        // uint32_t pet_index = get_pet_index(linear);
        
        init_default_page(&pet->pages[pet_index], frame);

        // pet->pages[pet_index].present = 1;
        // pet->pages[pet_index].rw      = 1;
        // pet->pages[pet_index].us      = 1;
        // pet->pages[pet_index].frame_address = (uint32_t)pmalloc() >> 12;
        // printk("malloc page address: %x\n", pet->pages[pet_index].frame_address<<12);
    }
    printk("malloc page address: %x\n", frame);
    // while(1){}
}

// init (user mode)task's pdt, it will be load to cr3 register
int32_t set_task_pdt(TASK *task){
    page_table_t *pdt_curr = get_pdt();
    page_table_t *pdt_task = (page_table_t*)vmalloc();
    uint32_t just_page_fault = *((uint32_t*)(pdt_task));
    
    // init task's pdt.
    // memory_set((void*)pdt_task, PAGE_SIZE, 0);
    // init_page_from_page(&pdt_task->pages[768], &pdt_curr->pages[768]);
    // pdt_task->pages[1023].present = 1;
    for(uint32_t pdt_index = 0; pdt_index < 1024; pdt_index++){
        // if(pdt_index < 768){
        //     pdt_task->pages[pdt_index].present = 0;
        //     continue;
        // }
        init_page_from_page(&pdt_task->pages[pdt_index], &pdt_curr->pages[pdt_index]);
    }
    pdt_task->pages[0].present = 0;
    // set 1023(=pdt_index) to task self's pdt.
    uint32_t pdt_index = get_pdt_index(pdt_task);
    uint32_t pet_index = get_pet_index(pdt_task);
    page_table_t *pet = get_pet(pdt_index);
    pdt_task->pages[1023].frame_address = pet->pages[pet_index].frame_address;
    task->pdt = (uint32_t)(((uint32_t)(pet->pages[pet_index].frame_address)) << 12);
    return 1;
}
  


uint32_t get_pdt_index(void *linear){
    ASSERT(linear >=0 && linear <= 0xffffffff);
    uint32_t index = (uint32_t)(linear) >> 22;
    return index;
}

uint32_t get_pet_index(void *linear){
    ASSERT(linear >= 0 && linear <= 0xffffffff);
    uint32_t index = ((uint32_t)(linear) >> 12) & 0x003ff;
    return index;
}



void init_page(page_t* page, uint8_t _rw, uint8_t _us, void* frame){
    ASSERT(_rw == 0 || _rw == 1);
    ASSERT(_us == 0 || _us == 1);

    page->present = 1;
    page->rw      = _rw;
    page->us      = _us;
    page->reserved = 0;
    page->accessed = 0;
    page->dirty    = 0;
    page->unused   = 0;
    page->frame_address = (uint32_t)frame >> 12;
}
void init_default_page(page_t *page, void* frame){
    init_page(page, 1, 1, frame);
}

static void init_page_from_page(page_t *new_page, page_t *old_page){
    new_page->present = old_page->present;
    new_page->rw      = old_page->rw;
    new_page->us      = old_page->us;
    new_page->reserved= old_page->reserved;
    new_page->accessed= old_page->accessed;
    new_page->dirty   = old_page->dirty;
    new_page->unused  = old_page->unused;
    new_page->frame_address = old_page->frame_address;
}




void* get_page_chunk(uint32_t pdt_index, uint32_t pet_index){
    ASSERT(pdt_index >= 0 && pdt_index < 1024);
    ASSERT(pet_index >= 0 && pet_index < 1024);

    return (void*)((pdt_index << 22) + (pet_index << 12)); 
}

page_table_t* get_pdt(){
    page_table_t *pdt = get_page_chunk(1023, 1023);
    return pdt;
}

page_table_t* get_pet(uint32_t pdt_index){
    page_table_t *pdt = get_pdt();
    // ASSERT(pdt->pages[pdt_index].present == 1);
    page_table_t *pet = get_page_chunk(1023, pdt_index);
    return pet;
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