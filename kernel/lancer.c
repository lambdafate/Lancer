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
#include "hd.h"

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

	run_new_task("scan-keyboard-input", task_keyboard);     
	run_new_task("test-task", _task0);     

	put_str("i am here\n");
	switch_to_user_mode();

	while(1){}
}



void welcome(){
	printk("[/kernel/lancer       ]: ");
	printk("welcome to lancer!\n");
}



void _task0(){
	
	printf("%s\n", "task0");
	asm volatile("int $0x80"::"a"(SYSCALL_NUMBER_HD_IDENTIFY));

	while(1){
		// printf("ticks: %x\n", get_ticks());
		// int num = 100;
		// while(num--){
		// 	delay(100);
		// }
	}
}



