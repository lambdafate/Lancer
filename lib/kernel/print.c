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
    uint32_t temp;

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
            case 'x':
                temp = (uint32_t)(*args++);
                save += itoc(save, temp);
                fmt++;
                break;
            default:
                *save++ = '%';
                *save++ = *fmt;
                args++;
                fmt++;
                break;
        }
    }
    
    *save = '\0';
    return save-buffer;
}


uint32_t itoc(int8_t *buffer, uint32_t value){
    int8_t *work = buffer;
    if(value == 0){
        *work++ = 0x30;        // 0x30 ---> '0'
        *work = '\0';
        return 1;
    }
    while(value){
        uint8_t fbits = ((uint8_t)value) & 0x0f;
        value = value >> 4;

        if(fbits < 10){
            fbits += 0x30;
        }else{
            fbits += 87;
        }
        *work++ = fbits;
    }
    *work = '\0';
    str_reverse(buffer);
    return work - buffer;
}

