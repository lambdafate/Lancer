#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"
#include "debug.h"

void welcome();
void _task0();
void _task1();
void _task2();
void _task3();
uint8_t *info0 = "I";
uint8_t *info1 = "will";
uint8_t *info2 = "fuck";
uint8_t *info3 = "you";
int32_t res = 0;

int main(){

	asm volatile("cli");
	
	welcome();
	lancer_init();

	run_new_task("task -- 0", _task0);
	run_new_task("task ------- 1", _task1);
	run_new_task("task -------------- 2", _task2);
	run_new_task("task ------------------------- 3", _task3);
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
		asm volatile("int $0x21"::"a"(info0));
	}
}

void _task1(){
	while(1){
		asm volatile("int $0x21"::"a"(info1));

	}
}

void _task2(){
	while(1){
		asm volatile("int $0x21"::"a"(info2));

	}
}

void _task3(){
	while(1){
		asm volatile("int $0x21"::"a"(info3));

	}
}