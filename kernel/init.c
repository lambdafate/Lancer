#include "interrupt.h"
#include "timer.h"
#include "malloc.h"

void lancer_init(){
    idt_init();
    timer_init();

    memory_init();
}