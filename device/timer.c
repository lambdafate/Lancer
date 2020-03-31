#include "io.h"

#define PIT_CTRT_PORT       0x43
#define PIT_DATA_PORT       0x40

#define PIT_CTRL_TIMER0     00110100
#define PIT_TIMER0_LOW      0x0               // it will send clock interrupt in 54.925ms 
#define PIT_TIMER0_HIGH     0x0               // if PIT_TIMER0=0(65535)

// PIT: programmable interval timer
// set PIT
void timer_init(){
    outb(PIT_CTRT_PORT, (uint8_t)PIT_CTRL_TIMER0);
    outb(PIT_DATA_PORT, (uint8_t)PIT_TIMER0_LOW);
    outb(PIT_DATA_PORT, (uint8_t)PIT_TIMER0_HIGH);
}