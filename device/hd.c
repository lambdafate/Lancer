#include "stdint.h"
#include "print.h"

// handle hard disk interrupt
// this function will run in kernel mode(ring0)
void handler_harddisk(){
    printk("handler hard disk!\n");
}


