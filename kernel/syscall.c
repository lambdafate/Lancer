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
#include "string.h"
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

void print_mbr(struct part *p){
    printk("boot: %x  part-type: %x  offset: %x   sum-sectors: %x\n", p->boot, p->part_type, p->sectors_offset, p->sectors_count);
    printk("start-head: %x   start-sector: %x  start-cylinder: %x\n", p->start_head, p->start_sector, p->start_cylinder);
    printk("end-head  : %x   end-sector  : %x  end-cylinder  : %x\n", p->end_head, p->end_sector, p->end_cylinder);
}


void test(){
    uint8_t *buffer = (uint8_t*)(0x0000);
    if(sata_read(0, 1, buffer) == -1){
        printk("Read error!\n");
        return;
    }
    ASSERT(*(buffer+510) == 0x55);
    ASSERT(*(buffer+511) == 0xaa);

    memcpy(mbrparts, buffer+446, 64);
    uint32_t logical_index = 0;

    for(uint32_t i=0; i < 4; i++){
        if(mbrparts[i].part_type == 0x00){
            continue;
        }
        if(mbrparts[i].part_type != 0x05){
            printk("\n    ******     %x: master partition    *******\n", i);
            print_mbr(&mbrparts[i]);
            continue;
        }
        // it is a extend partition
        printk("\n    ******     %x: extend partition   *******\n", i);

        ASSERT(sata_read(mbrparts[i].sectors_offset, 1, buffer) != -1);
        memcpy(&logicalparts[logical_index], buffer+446, 32);
        print_mbr(&logicalparts[logical_index++]);

        ASSERT(*(buffer+510) == 0x55);
        ASSERT(*(buffer+511) == 0xaa);

        while(logical_index < 8 && logicalparts[logical_index].part_type != 0){
            uint32_t begin_sector = mbrparts[i].sectors_offset + logicalparts[logical_index++].sectors_offset;
            
            sata_read(begin_sector, 1, buffer);
            memcpy(&logicalparts[logical_index], buffer+446, 32);
            
            ASSERT(*(buffer+510) == 0x55);
            ASSERT(*(buffer+511) == 0xaa);

            printk("\n    ******     extend partition    *******\n");
            print_mbr(&logicalparts[logical_index++]);
        }    

    }
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




