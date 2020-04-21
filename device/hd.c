#include "stdint.h"
#include "print.h"
#include "interrupt.h"
#include "hd.h"
#include "io.h"
#include "string.h"
#include "debug.h"

void hd_init(){
    uint8_t *hd_num = (uint8_t*)(0x475);
    printk("hd num: %x\n", *hd_num);

    if(*hd_num){
        enable_irq(IRQ2_SLAVE, IRQ_MASTER);
        enable_irq(IRQ14_HARDDISK, IRQ_SLAVE);
        enable_irq(IRQ15_HARDDISK, IRQ_SLAVE);
    }
}


// handle hard disk interrupt
// this function will run in kernel mode(ring0)
void handler_harddisk(){
    printk("handler hard disk!\n");
    uint16_t hd_info[256];
    insw(HD_PORT_PRIMARY_DATA, hd_info, 256);

    printk("hard disk info:\n");
    printk("    hd sn: ");
    uint8_t *temp = &hd_info[10];
    // temp++;
    uint8_t num = 10;
    while(num--){
        printk("%c", *(temp+1));
        printk("%c", *temp);
        temp += 2;
    }
    printk("\n    hd model: ");
    num = 20;
    temp = &hd_info[27];
    while(num--){
        printk("%c", *(temp+1));
        printk("%c", *temp);
        temp += 2;
    }
    
    printk("\n    hd size: ");
    uint32_t sector = ((uint32_t)(hd_info[61]) << 16) + (uint32_t)(hd_info[60]);
    printk("%x MB\n", sector / 2048);

    printk("    LBA: %s\n", (hd_info[49] & 0x0200) ? "YES": "NO");
    printk("    48 search: %s\n", (hd_info[83] & 0x0400) ? "YES": "NO");

}

// handle 'int 0x80' to get hd identify.
// run in ring0
void sys_hd_identify(){
    uint8_t wait = 100000;
    while((inb(HD_PORT_PRIMARY_STATUS_COMMAND) & 0x80) && wait--){}
    if(wait == 0){
        printk("Identify error: hard disk busy.\n");
        return;
    }
    outb(HD_PORT_PRIMARY_DEVICE, MAKE_DEVICE(DEVICE_LBA_MODE, DEVICE_DRV_MASTER, 0));

    outb(HD_PORT_PRIMARY_SECTOR_COUNT, 0);
    outb(HD_PORT_PRIMARY_LBA_LOW, 0);
    outb(HD_PORT_PRIMARY_LBA_MID, 0);
    outb(HD_PORT_PRIMARY_LBA_HIGH, 0);

    outb(HD_PORT_PRIMARY_STATUS_COMMAND, COMMAND_IDENTIFY);
    uint8_t status = inb(HD_PORT_PRIMARY_STATUS_COMMAND);
    if(status == 0){
        printk("Identify: the drive does not exist\n");
    }else{
        printk("Identify status: %x\n", status);   
    }
}