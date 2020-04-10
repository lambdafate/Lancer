#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"
#include "debug.h"
#include "timer.h"

void welcome();
void _task0();
void _task1();
void _task2();


uint32_t get_ticks(){
	uint32_t ticks = 0;
	asm volatile("int $0x21":"=a"(ticks):"a"(2));
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

	run_new_task("A--", _task0);     
	tasks[0].stackframe.esp = 0x1000; 
	tasks[0].ticks = tasks[0].priority = 200;

	run_new_task("-B-", _task1);		
	tasks[1].stackframe.esp = 0x2000;
	tasks[1].ticks = tasks[1].priority = 300;

	run_new_task("--C", _task2);		
	tasks[2].stackframe.esp = 0x3000;
	tasks[2].ticks = tasks[2].priority = 400;

	// run_new_task("task -------------- 2", _task2);
	// run_new_task("task ------------------------- 3", _task3);

	put_str("i am here\n");
	switch_to_user_mode();

	while(1){}
}



void welcome(){
	put_str("[/kernel/lancer       ]: ");
	put_str("welcome to lancer!\n");
}


void _task0(){
	while(1){
		asm volatile("int $0x21"::"a"(1));
		delay(10);
	}
}

void _task1(){
	while(1){
		asm volatile("int $0x21"::"a"(1));
		delay(20);
	}
}


void _task2(){
	while(1){
		asm volatile("int $0x21"::"a"(1));
		delay(30);
	}
}

