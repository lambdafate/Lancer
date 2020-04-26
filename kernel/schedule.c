#include "stdint.h"
#include "schedule.h"
#include "page.h"
#include "global.h"
#include "string.h"
#include "debug.h"

void init_tasks();
void init_stackframe(STACKFRAME *stackframe, void *eip, void *esp);
int32_t run_new_task(uint8_t *task_name, void *func);

TASK* schedule_priority();
TASK* schedule_time_ticks();

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
	TASK *new_task = schedule_priority();
	
    switch_to(current_task, new_task);
}


TASK* schedule_priority(){
	current_task->ticks--;
	if(current_task->ticks > 0){
		return current_task;
	}
	ASSERT(current_task->ticks <= 0);

	int32_t great_priority = 0;
	TASK *new_task = NULL;
	while(new_task == NULL){
		for (uint32_t i = 0; i < TASK_MAX_NUM; i++){
			if((tasks[i].status == TASK_READY || tasks[i].status == TASK_RUNNING) && great_priority < tasks[i].ticks){
				great_priority = tasks[i].ticks;
				new_task = &tasks[i];
			}
		}
		if(new_task != NULL){
			return new_task;
		}

		for (uint32_t i = 0; i < TASK_MAX_NUM; i++){
			if(tasks[i].status == TASK_READY || tasks[i].status == TASK_RUNNING){
				tasks[i].ticks = tasks[i].priority;
			}
		}		
	}	
}


TASK* schedule_time_ticks(){
    uint32_t i = (current_task-tasks+1)%TASK_MAX_NUM;
    for (; i < TASK_MAX_NUM; i = (i+1)%TASK_MAX_NUM){
        if(tasks[i].status == TASK_READY || i == (current_task - tasks)){
            return &tasks[i];   
        }
    }
}


void switch_to(TASK *curr_task, TASK *new_task){
	if(curr_task == new_task){
		return;
	}
	curr_task->status = TASK_READY;
	new_task->status  = TASK_RUNNING;
	current_task = new_task;
	// maybe switch r3
	asm volatile("movl %0, %%cr3"::"a"(current_task->pdt));

	// we must change tss's esp0 to current_task's stackframe when happen task switch. 
    write_tss((uint32_t)current_task + sizeof(STACKFRAME));
}



int32_t run_new_task(uint8_t *task_name, void *func){
	TASK *temp = 0;
	for (uint32_t i = 0; i < TASK_MAX_NUM; i++){
		if(tasks[i].status == TASK_DIED){
			temp = &tasks[i];
			break;
		}
	}
	if(temp == 0){
		return -1;
	}

	temp->status = TASK_READY;
	init_stackframe(&temp->stackframe, func, ((768<<22)+(0<<12)));
	temp->ticks = TASK_TICKS;
	temp->priority = TASK_PRIORITY;

	// set task name
	strcpy(temp->name, task_name);
	// change current_task
	current_task = temp;
	// set task pdt(will be loaded to CR3)
	set_task_pdt(current_task);

	return 1;
}

void switch_to_user_mode(){
	ASSERT(current_task != NULL);

	current_task->status = TASK_RUNNING;
	write_tss((uint32_t)current_task + sizeof(STACKFRAME));
	uint32_t _position = (uint32_t)current_task + 0x20;
	asm volatile("cli;\
				movl %1, %%cr3;\
				movl %0, %%esp;\
				popl %%fs;\
				popl %%gs;\
				popl %%es;\
				popl %%ds;\
				add $4, %%esp;\
				iret"::"a"(_position), "b"(current_task->pdt));
}


void init_tasks(){
	uint8_t* init_name = "you should't see it.";
	for (uint32_t i = 0; i < TASK_MAX_NUM; i++){
		init_stackframe(&tasks[i].stackframe, 0, 0);		
		tasks[i].pid = -1;
		tasks[i].status = TASK_DIED;
		tasks[i].ticks = TASK_TICKS;
		tasks[i].priority = TASK_PRIORITY;
		tasks[i].run_ticks = 0;
		tasks[i].pdt	= 0;
		// tasks[i].next	= 0;
		memset(tasks[i].fds, TASK_MAX_FD_NUM*sizeof(struct file_descriptor), 0);
		strcpy(tasks[i].name, init_name);
	}
}




// eip: target func address, where you will run begin.
void init_stackframe(STACKFRAME *stackframe, void *eip, void *esp){

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
