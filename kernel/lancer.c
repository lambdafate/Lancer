#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"
#include "debug.h"
#include "timer.h"
#include "keyboard.h"

void welcome();
void _task0();


uint32_t get_ticks(){
	uint32_t ticks = 0;
	asm volatile("int $0x80":"=a"(ticks):"a"(2));
	return ticks;
}

// delay ms
void delay(uint32_t ms){
	uint32_t flag = get_ticks();
	while((get_ticks()-flag) * 1000 / PIT_HZ < ms){}
}


int main(){

	asm volatile("cli");
	
	welcome();

	while(1){

	}

	lancer_init();

	// run_new_task("A--", _task0);     
	// tasks[0].stackframe.esp = 0x1000; 
	// tasks[0].ticks = tasks[0].priority = 200;

	
	run_new_task("test-task", _task0, TASK_RING3);     
	tasks[0].stackframe.esp = 0x3000; 
	tasks[0].ticks = tasks[0].priority = 200;


	put_str("i am here\n");
	switch_to_user_mode();

	while(1){}
}



void welcome(){
	// printk("hello, %c, %c\n", 'G', 'F');
	printk("%s", "");
	// printk("[/kernel/lancer       ]: ");
	// printk("welcome to lancer!\n");
}



void _task0(){
	while(1){
		asm volatile("int $0x80"::"a"(1));
		// delay(10);
	}
}



