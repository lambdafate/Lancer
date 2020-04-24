#ifndef _KERNEL_GLOBAL_H
#define _KERNEL_GLOBAL_H
#include "stdint.h"

#define KERNEL_STACKTOP                       0xc00a0000

#define RPL0                                  0
#define RPL1                                  1
#define RPL2                                  2
#define RPL3                                  3
#define TI_GDT                                0
#define TI_LDT                                1 

#define GDT_ADDRESS                           0xc0080000
#define GDT_SIZE                              9
#define GDT_Limit                             (GDT_SIZE*8-1)

// gdt index
#define GDT_INDEX_KERNEL_NULL                 0
#define GDT_INDEX_KERNEL_CODE                 1
#define GDT_INDEX_KERNEL_SCREEN               2
#define GDT_INDEX_KERNEL_DATA                 3
#define GDT_INDEX_KERNEL_STACK                4
#define GDT_INDEX_KERNEL_TSS                  5
#define GDT_INDEX_USER_CODE                   6   
#define GDT_INDEX_USER_DATA                   7
#define GDT_INDEX_USER_STACK                  8

// gdt selectors
#define GDT_SELECTOR_KERNEL_NULL              (GDT_INDEX_KERNEL_NULL << 3)
#define GDT_SELECTOR_KERNEL_CODE              (GDT_INDEX_KERNEL_CODE << 3)
#define GDT_SELECTOR_KERNEL_SCREEN            (GDT_INDEX_KERNEL_SCREEN << 3)  
#define GDT_SELECTOR_KERNEL_DATA              (GDT_INDEX_KERNEL_DATA << 3)
#define GDT_SELECTOR_KERNEL_STACK             (GDT_INDEX_KERNEL_STACK << 3)
#define GDT_SELECTOR_KERNEL_TSS               (GDT_INDEX_KERNEL_TSS << 3) 
#define GDT_SELECTOR_USER_CODE                ((GDT_INDEX_USER_CODE << 3) | RPL3)
#define GDT_SELECTOR_USER_DATA                ((GDT_INDEX_USER_DATA << 3) | RPL3)
#define GDT_SELECTOR_USER_STACK               ((GDT_INDEX_USER_STACK << 3) | RPL3)


// global descriptor
typedef struct{
    uint32_t limit_low : 16;
    uint32_t base_low  : 24;
    uint32_t type      :  4;
    uint32_t p_dpl_s   :  4;
    uint32_t limit_high:  4;
    uint32_t attr      :  4;
    uint32_t base_high :  8;
}__attribute((packed)) global_descriptor;

typedef struct{
    uint32_t backlink;
    uint32_t esp0, ss0;
    uint32_t esp1, ss1;
    uint32_t esp2, ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp;
    uint32_t esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint32_t trace_bitmap;
    uint32_t ssp;
}TSS;                                       // 27 * 4 = 108bytes

#define TSS_ADDRESS                          0xc008f000
#define TSS_LIMIT                            (sizeof(TSS)-1)


void write_tss(uint32_t esp0);
void set_tss();
void flush_tss();

#endif