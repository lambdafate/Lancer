#include "io.h"
#include "stdint.h"
#include "interrupt.h"

#define PIT_CTRT_PORT       0x43
#define PIT_DATA_PORT       0x40

#define PIT_CTRL_TIMER0     00110100
#define PIT_TIMER0_LOW      0x0               // it will send clock interrupt in 54.925ms 
#define PIT_TIMER0_HIGH     0x0               // if PIT_TIMER0=0(65535)


// PIT: programmable interval timer
// set PIT
void timer_init(uint32_t frequency){
    outb(PIT_CTRT_PORT, (uint8_t)PIT_CTRL_TIMER0);

    uint32_t divisor = 1193180 / frequency;
    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    outb(PIT_DATA_PORT, l);
    outb(PIT_DATA_PORT, h);

    enable_irq(IRQ0_CLOCK, IRQ_MASTER);
}