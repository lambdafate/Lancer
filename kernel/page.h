#ifndef _KERNEL_PAGE_H
#define _KERNEL_PAGE_H
#include "stdint.h"
#include "schedule.h"

typedef struct{
    uint32_t present : 1;
    uint32_t rw      : 1;
    uint32_t us      : 1;       // if us=1, it's user-mode page.
    uint32_t reserved: 2;
    uint32_t accessed: 1;       // cpu use it.
    uint32_t dirty   : 1;       // cpu use it. only when this (dirty)page is in pet, not pdt.
    uint32_t unused  : 5;
    uint32_t frame_address : 20;
}__attribute((packed)) page_t;


typedef struct{
    page_t pages[1024];
}page_table_t;


void handler_page_fault(uint8_t vector);
void show_page_map();

uint32_t get_pdt_index(void *linear);
uint32_t get_pet_index(void *linear);
void init_page(page_t* page, uint8_t _rw, uint8_t _us, void* linear);
void init_default_page(page_t *page, void* linear);

void* get_page_chunk(uint32_t pdt_index, uint32_t pet_index);
page_table_t* get_pdt();
page_table_t* get_pet(uint32_t pdt_index);


int32_t set_task_pdt(TASK *task);

#endif