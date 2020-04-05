#ifndef _KERNEL_GLOBAL_H
#define _KERNEL_GLOBAL_H

#define RPL0        0
#define RPL1        1
#define RPL2        2
#define RPL3        3
#define TI_GDT      0
#define TI_LDT      1

#define GDTADDRESS          0x00080000
#define GDTLimit            55

// selectors
#define CODE_SELECTOR       0x08
#define SCREEN_SELECTOR     0x10
#define DATA_SELECTOR       0x18
#define STACK_SELECTOR      0x20
#define TSS_SELECTOR        0x28
#define LDT_SELECTOR        0x30 


// idt' descriptor attribute
#define IDT_ATTRIBUTE_DPL0      0x8e
#define IDT_ATTRIBUTE_DPL3      0xee



uint64_t* gdt = (uint64_t*)GDTADDRESS;
uint32_t  gdt_limit = GDTLimit;

// void set_gdt_descriptor(uint32_t selecor, uint32_t base, uint32_t limit, );

#endif