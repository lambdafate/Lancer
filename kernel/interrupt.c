#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"
#include "interrupt.h"
#include "schedule.h"
#include "debug.h"

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
static void init_hander_table();
static void handler_cpu_inside(uint8_t verctor);


void handler_clock(){
    // put_str("handler-clock----\n");
    clock_ticks++;

    current_task->run_ticks++;

    // if(current_task->ticks > 0){
    //     return;
    // }
    schedule();
}

// a template syscall
// eax=1: print current_task's name
// eax=2: get ticks
void handler_syscall(){
    uint32_t call_num = 0;
    asm volatile("":"=a"(call_num));
    ASSERT(call_num == 1 || call_num == 2);

    if(call_num == 1){
        // put_str("handler_syscall---> call_num: ");
        // put_int(call_num);
        // put_str("  ");
        put_str(current_task->name);
        put_str(" ------ ticks: ");
        put_int(current_task->ticks);
        put_str(" ------ priority: ");
        put_int(current_task->priority);
        put_str(" ------ run_ticks: ");
        put_int(current_task->run_ticks);
        put_str("\n");
        // put_str(" \n");
        return;
    }
    asm volatile(""::"a"(clock_ticks));
}


// init interrupt descriptor table
void idt_init(){   
    init_hander_table(); 
    init_gate_descriptor();                 
    init_pic();                                 // init programable interrupt controller

    // load idt
    uint64_t addr = (uint16_t)(sizeof(idt)-1) | (((uint64_t)((uint32_t)idt))<<16);
    asm volatile ("lidt %0": :"m"(addr));
    put_str("\nidt_init-> lidt done...\n");
}


static void init_hander_table(){
    for (uint32_t i = 0; i < 32; i++){
        interrupt_handler_table[i] = handler_cpu_inside;
        interrupt_info[i] = "unknown";
    }
    // clock interrupt
    interrupt_handler_table[32] = handler_clock;
    // a template syscall.
    interrupt_handler_table[33] = handler_syscall;
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
    make_gate_descriptor(&idt[32], IDT_ATTRIBUTE_DPL3, interrupt_handler_entrys[32]);

    // init 33
    make_gate_descriptor(&idt[33], IDT_ATTRIBUTE_DPL3, interrupt_handler_entrys[33]);
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
    outb(PIC_M_DATA, 0xfe);
    outb(PIC_S_DATA, 0xff);
}





static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler){
    gate->handler_low = ((uint32_t)handler & 0x0000ffff);
    gate->selector = GDT_SELECTOR_KERNEL_CODE;
    gate->constant = 0;
    gate->attribute = attribute;
    gate->handler_high = ((uint32_t)handler & 0xffff0000) >> 16 ;
}


