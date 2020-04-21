#ifndef _DEVICE_KEYBOARD_H
#define _DEVICE_KEYBOARD_H
#include "stdint.h"

#define KEYBOARD_BUFFER_SIZE    1024

typedef struct{
    uint32_t head;
    uint32_t tail;
    uint32_t size;
    int8_t buffer[KEYBOARD_BUFFER_SIZE];
}KEYBOARD_BUFFER;


void handler_keyboard();
void kb_init();
void task_keyboard();

#endif