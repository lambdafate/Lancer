#ifndef _KERNEL_SCHEDULE_H
#define _KERNEL_SCHEDULE_H
#include "stdint.h"

#define PCB_NAME_SIZE                   32
#define TASK_MAX_NUM                    30
#define TASK_TICKS                       3
#define TASK_PRIORITY                    3
#define TASK_RING0                       0
#define TASK_RING3                       3

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
    int32_t ticks;
    int32_t priority;
    uint32_t run_ticks;
    uint32_t r3;
    int8_t name[PCB_NAME_SIZE];
    struct _task *next;
}TASK;

extern TASK *current_task;
extern TASK *task_head_ready;
TASK  tasks[TASK_MAX_NUM];
void schedule_init();
void switch_to_user_mode();
void schedule();
void switch_to(TASK *curr_task, TASK *new_task);
void init_stackframe(STACKFRAME *stackframe, void *eip, void *esp, uint8_t ring);
int8_t run_new_task(uint8_t *task_name, void *func, uint8_t ring);

#endif