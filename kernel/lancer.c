#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"

void welcome();
void _task0();
void _task1();
void _task2();
void _task3();

int main(){

	asm volatile("cli");
	
	welcome();
	lancer_init();
	
	run_new_task("task -- 0", _task0);
	run_new_task("task ------- 1", _task1);
	run_new_task("task -------------- 2", _task2);
	run_new_task("task ------------------------- 3", _task3);
	switch_to_user_mode();

	while(1){}
}



void welcome(){
	put_str("[/kernel/lancer       ]: ");
	put_str("welcome to lancer!\n");
}


void _task0(){
	while(1){
		asm volatile("int $0x21");

	}
}

void _task1(){
	while(1){
		asm volatile("int $0x21");

	}
}

void _task2(){
	while(1){
		asm volatile("int $0x21");

	}
}

void _task3(){
	while(1){
		asm volatile("int $0x21");

	}
}