#ifndef _LIB_KERNEL_IO_H
#define _LIB_KERNEL_IO_H
#include "stdint.h"

static inline void outb(uint16_t port, uint8_t data){
    asm volatile ("outb  %b0, %w1": : "a"(data), "Nd"(port));
} 

static inline void outw(uint16_t port, uint16_t data){
    asm volatile ("outw  %%ax, %w1": : "a"(data), "Nd"(port));
} 

static inline void outsw(uint16_t port, const void* addr, uint32_t count){
    asm volatile ("cld; rep outsw": "+S"(addr), "+c"(count): "d"(port));
}

static inline uint8_t inb(uint16_t port){
    uint8_t data;
    asm volatile ("inb %w1, %b0": "=a"(data): "Nd"(port));
    return data;
}

static inline void insw(uint16_t port, void* addr, uint32_t count){
    asm volatile ("cld; rep insw": "+D"(addr), "+c"(count): "d"(port): "memory");
}


#endif