#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"

void welcome();

int main(){

	asm volatile("cli");
	
	welcome();
	lancer_init();
	
	switch_to_user_mode();

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



void welcome(){
	put_str("[/kernel/lancer       ]: ");
	put_str("welcome to lancer!\n");
}

