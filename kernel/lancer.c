#include "print.h"

void test_func_put_char();
int main(){

	test_func_put_char();

	char* str = "Hello, fuck, we must follow 'cdecl', if you call a fuction, for example call put_char here, you must deal with args on stack";
	put_str(str);

	while(1){}
}


void test_func_put_char(){
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