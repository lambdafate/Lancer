#include "interrupt.h"
#include "timer.h"
#include "malloc.h"
#include "schedule.h"
#include "keyboard.h"

void lancer_init(){
    idt_init();
    timer_init(PIT_HZ);
    memory_init();
    schedule_init();

    kb_buffer_init();
}