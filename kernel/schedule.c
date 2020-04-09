#include "stdint.h"
#include "schedule.h"
#include "global.h"
#include "string.h"
#include "debug.h"

void init_tasks();
void init_stackframe_with_user_mode(STACKFRAME *stackframe, void *eip, void *esp);
void run_new_task(uint8_t *task_name, void *func);


TASK *current_task;
TASK *task_head_ready;
TASK  tasks[TASK_MAX_NUM];
int32_t pid_dispached;

// init first tss, and current_task, task_head_ready;
void schedule_init(){
    current_task = NULL;
    task_head_ready = NULL;
	pid_dispached = -1;
	init_tasks();

    set_tss();							// set tss segment descriptor in gdt
	flush_tss();						// load tss descriptor to tr.
}

void schedule(){
    TASK *temp = 0;
    uint32_t i = (current_task-tasks);
    for (; i < TASK_MAX_NUM; i = (i+1)%TASK_MAX_NUM){
        if(tasks[i].status == TASK_READY){
            temp = &tasks[i];
            break;   
        }
    }
	
    if(temp == 0){
        return;
    }
    switch_to(current_task, temp);
}


void switch_to(TASK *curr_task, TASK *new_task){
	curr_task->status = TASK_READY;
	new_task->status  = TASK_RUNNING;
	// maybe switch r3

	current_task = new_task;
	
	// we must change tss's esp0 to current_task's stackframe when happen task switch. 
    write_tss((uint32_t)current_task + sizeof(STACKFRAME));
}



void run_new_task(uint8_t *task_name, void *func){
	TASK *temp = 0;
	for (uint32_t i = 0; i < TASK_MAX_NUM; i++){
		if(tasks[i].status == TASK_DIED){
			temp = &tasks[i];
			break;
		}
	}
	if(temp == 0){
		return;
	}

	temp->status = TASK_READY;
	init_stackframe_with_user_mode(&temp->stackframe, func, 0x1000);
	// set task name
	strcpy(temp->name, task_name);
	// change current_task
	current_task =temp;
}

void switch_to_user_mode(){
	ASSERT(current_task != NULL);
	// ASSERT(current_task == tasks);

	// TASK *t0 = (TASK*)0x00001000;

	// current_task = t0;

	// memory_set(t0, 4096, 0);
	// init_stackframe_with_user_mode(&t0->stackframe, _task0, (void*)t0+0x1000);

	// write_tss((uint32_t)(t0)+sizeof(STACKFRAME));
	current_task->status = TASK_RUNNING;
	write_tss((uint32_t)current_task + sizeof(STACKFRAME));
	uint32_t _position = (uint32_t)current_task + 0x20;
	asm volatile("cli;\
				movl %0, %%esp;\
				popl %%fs;\
				popl %%gs;\
				popl %%es;\
				popl %%ds;\
				add $4, %%esp;\
				iret"::"a"(_position));
}


void init_tasks(){
	uint8_t* init_name = "you should't see it.";
	for (uint32_t i = 0; i < TASK_MAX_NUM; i++){
		init_stackframe_with_user_mode(&tasks[i].stackframe, 0, 0);		
		tasks[i].pid = -1;
		tasks[i].status = TASK_DIED;
		tasks[i].time_counter = TASK_TIMECOUNTER;
		tasks[i].priority = TASK_PRIORITY;
		tasks[i].r3		= 0;
		tasks[i].next	= 0;
		strcpy(tasks[i].name, init_name);
	}
}




// eip: target func address, where you will run begin.
// esp: user mode's stack.
void init_stackframe_with_user_mode(STACKFRAME *stackframe, void *eip, void *esp){
	stackframe->fs = GDT_SELECTOR_USER_DATA;
	stackframe->gs = GDT_SELECTOR_USER_DATA;
	stackframe->es = GDT_SELECTOR_USER_DATA;
	stackframe->ds = GDT_SELECTOR_USER_DATA;

	stackframe->ss = GDT_SELECTOR_USER_STACK;
	stackframe->esp = esp;
	stackframe->eflags = 0x0202;
	stackframe->cs = GDT_SELECTOR_USER_CODE;
	stackframe->eip = eip;
}
