#include "stdint.h"
#include "global.h"
#include "print.h"
#include "init.h"
#include "debug.h"
#include "bitmap.h"
#include "malloc.h"
#include "schedule.h"
#include "string.h"


typedef void (* func)(uint8_t*);

void test_func_put_char();
void test_func_put_char();
void test_func_put_hex();
void test_func_put_int();
void welcome();

int main(){

	welcome();

	// ASSERT(1 != 1);

	hardware_init();

	put_str("malloc(2): \n");
	uint8_t* first = 0x7c00;
	*first = 0xff;

	void *p = malloc(2);
	ASSERT(p == 0xa08000);
	put_hex(p); put_char('\n');

	p = malloc(1);
	ASSERT(p == 0xa0a000);
	put_hex(p); put_char('\n');

	*first = 0xf2;
	p= malloc(2);
	ASSERT(p == 0xa02000);
	put_hex(p); put_char('\n');

	PCB *t0 = (PCB*)0x00000000;
	memory_set(t0, 4096, 0);
	t0->ldt[0] = 0x00cf98000000ffff;
	t0->ldt[1] = 0x00cf92000000ffff;
	
	asm volatile ("sti");
	
	while(1){}
}

void _task0(){

}


void welcome(){
	put_str("[/kernel/lancer       ]: ");
	put_str("welcome to lancer!\n");
}



void test_func_put_char(){

	put_char('a'); put_char(' ');put_char('b'); put_char('\t'); put_char('c');put_char('\n');
	// test put_char
	put_char('f'); put_char('u'); put_char('c'); put_char('k');

	// test backspace
	put_char(8);

	// test '\r'
	put_char('\r'); put_char('r');

	// test '\n'
	put_char('\n'); put_char('n'); put_char('n');

	int num = 80;
	while(num--){
		put_char('n');
	}

	// test rollscreen
	num = 20;
	while(num--){
		put_char('\n');
	}
	put_char('n'); put_char('e'); put_char('w');
}


void test_fuc_put_str(){
	int line = 24;
	while(line--){}
	char* str = "Hello, fuck, we must follow 'cdecl', if you call a fuction, for example call put_char here, you must deal with args on stack";
	put_str(str);
}

void test_func_put_hex(){
	put_str("\n hex1: \0");
	put_hex(0x12345678);
	put_str(" hex2: \0");
	put_hex(0x0000);
	put_str(" hex3: \0");
	put_hex(0xffffffff);
	put_str(" hex4: \0");
	put_hex(0xf00e0aca);
}

void test_func_put_int(){
	put_str("\n int1: \0");
	put_int(123456);
	put_str(" int2: \0");
	put_int(0xffffffff);
	put_str(" int3: \0");
	put_int(0);
	put_str(" int4: \0");
	put_int(-1);
	
}




