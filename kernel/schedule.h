#ifndef _KERNEL_SCHEDULE_H
#define _KERNEL_SCHEDULE_H
#include "stdint.h"

#define PCB_NAME_SIZE       32
#define PCB_LDT_SIZE        2
#define PCB_MAX_NUM         2


enum TASK_STATUS{
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_DIED
};

typedef struct{
    uint32_t backlink;
    uint32_t esp0, ss0;
    uint32_t esp1, ss1;
    uint32_t esp2, ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp;
    uint32_t esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint32_t trace_bitmap;
    uint32_t ssp;
}TSS;


typedef struct{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t fs, gs, es, ds;
    uint32_t error_code;
    uint32_t eip, cs, eflags, esp, ss;
}StackFrame;


typedef struct{
    StackFrame stackframe;
    uint64_t ldt[PCB_LDT_SIZE];
    uint32_t r3;
    uint8_t pid;
    TASK_STATUS status;
    uint32_t time_counter;
    uint8_t priority;
    int8_t name[PCB_NAME_SIZE];
}PCB;


TSS tss;
PCB task0;
PCB tasks[PCB_MAX_NUM];
PCB *current_task;

void schedule_init();

#endif