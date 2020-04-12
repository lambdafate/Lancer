#include "stdint.h"


void memory_set(uint8_t* dest, uint32_t size, uint8_t value){
    uint32_t index = 0;
    uint8_t* destination = dest;
    while(index++ < size){
        *destination++ = value;
    }
}

uint32_t strlen(int8_t *str){
    uint32_t len = 0;
    while(*str++){
        len++;
    }
    return len;
}

int8_t* strcpy(int8_t *dest, int8_t *src){
    int8_t *res = dest;
    while(*src){
        *dest++ = *src++;
    }
    *dest = '\0';
    return res;
}
