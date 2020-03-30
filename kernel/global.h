#ifndef _KERNEL_GLOBAL_H
#define _KERNEL_GLOBAL_H


#define RPL0        0
#define RPL1        1
#define RPL2        2
#define RPL3        3
#define TI_GDT      0
#define TI_LDT      1

// selectors
#define CODE_SELECTOR       0x08
#define SCREEN_SELECTOR     0x10
#define DATA_SELECTOR       0x18
#define STACK_SELECTOR      0x20    

// idt' descriptor attribute
#define IDT_ATTRIBUTE_DPL0      0x8e
#define IDT_ATTRIBUTE_DPL3      0xee


#endif