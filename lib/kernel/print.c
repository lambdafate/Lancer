#include "stdint.h"
#include "print.h"
#include "string.h"
#include "debug.h"

int32_t vsprintf(int8_t *buffer, int8_t *fmt, int32_t *args);

void printf(int8_t *fmt, ...){
    int8_t buffer[256];

}

void printk(int8_t *fmt, ...){
    int8_t buffer[256];
    vsprintf(buffer, fmt, (int32_t*)((uint32_t)(&fmt)+4));
    put_str(buffer);
}


int32_t vsprintf(int8_t *buffer, int8_t *fmt, int32_t *args){
    int8_t *save = buffer;
    int8_t *work;
    while (*fmt){
        if(*fmt != '%'){
            *save++ = *fmt++;
            continue;
        }
        fmt++;
        switch (*fmt){
            case 's':
                work = (int8_t*)(*args);
                while(*work){
                    *save++ = *work++;
                }
                
            case 'c':
                *save++ = (int8_t)(*args);
                
            default:
                fmt++;
                args++;
                break;
        }
    }
    // ASSERT(save == buffer);
    *save = '\0';
    put_hex(save);
    put_char('\n');
    put_hex(buffer);
    return save-buffer;
}



