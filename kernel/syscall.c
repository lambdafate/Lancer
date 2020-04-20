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

void sys_hd_identify(){
    outb(0x1f6, 0xA0);

    outb(0x1f2, 0);
    outb(0x1f3, 0);
    outb(0x1f4, 0);
    outb(0x1f5, 0);

    outb(0x1f7, 0xec);
    uint8_t status = inb(0x1f7);
    if(status == 0){
        printk("no disk drive.\n");
        return;
    }
    
    printk("int 0x80: identify\n");   
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
            sys_hd_identify();
            return;
        default:
            break;
    }
}




