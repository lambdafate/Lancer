#ifndef _KERNEL_SCHEDULE_H
#define _KERNEL_SCHEDULE_H
#include "stdint.h"

#define PCB_NAME_SIZE       32
#define PCB_LDT_SIZE        3
#define PCB_MAX_NUM         2


enum TASK_STATUS{
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_DIED
};

typedef struct{
    uint32_t edi, esi, ebp, esp_, ebx, edx, ecx, eax;
    uint32_t fs, gs, es, ds;
    uint32_t error_code;
    uint32_t eip, cs, eflags, esp, ss;
}StackFrame;                                // 18 * 4 = 72bytes


typedef struct{
    StackFrame stackframe;
    uint32_t r3;
    uint8_t pid;
    uint8_t status;
    uint32_t time_counter;
    uint8_t priority;
    int8_t name[PCB_NAME_SIZE];
}PCB;

extern uint32_t *current;

void schedule_init();

#endif