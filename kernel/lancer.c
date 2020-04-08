#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"

void welcome();
void _task0();

int main(){

	asm volatile("cli");
	
	PCB *t0 = (PCB*)0x00001000;
	current = (void*)t0;
	memory_set(t0, 4096, 0);

	welcome();
	lancer_init();

	t0->stackframe.fs = GDT_SELECTOR_USER_DATA;
	t0->stackframe.gs = GDT_SELECTOR_USER_DATA;
	t0->stackframe.es = GDT_SELECTOR_USER_DATA;
	t0->stackframe.ds = GDT_SELECTOR_USER_DATA;

	t0->stackframe.ss = GDT_SELECTOR_USER_STACK;
	t0->stackframe.esp = 0x2000;
	t0->stackframe.eflags = 0x0202;
	t0->stackframe.cs = GDT_SELECTOR_USER_CODE;
	t0->stackframe.eip = (void*)_task0;

	write_tss(0x1048);
	set_tss();
	flush_tss();

	asm volatile("cli;\
				movl $0x1020, %%esp;\
				popl %%fs;\
				popl %%gs;\
				popl %%es;\
				popl %%ds;\
				add $4, %%esp;\
				iret"::);

	// asm volatile("cli;\
	// 			  movl $67, %%eax;\
	// 			  movl %%eax, %%fs;\
	// 			  movl %%eax, %%gs;\
	// 		      movl %%eax, %%es;\
	// 			  movl %%eax, %%ds;\
	// 			  pushl $75;\ 
	// 			  pushl $0x2000;\
	// 			  pushf;\
	// 			  popl %%eax;\
	// 			  or $0x200, %%eax;\
	// 			  and $0xffffbfff, %%eax;\
	// 			  pushl %%eax;\
	// 			  pushl $59;\
	// 			  pushl %0;\
	// 			  iret"::"g"(_task0));

	
	while(1){}
}

void _task0(){
	while(1){
		asm volatile("int $0x21");
		for(int i=0; i<100; i++){

		}
	}
}

void welcome(){
	put_str("[/kernel/lancer       ]: ");
	put_str("welcome to lancer!\n");
}

