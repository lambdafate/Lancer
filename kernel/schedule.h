#ifndef _KERNEL_SCHEDULE_H
#define _KERNEL_SCHEDULE_H
#include "stdint.h"

#define PCB_NAME_SIZE                   32
#define TASK_MAX_NUM                    30
#define TASK_TIMECOUNTER                 3
#define TASK_PRIORITY                    3


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
}STACKFRAME;                                // 18 * 4 = 72bytes


typedef struct _task{
    STACKFRAME stackframe;
    int32_t pid;
    uint8_t status;
    uint32_t time_counter;
    uint8_t priority;
    uint32_t r3;
    int8_t name[PCB_NAME_SIZE];
    struct _task *next;
}TASK;

extern TASK *current_task;
extern TASK *task_head_ready;
void schedule_init();
void switch_to_user_mode();

#endif