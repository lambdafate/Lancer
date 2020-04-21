/*  
    file: /kernel/syscall.c
    a syscall, just same as 'int 0x80' in GNU/Linux.
    eax: syscall number
    ebx, ecx, edx: args

        author: lambdafate
        data: 2020/04/12 16:42.
*/

#include "stdint.h"
#include "syscall.h"
#include "schedule.h"
#include "interrupt.h"
#include "hd.h"
#include "print.h"
#include "debug.h"
#include "io.h"


void sys_write(int8_t *str){
    asm volatile("int $0x80"::"a"(SYSCALL_NUMBER_WRITE), "b"(str));
}

uint32_t sys_get_ticks(){
    uint32_t ticks = 0;
	asm volatile("int $0x80":"=a"(ticks):"a"(SYSCALL_NUMBER_GET_TICKS));
	return ticks;
}

void test(){

    uint8_t *buffer = (uint8_t*)(0x7c00);
    strcpy(buffer, "there is buffer                           what the fuck!!!!\n");

    // printk("first buffer: %s\n", buffer);

	// sys_hd_identify();
    printk("write buffer from 0x7c00: %s\n", buffer);
    int32_t res = sata_write(2, 1, buffer);
    if(res == -1){
        printk("write error\n");
    }

    buffer = (uint8_t*)(0x7000);
    printk("0x7000 before read: %s\n", buffer);
	res = sata_read(2, 1, buffer);
	if(res == -1){
		printk("read error\n");
	}

    ASSERT(buffer == 0x7000);
    printk("0x7000 after read: %s\n", buffer);
    
}



void handler_syscall(){
    uint32_t syscall_numer = 0;
    void *arg1 = NULL;
    asm volatile("":"=a"(syscall_numer), "=b"(arg1));

    ASSERT(syscall_numer < SYSCALL_NUMBER);
    uint8_t temp;

    switch (syscall_numer){
        case SYSCALL_NUMBER_GET_TICKS:
            asm volatile(""::"a"(clock_ticks));
            return;
        case SYSCALL_NUMBER_WRITE:
            put_str((int8_t*)arg1);
            return;
        case SYSCALL_NUMBER_HD_IDENTIFY:
            test();
            return;
        default:
            break;
    }
}




