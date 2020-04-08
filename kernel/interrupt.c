#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"
#include "interrupt.h"
#include "schedule.h"

uint8_t clock_flag  = 0;

// sizeod(void*) = 4(in 32 cpu), so there can use void* or uint32_t
extern void* interrupt_handler_entrys[IDT_NUM];     // handler entry table defined in interrupt.asm
void* interrupt_handler_table[IDT_NUM];             // entrys[i] will call table[i]
char* interrupt_info[IDT_NUM];                      // recard each interrupt info
gate_descriptor idt[IDT_NUM];                       // interrupt descriptor table


static void init_pic();
static void init_gate_descriptor();
static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler);
static void init_hander_table();
static void _gernel_handler(uint8_t verctor);


void handler_clock(uint8_t vector){
    TASK *temp = 0;
    uint32_t i;
    for (i = 0; i < TASK_MAX_NUM; i++){
        if(tasks[i].status == TASK_READY){
            temp = &tasks[i];
            break;   
        }
    }
    if(temp == 0){
        return;
    }
    current_task->status = TASK_BLOCKED;
    temp->status = TASK_RUNNING;
    current_task = temp;

    current_task = tasks;
    put_str("\nclock interrupt  ");
    put_int(i);
    put_char('\n');
    
}

void handler_syscall(uint8_t vector){
    uint8_t *info="task0 syscall default\n";
    // asm volatile("movl %%eax, %0":"=m"(info):);
    put_str(info);
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
        interrupt_handler_table[i] = _gernel_handler;
        interrupt_info[i] = "unknown";
    }
    interrupt_handler_table[32] = handler_clock;
    interrupt_handler_table[33] = handler_syscall;
}


// gernel interrupt handler
static void _gernel_handler(uint8_t verctor){
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


// init idt[] according to interrupt_hander_table
static void init_gate_descriptor(){
    for (int32_t i = 0; i < 33; i++){
        make_gate_descriptor(&idt[i], IDT_ATTRIBUTE_DPL0, interrupt_handler_entrys[i]);
    }
    // init 33
    make_gate_descriptor(&idt[33], IDT_ATTRIBUTE_DPL3, interrupt_handler_entrys[33]);
}


static void make_gate_descriptor(gate_descriptor* gate, uint8_t attribute, void* handler){
    gate->handler_low = ((uint32_t)handler & 0x0000ffff);
    gate->selector = GDT_SELECTOR_KERNEL_CODE;
    gate->constant = 0;
    gate->attribute = attribute;
    gate->handler_high = ((uint32_t)handler & 0xffff0000) >> 16 ;
}


