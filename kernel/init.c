#include "interrupt.h"
#include "timer.h"

void hardware_init(){
    idt_init();
    timer_init();
}