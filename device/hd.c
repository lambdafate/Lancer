#include "stdint.h"
#include "print.h"
#include "interrupt.h"
#include "hd.h"
#include "io.h"
#include "string.h"
#include "debug.h"

void sata_wait();
static void sata_exec_command(uint8_t command, uint32_t begin_sector, uint8_t sector_count);
static int32_t sata_check_status();

struct disk sata0_master;

// detect hard disk's partitions
void detect_disk_parts(struct disk *disk){
    disk->mpart_num = 0;
    disk->lpart_num = 0;

    struct part parts_table[4];

    uint8_t *buffer = (uint8_t*)(0xc0000000);
    if(sata_read(0, 1, buffer) == -1){
        printk("Read error!\n");
        return;
    }
    ASSERT(*(buffer+510) == 0x55);
    ASSERT(*(buffer+511) == 0xaa);

    memcpy(parts_table, buffer+446, 64);
    uint32_t logical_index = 0;

    for(uint32_t i=0; i < 4; i++){
        // empty partition, just break;
        if(parts_table[i].part_type == 0x00){
            break;
        }
        // it's a master partition.
        if(parts_table[i].part_type != 0x05){
            // printk("\n    ******     %x: master partition    *******\n", i);
            // print_mbr(&mbrparts[i]);
            disk->master_parts[disk->mpart_num].start_sector = parts_table[i].sectors_offset;
            disk->master_parts[disk->mpart_num].sector_count = parts_table[i].sectors_count; 
            disk->mpart_num++;
            continue;
        }

        // it is a extend partition
        // we assume extend partition in last.
        // printk("\n    ******     %x: extend partition   *******\n", i);
        
        uint32_t extend_part_offset = parts_table[i].sectors_offset;
        uint32_t sub_extend_part_offset = 0;

        while(disk->lpart_num < LOGICAL_PARTITION_MAX_NUM){
            uint32_t begin_sector = extend_part_offset + sub_extend_part_offset;
            
            sata_read(begin_sector, 1, buffer);
            memcpy(parts_table, buffer+446, 32);
            ASSERT(*(buffer+510) == 0x55);
            ASSERT(*(buffer+511) == 0xaa);

            disk->logical_parts[disk->lpart_num].start_sector = begin_sector + parts_table[0].sectors_offset;
            disk->logical_parts[disk->lpart_num].sector_count = parts_table[0].sectors_count;
            disk->lpart_num++;

            sub_extend_part_offset = parts_table[1].sectors_offset;

            if(parts_table[1].part_type == 0x00){
                break;
            }
        }    

        return;
    }
}

void print_disk_parts(struct disk *disk){
    for(uint32_t i = 0; i < disk->mpart_num; i++){
        printk("master-part %x :    ", i);
        printk("start-sector: %x    sector-count: %x\n", disk->master_parts[i].start_sector, disk->master_parts[i].sector_count);
    }

    for(uint32_t i = 0; i < disk->lpart_num; i++){
        printk("logical-part %x :     ", i);
        printk("start-sector: %x    sector-count: %x\n", disk->logical_parts[i].start_sector, disk->logical_parts[i].sector_count);
    }
}


void hd_init(){
    uint8_t *hd_num = (uint8_t*)(0x475);
    printk("hd num: %x\n", *hd_num);

    ASSERT(*hd_num != 0);
    
    enable_irq(IRQ2_SLAVE, IRQ_MASTER);
    enable_irq(IRQ14_HARDDISK, IRQ_SLAVE);
    // enable_irq(IRQ15_HARDDISK, IRQ_SLAVE);

    // we support only one disk now.
    strcpy(sata0_master.name, "sata0-master");

    detect_disk_parts(&sata0_master);
    print_disk_parts(&sata0_master);

}


// handle hard disk interrupt
// this function will run in kernel mode(ring0)
void handler_harddisk(){
    printk("handler hard disk!\n");
}

// handle 'int 0x80' to get hd identify.
// run in ring0
void sys_hd_identify(){
    
    sata_wait();
    sata_exec_command(COMMAND_IDENTIFY, 0, 0);

    if(sata_check_status()){
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

int32_t sata_read(uint32_t begin_sector, uint8_t sector_count, void *buffer){

    sata_wait();
    sata_exec_command(COMMAND_READ, begin_sector, sector_count);

    if(sata_check_status()){
        printk("Read error: status register's error=1.\n");
        return -1;
    }
    insw(HD_PORT_PRIMARY_DATA, buffer, sector_count * 512 / 2);

    ASSERT((inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_DRQ) == 0);
    return sector_count * 512;
}

int32_t sata_write(uint32_t begin_sector, uint8_t sector_count, void *buffer){
    
    sata_wait();
    sata_exec_command(COMMAND_WRITE, begin_sector, sector_count);

    if(sata_check_status()){
        printk("Write error: status register's error=1.\n");
        return -1;
    }

    outsw(HD_PORT_PRIMARY_DATA, buffer, sector_count * 512 / 2);
    return sector_count * 512;
}



void sata_wait(){
    while(inb(HD_PORT_PRIMARY_STATUS_COMMAND) & STATUS_BUSY){}
}
static void sata_exec_command(uint8_t command, uint32_t begin_sector, uint8_t sector_count){
    outb(HD_PORT_PRIMARY_ERROR_FEATURES, 0);
    outb(HD_PORT_PRIMARY_SECTOR_COUNT, sector_count);
    outb(HD_PORT_PRIMARY_LBA_LOW, (uint8_t)(begin_sector));
    outb(HD_PORT_PRIMARY_LBA_MID, (uint8_t)(begin_sector >> 8));
    outb(HD_PORT_PRIMARY_LBA_HIGH, (uint8_t)(begin_sector >> 16));
    outb(HD_PORT_PRIMARY_DEVICE, MAKE_DEVICE(DEVICE_LBA_MODE, DEVICE_DRV_MASTER, (((uint8_t)(begin_sector >> 24)) & 0x0f)));
    
    outb(HD_PORT_PRIMARY_STATUS_COMMAND, command);
}

// if error-> return 1
// else -> return 0
static int32_t sata_check_status(){
    uint8_t status = inb(HD_PORT_PRIMARY_STATUS_COMMAND);
    while((status & STATUS_BUSY) || !(status & STATUS_DRQ)){
        status = inb(HD_PORT_PRIMARY_STATUS_COMMAND);
    }
    return status & STATUS_ERROR;
}