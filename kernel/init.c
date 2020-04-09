#include "interrupt.h"
#include "timer.h"
#include "malloc.h"
#include "schedule.h"

void lancer_init(){
    idt_init();
    timer_init(100);
    memory_init();
    schedule_init();
}