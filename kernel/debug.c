#include "stdint.h"
#include "print.h"


void panic(uint8_t* condition, uint8_t* basefile, uint8_t* file, const uint8_t* function , uint32_t line){
    asm volatile ("cli");

    put_str("ASSERT ERROR:\n");
    put_str(basefile); put_str("-->"); put_str(file); put_str(":");
    put_int(line); put_str(": "); put_str(function); put_str("()    ");put_str(condition);
    put_char('\n');

    while(1){}
}



