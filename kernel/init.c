#include "interrupt.h"
#include "timer.h"
#include "malloc.h"

void hardware_init(){
    idt_init();
    timer_init();

    memory_init();
}