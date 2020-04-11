#include "stdint.h"
#include "schedule.h"
#include "interrupt.h"
#include "debug.h"

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