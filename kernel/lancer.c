#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"
#include "debug.h"
#include "timer.h"
#include "keyboard.h"
#include "syscall.h"
#include "page.h"


void welcome();
void _task0();


uint32_t get_ticks(){
	uint32_t ticks = 0;
	ticks = sys_get_ticks();
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
	lancer_init();

	// uint32_t *test_p = (uint32_t*)((768<<22)-4);
	// *test_p = 0x12345;
	// printk("test page fault: %x\n", *test_p);
	// while(1){}

	// show_page_map();

	// run_new_task("A--", _task0);     
	// tasks[0].stackframe.esp = 0x1000; 
	// tasks[0].ticks = tasks[0].priority = 200;

	// run keyboard scan task
	run_new_task("scan-keyboard-input", task_keyboard, TASK_RING3);     
	tasks[0].stackframe.esp = (uint32_t*)((768 << 22) + ((0 << 12))); 
	tasks[0].ticks = tasks[0].priority = 10;

	// printk("scan-keyboard-input init over\n");
	run_new_task("test-task", _task0, TASK_RING3);     
	tasks[1].stackframe.esp = (uint32_t*)((768<<22) + (0<<12)); 
	tasks[1].ticks = tasks[1].priority = 5;


	put_str("i am here\n");
	switch_to_user_mode();

	while(1){}
}



void welcome(){
	printk("[/kernel/lancer       ]: ");
	printk("welcome to lancer!\n");
}



void _task0(){
	while(1){
		printf("task0...\n");
		uint32_t num = 100;
		while (num--)
		{
			delay(100);
		}
		
	}
}



