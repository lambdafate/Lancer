#ifndef _KERNEL_INTERRUPT_H
#define _KERNEL_INTERRUPT_H
#include "stdint.h"

#define IDT_NUM     256
#define PIC_M_CTRL  0x20        // intel 8259A interrupt
#define PIC_M_DATA  0x21        
#define PIC_S_CTRL  0xa0
#define PIC_S_DATA  0xa1

// idt' descriptor attribute
#define IDT_ATTRIBUTE_DPL0                    0x8e
#define IDT_ATTRIBUTE_DPL3                    0xee

typedef struct{
    uint16_t handler_low;
    uint16_t selector;
    uint8_t  constant;              // constant=0 fixed 
    uint8_t  attribute;
    uint16_t handler_high;
}gate_descriptor;


extern uint32_t clock_ticks;
void idt_init();

#endif