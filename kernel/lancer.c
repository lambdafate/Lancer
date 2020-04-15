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

	// printk("%s, %d,  %x\n", "test", 12, 0xa00000);
	// while(1){}
	 
	// int8_t *test_reverse = "abcd efgk";
	// put_str(test_reverse);
	// put_str("\n");
	// put_str(str_reverse(test_reverse));
	// while(1){}


	// show_page_map();

	// run_new_task("A--", _task0);     
	// tasks[0].stackframe.esp = 0x1000; 
	// tasks[0].ticks = tasks[0].priority = 200;

	// run keyboard scan task
	run_new_task("scan-keyboard-input", task_keyboard, TASK_RING3);     
	tasks[0].stackframe.esp = 0x6000; 
	tasks[0].ticks = tasks[0].priority = 10;
	
	run_new_task("test-task", _task0, TASK_RING3);     
	tasks[0].stackframe.esp = 0x3000; 
	tasks[0].ticks = tasks[0].priority = 5;


	// int num = 10;
	// while(num--){
	// 	void *page = pmalloc();
	// 	printk("%x:  %x\n", num, page);
	// }
	// while(1){}

	put_str("i am here\n");
	switch_to_user_mode();

	while(1){}
}



void welcome(){
	printk("[/kernel/lancer       ]: ");
	printk("welcome to lancer!\n");
}



void _task0(){
	uint32_t *test = (uint32_t*)((0<<22) + (256<<12));
	*test = 0x12345678;
	printf("\n*test: %x test: %x\n", *test, test);
	test = (uint32_t*)((1<<22) + (256<<12) + 0x123);
	*test = 0x12345678;
	printf("\n*test: %x test: %x\n", *test, test);
	while(1){

	}
}



