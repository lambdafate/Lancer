#include "interrupt.h"
#include "timer.h"
#include "malloc.h"
#include "schedule.h"

void lancer_init(){
    idt_init();
    timer_init(PIT_HZ);
    memory_init();
    schedule_init();
}