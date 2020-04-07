#include "stdint.h"
#include "global.h"
#include "debug.h"





// set tss descriptor
void write_tss(){
    ASSERT(TSS_LIMIT >= 103 && TSS_LIMIT <= 0xffff);
    ASSERT(TSS_ADDRESS <= 0xffffff);

    global_descriptor *tss = gdt + GDT_INDEX_KERNEL_TSS;
    tss->limit_low = TSS_LIMIT;     
    tss->base_low  = TSS_ADDRESS;
    tss->type      = 1001;              // gdt: type
    tss->p_dpl_s   = 1000;              // gdt: p dpl s
    tss->limit_high=    0;
    tss->attr      = 0100;
    tss->base_high =    0;
}

// load tss descriptor to tr
void flush_tss(){
    asm volatile("ltr %%ax"::"a"(GDT_SELECTOR_KERNEL_TSS));
}