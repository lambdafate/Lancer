#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"
#include "interrupt.h"
#include "schedule.h"
#include "debug.h"
#include "keyboard.h"
#include "syscall.h"
#include "page.h"
#include "hd.h"

#define IRQ0_CLOCK      11111110
#define IRQ1_KEYBOARD   11111101
#define IRQ2_SLAVE      11111011
#define IRQ14_HARDDISK  10111111

uint32_t clock_ticks = 0;


// sizeod(void*) = 4(in 32 cpu), so there can use void* or uint32_t
extern void* interrupt_handler_entrys[IDT_NUM];     // handler entry table defined in interrupt.asm
void* interrupt_handler_table[IDT_NUM];             // entrys[i] will call table[i]
char* interrupt_info[IDT_NUM];                      // recard each interrupt info
gate_descriptor idt[IDT_NUM];                       // interrupt descriptor table

static void init_pic();
static void init_gate_descriptor();
static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler);
static void init_handler_table();
static void handler_cpu_inside(uint8_t verctor);
void handler_clock();
void handler_donothing();


void* maskable_interrupt[16] = {
    handler_clock,     handler_keyboard,  handler_donothing, handler_donothing,     // IRQ0~IRQ3
    handler_donothing, handler_donothing, handler_donothing, handler_donothing,     // IRQ4~IRQ7
    handler_donothing, handler_donothing, handler_donothing, handler_donothing,     // IRQ8~IRQ11
    handler_donothing, handler_donothing, handler_harddisk,  handler_harddisk     // IRQ12~IRQ15
};

void handler_clock(){
    clock_ticks++;
    current_task->run_ticks++;

    // if(current_task->ticks > 0){
    //     return;
    // }
    schedule();
}


// init interrupt descriptor table
void idt_init(){   
    init_handler_table(); 
    init_gate_descriptor();                 
    init_pic();                                 // init programable interrupt controller

    // load idt
    uint64_t addr = (uint16_t)(sizeof(idt)-1) | (((uint64_t)((uint32_t)idt))<<16);
    asm volatile ("lidt %0": :"m"(addr));
    put_str("\nidt_init-> lidt done...\n");
}

void handler_donothing(){
    printk("do-nothing!\n");
}


static void init_handler_table(){
    // 0~31 num is cpu inside interrupt
    for (uint32_t i = 0; i < 32; i++){
        interrupt_handler_table[i] = handler_cpu_inside;
        interrupt_info[i] = "unknown";
    }
    // page fault.
    interrupt_handler_table[14] = handler_page_fault;
    
    // 0x20~0x2f is maskable interrupt
    for(uint32_t i = 0x20; i < 0x30; i++){
        interrupt_handler_table[i] = maskable_interrupt[i-0x20]; 
    }

    // syscall
    interrupt_handler_table[0x80] = handler_syscall;
}


// gernel interrupt handler
static void handler_cpu_inside(uint8_t verctor){
    if(verctor == 0x27 || verctor == 0x2f){
        return;
    }
    put_str("int 0x");
    put_int(verctor);
    put_str(" -- ");
    put_str(interrupt_info[verctor]);
    // put_char('\n');
    for(int i=0; i<1000000; i++){
        for(int i=0; i<1000000; i++){

        }
	}
}


// init idt[] according to interrupt_hander_table
static void init_gate_descriptor(){
    // init cpu inside interrupt 0~31
    for (int32_t i = 0; i < 32; i++){
        make_gate_descriptor(&idt[i], IDT_ATTRIBUTE_DPL0, interrupt_handler_entrys[i]);
    }

    // init maskable interrupt 0x20~0x2f
    for(uint32_t i = 0x20; i < 0x30; i++){
        make_gate_descriptor(&idt[i], IDT_ATTRIBUTE_DPL0, interrupt_handler_entrys[i]);
    }

    // init syscall interrupt
    make_gate_descriptor(&idt[0x80], IDT_ATTRIBUTE_DPL3, interrupt_handler_entrys[0x30]);
}


// init intel 8259A
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
    outb(PIC_M_DATA, 0xff & IRQ0_CLOCK & IRQ1_KEYBOARD & IRQ2_SLAVE & 0);
    outb(PIC_S_DATA, 0xff & IRQ14_HARDDISK & 0);
}


static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler){
    gate->handler_low = ((uint32_t)handler & 0x0000ffff);
    gate->selector = GDT_SELECTOR_KERNEL_CODE;
    gate->constant = 0;
    gate->attribute = attribute;
    gate->handler_high = ((uint32_t)handler & 0xffff0000) >> 16 ;
}


