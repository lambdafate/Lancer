#include "stdint.h"
#include "print.h"
#include "string.h"
#include "syscall.h"
#include "debug.h"

int32_t vsprintf(int8_t *buffer, int8_t *fmt, int32_t *args);

// use in user mode(ring3).
void printf(int8_t *fmt, ...){
    int8_t buffer[256];
    vsprintf(buffer, fmt, (int32_t*)((int32_t)(&fmt)+4));
    sys_write(buffer);
}


// use in kernel mode(ring0)
void printk(int8_t *fmt, ...){
    uint8_t buffer[256];
    vsprintf(buffer, fmt, (int32_t*)((uint32_t)(&fmt)+4));
    put_str(buffer);
}


int32_t vsprintf(int8_t *buffer, int8_t *fmt, int32_t *args){
    int8_t *save = buffer;
    int8_t *work = NULL;
    while (*fmt){
        if(*fmt != '%'){
            *save++ = *fmt++;
            continue;
        }
        fmt++;
        switch (*fmt){
            case 's':
                work = (int8_t*)(*args++);
                while(*work){
                    *save++ = *work++;
                }
                fmt++;
                break;
            case 'c':
                *save++ = (int8_t)(*args++);
                fmt++;
                break;
            default:
                ASSERT(0);
                break;
        }
    }
    
    *save = '\0';
    return save-buffer;
}



