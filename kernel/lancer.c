#include "global.h"
#include "stdint.h"
#include "print.h"
#include "init.h"
#include "schedule.h"

void welcome();

void task1(){
	while(1){
		
		for(int i=0; i<100; i++){

		}
	}
}




int main(){

	asm volatile("cli");
	
	welcome();
	lancer_init();
	
	switch_to_user_mode();

	while(1){}
}



void welcome(){
	put_str("[/kernel/lancer       ]: ");
	put_str("welcome to lancer!\n");
}

