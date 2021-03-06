#include "interrupt.h"
#include "timer.h"
#include "malloc.h"
#include "schedule.h"
#include "keyboard.h"
#include "hd.h"
#include "fs.h"

void lancer_init(){
    idt_init();

    timer_init(PIT_HZ);

    memory_init();
    
    schedule_init();

    kb_init();

    hd_init();

    fs_init();
}