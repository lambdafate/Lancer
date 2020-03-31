#include "print.h"
#include "init.h"

void test_func_put_char();
void test_func_put_char();
void test_func_put_hex();
void test_func_put_int();
void welcome();

int main(){

	welcome();
	hardware_init();

	asm volatile ("sti");
	asm volatile ("int $0");
	asm volatile ("cli");
	//test_func_put_char();

	//test_func_put_hex();
	//test_func_put_int();
	while(1){}
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




