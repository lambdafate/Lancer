#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"

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
extern void* interrupt_handler_table[IDT_NUM];  // handler table defined in interrupt.asm
gate_descriptor idt[IDT_NUM];                   // interrupt descriptor table

static void init_pic();
static void init_gate_descriptor();
static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler);


// init interrupt descriptor table
void idt_init(){
    init_gate_descriptor();
    init_pic();                                 // init programable interrupt controller

    // load idt
    uint64_t addr = (uint16_t)(sizeof(idt)-1) | (((uint64_t)((uint32_t)idt))<<16);
    asm volatile ("lidt %0": :"m"(addr));
}

static void init_pic(){
    // init master
    outb(PIC_M_CTRL, 0x11);  // ICW1: need ICW4
    outb(PIC_M_DATA, 0x20);  // ICW2: init begin interrupt vector num-> 0x20(32)
    outb(PIC_M_DATA, 0x04);  // ICW3
    outb(PIC_M_DATA, 0x01);  // ICW4

    // init slave
    outb(PIC_S_CTRL, 0x11);
    outb(PIC_S_DATA, 0x28);  // begin verctor: 0x28
    outb(PIC_S_DATA, 0x02);
    outb(PIC_S_DATA, 0x01);

    // open master's IR0
    outb(PIC_M_DATA, 0xfe);
    outb(PIC_S_DATA, 0xff);
}




// init idt[] according to interrupt_hander_table
static void init_gate_descriptor(){
    for (int32_t i = 0; i < IDT_NUM; i++){
        make_gate_descriptor(&idt[i], IDT_ATTRIBUTE_DPL0, interrupt_handler_table[i]);
    }
}

static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler){
    gate->handler_low = ((uint32_t)handler & 0x0000ffff);
    gate->selector = CODE_SELECTOR;
    gate->constant = 0;
    gate->attribute = attribute;
    gate->handler_high = ((uint32_t)handler & 0xffff0000) >> 16 ;
}