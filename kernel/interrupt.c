#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"
#include "interrupt.h"
#include "schedule.h"
#include "debug.h"
#include "keyboard.h"
#include "syscall.h"

uint32_t clock_ticks = 0;

uint8_t clock_flag  = 0;
int32_t syscall_number = -1;
void *syscall_arg1 = NULL;
void *syscall_arg2 = NULL;
void *syscall_arg3 = NULL;


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


static void init_handler_table(){
    for (uint32_t i = 0; i < 32; i++){
        interrupt_handler_table[i] = handler_cpu_inside;
        interrupt_info[i] = "unknown";
    }
    // clock interrupt.
    interrupt_handler_table[0x20] = handler_clock;
    // keyboard interrupt.
    interrupt_handler_table[0x21] = handler_keyboard;

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

	}
}


// init idt[] according to interrupt_hander_table
static void init_gate_descriptor(){
    // init cpu inside interrupt
    for (int32_t i = 0; i < 32; i++){
        make_gate_descriptor(&idt[i], IDT_ATTRIBUTE_DPL0, interrupt_handler_entrys[i]);
    }
    // init clock interrupt
    make_gate_descriptor(&idt[0x20], IDT_ATTRIBUTE_DPL3, interrupt_handler_entrys[32]);

    // init keyboard interrupt
    make_gate_descriptor(&idt[0x21], IDT_ATTRIBUTE_DPL3, interrupt_handler_entrys[33]);

    // init syscall interrupt
    make_gate_descriptor(&idt[0x80], IDT_ATTRIBUTE_DPL3, interrupt_handler_entrys[34]);
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
    outb(PIC_M_DATA, 0xff-3);
    outb(PIC_S_DATA, 0xff);
}


static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler){
    gate->handler_low = ((uint32_t)handler & 0x0000ffff);
    gate->selector = GDT_SELECTOR_KERNEL_CODE;
    gate->constant = 0;
    gate->attribute = attribute;
    gate->handler_high = ((uint32_t)handler & 0xffff0000) >> 16 ;
}


