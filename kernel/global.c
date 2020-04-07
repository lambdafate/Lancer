#include "stdint.h"
#include "global.h"
#include "debug.h"

TSS *tss = (TSS*)(TSS_ADDRESS);
global_descriptor *gdt = (global_descriptor*)(GDT_ADDRESS);

// set bss's ss0 and esp0
void write_tss(uint32_t esp0){
    tss->ss0  = GDT_SELECTOR_KERNEL_STACK;
    tss->esp0 = esp0;
}


// set tss descriptor on gdt
void set_tss(){
    ASSERT(TSS_LIMIT >= 103 && TSS_LIMIT <= 0xffff);
    ASSERT(TSS_ADDRESS <= 0xffffff);

    global_descriptor *tss_des = gdt + GDT_INDEX_KERNEL_TSS;
    tss_des->limit_low = TSS_LIMIT;     
    tss_des->base_low  = TSS_ADDRESS;
    tss_des->type      = 1001;              // gdt: type
    tss_des->p_dpl_s   = 1000;              // gdt: p dpl s
    tss_des->limit_high=    0;
    tss_des->attr      = 0100;
    tss_des->base_high =    0;
}

// load tss descriptor to tr
void flush_tss(){
    asm volatile("ltr %%ax"::"a"(GDT_SELECTOR_KERNEL_TSS));
}