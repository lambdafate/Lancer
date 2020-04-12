#ifndef _LIB_KERNEL_PRINT_H
#define _LIB_KERNEL_PRINT_H

#include "stdint.h"

void put_char(uint8_t ascii);
void put_str(int8_t* str);
void put_hex(int32_t num);
void put_int(int32_t num);

void printk(int8_t *fmt, ...);
void printf(int8_t *fmt, ...);


#endif