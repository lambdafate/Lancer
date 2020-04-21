#include "stdint.h"
#include "print.h"
#include "interrupt.h"
#include "hd.h"
#include "io.h"
#include "string.h"
#include "debug.h"

struct disk sata0_master;

void hd_init(){
    uint8_t *hd_num = (uint8_t*)(0x475);
    printk("hd num: %x\n", *hd_num);

    ASSERT(*hd_num != 0);
    
    enable_irq(IRQ2_SLAVE, IRQ_MASTER);
    enable_irq(IRQ14_HARDDISK, IRQ_SLAVE);
    // enable_irq(IRQ15_HARDDISK, IRQ_SLAVE);

    // we support only one disk now.
    strcpy(sata0_master.name, "sata0-master");

    
}


// handle hard disk interrupt
// this function will run in kernel mode(ring0)
void handler_harddisk(){
    printk("handler hard disk!\n");
}

// handle 'int 0x80' to get hd identify.
// run in ring0
void sys_hd_identify(){
    uint8_t wait = 100000;
    while((inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_BUSY) && wait--){}
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
        return;
    }
    printk("status register: %x\n", status);
    while((status & STATUS_BUSY) && !(status & STATUS_DRQ) ){
        status = inb(HD_PORT_PRIMARY_STATUS_COMMAND);
    }
    if(status & STATUS_ERROR){
        printk("Identify error: status register's error=1.\n");
        return;
    }

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

void sata_wait(){
    while(inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_BUSY){}
}

int32_t sata_read(uint32_t begin_sector, uint8_t sector_count, void *buffer){
    ASSERT(sector_count == 1);
    ASSERT(begin_sector > 0);

    sata_wait();

    outb(HD_PORT_PRIMARY_DEVICE, MAKE_DEVICE(DEVICE_LBA_MODE, DEVICE_DRV_MASTER, (((uint8_t)(begin_sector >> 24)) & 0x0f)));
    
    outb(HD_PORT_PRIMARY_LBA_LOW, (uint8_t)(begin_sector));
    outb(HD_PORT_PRIMARY_LBA_MID, (uint8_t)(begin_sector >> 8));
    outb(HD_PORT_PRIMARY_LBA_HIGH, (uint8_t)(begin_sector >> 16));

    outb(HD_PORT_PRIMARY_SECTOR_COUNT, sector_count);

    outb(HD_PORT_PRIMARY_STATUS_COMMAND, COMMAND_READ);

    if(inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_ERROR){
        printk("Read error: status register's error=1.\n");
        return -1;
    }
    // while((inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_DRQ) == 0){}

    insw(HD_PORT_PRIMARY_DATA, buffer, sector_count * 512 / 2);

    ASSERT((inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_DRQ) == 0);
    return sector_count * 512;
}

int32_t sata_write(uint32_t begin_sector, uint8_t sector_count, void *buffer){
    ASSERT(sector_count == 1);
    ASSERT(begin_sector > 0);

    sata_wait();
    
    outb(HD_PORT_PRIMARY_DEVICE, MAKE_DEVICE(DEVICE_LBA_MODE, DEVICE_DRV_MASTER, (((uint8_t)(begin_sector >> 24)) & 0x0f)));
    
    outb(HD_PORT_PRIMARY_LBA_LOW, (uint8_t)(begin_sector));
    outb(HD_PORT_PRIMARY_LBA_MID, (uint8_t)(begin_sector >> 8));
    outb(HD_PORT_PRIMARY_LBA_HIGH, (uint8_t)(begin_sector >> 16));

    outb(HD_PORT_PRIMARY_SECTOR_COUNT, sector_count);

    outb(HD_PORT_PRIMARY_STATUS_COMMAND, COMMAND_WRITE);
    
    // uint8_t status = inb(HD_PORT_PRIMARY_STATUS_COMMAND);
    // while((status & STATUS_DRQ) == 0){
    //     status = inb(HD_PORT_PRIMARY_STATUS_COMMAND);
    // }

    outsw(HD_PORT_PRIMARY_DATA, buffer, sector_count * 512 / 2);

    // uint32_t times = sector_count * 512 / 2;
    // uint16_t *temp = (uint16_t*)buffer;
    // while(times--){
    //     if(inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_ERROR){
    //         printk("Write error: status register's error=1.\n");
    //         return -1;
    //     }
    //     outw(HD_PORT_PRIMARY_DATA, *temp);
    //     outb(HD_PORT_PRIMARY_STATUS_COMMAND, COMMAND_FLUSH);
    //     temp++;
    // }
    return sector_count * 512;
}


