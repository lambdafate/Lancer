#ifndef _KERNEL_PAGE_H
#define _KERNEL_PAGE_H
#include "stdint.h"


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



#endif