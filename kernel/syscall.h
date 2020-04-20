#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H


#define SYSCALL_NUMBER                      3   

#define SYSCALL_NUMBER_GET_TICKS            0
#define SYSCALL_NUMBER_WRITE                1
#define SYSCALL_NUMBER_HD_IDENTIFY          2

void handler_syscall();

void sys_write(int8_t *str);
uint32_t sys_get_ticks();


#endif