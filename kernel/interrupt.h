#ifndef _KERNEL_INTERRUPT_H
#define _KERNEL_INTERRUPT_H
#include "stdint.h"

#define IDT_NUM     33
#define PIC_M_CTRL  0x20        // intel 8259A interrupt
#define PIC_M_DATA  0x21        
#define PIC_S_CTRL  0xa0
#define PIC_S_DATA  0xa1

typedef struct{
    uint16_t handler_low;
    uint16_t selector;
    uint8_t  constant;              // constant=0 fixed 
    uint8_t  attribute;
    uint16_t handler_high;
}gate_descriptor;

// sizeod(void*) = 4(in 32 cpu), so there can use void* or uint32_t
extern void* interrupt_handler_entrys[IDT_NUM];     // handler entry table defined in interrupt.asm
void* interrupt_handler_table[IDT_NUM];             // entrys[i] will call table[i]
char* interrupt_info[IDT_NUM];                      // recard each interrupt info
gate_descriptor idt[IDT_NUM];                       // interrupt descriptor table


void idt_init();

#endif