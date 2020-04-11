#include "stdint.h"
#include "print.h"
#include "io.h"
#include "keyboard.h"
#include "string.h"

static int8_t isempty();
static int8_t isfull();
static int8_t push_last(int8_t value);
static int8_t pop_first();

KEYBOARD_BUFFER kb_buffer;

void handler_keyboard(){
    put_str("handler-keyboard: ");
    uint8_t data = inb(0x60);
    push_last(data);
    put_hex(data);
    put_char('\n');
}


void kb_buffer_init(){
    kb_buffer.head = 0;
    kb_buffer.tail = 0;
    kb_buffer.size = 0;
    memory_set(kb_buffer.buffer, KEYBOARD_BUFFER_SIZE, 0);
}


void task_keyboard(){
    while(1){
        if(kb_buffer.size == 0){
            continue;
        }
        for (uint32_t i = 0; i < kb_buffer.size; i++){
        
        }
        
    }
}


static int8_t isempty(){
    return kb_buffer.size == 0; 
}

static int8_t isfull(){
    return (kb_buffer.tail + 1) % KEYBOARD_BUFFER_SIZE == kb_buffer.head;
}

static int8_t push_last(int8_t value){
    if(isfull()){
        return -1;
    }
    kb_buffer.buffer[kb_buffer.tail] = value;
    kb_buffer.tail = (kb_buffer.tail + 1) % KEYBOARD_BUFFER_SIZE;
    kb_buffer.size++;
    return value;
}

static int8_t pop_first(){
    if(isempty()){
        return -1;
    }
    int8_t value = kb_buffer.buffer[kb_buffer.head];
    kb_buffer.head = (kb_buffer.head + 1) % KEYBOARD_BUFFER_SIZE;
    kb_buffer.size--;
    return value;
}
