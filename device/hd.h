#ifndef _DEVICE_HD_H
#define _DEVICE_HD_H
#include "stdint.h"

#define HD_PORT_PRIMARY_DATA                    0x1f0
#define HD_PORT_PRIMARY_ERROR_FEATURES          0x1f1
#define HD_PORT_PRIMARY_SECTOR_COUNT            0x1f2
#define HD_PORT_PRIMARY_LBA_LOW                 0x1f3
#define HD_PORT_PRIMARY_LBA_MID                 0x1f4
#define HD_PORT_PRIMARY_LBA_HIGH                0x1f5
#define HD_PORT_PRIMARY_DEVICE                  0x1f6
#define HD_PORT_PRIMARY_STATUS_COMMAND          0x1f7

#define HD_PORT_PRIMARY_CONTROL                 0x3f6


#define DEVICE_LBA_MODE                         1
#define DEVICE_DRV_MASTER                       0
#define DEVICE_DRV_SLAVE                        1
#define MAKE_DEVICE(lba, drv, hs)               (0xa0 | ((lba) << 6) | ((drv) << 4) | ((hs) & 0x0f))

#define COMMAND_IDENTIFY                        0xec
#define COMMAND_READ                            0x20
#define COMMAND_WRITE                           0x30
#define COMMAND_FLUSH                           0xe7

// status register
#define STATUS_BUSY                             0b10000000
#define STATUS_READY                            0b01000000
#define STATUS_DSC                              0b00010000
#define STATUS_DRQ                              0b00001000
#define STATUS_ERROR                            0b00000001

#define LOGICAL_PARTITION_MAX_NUM               8


struct partition{
    uint8_t name[16];
    uint32_t start_sector;             
    uint32_t sector_count;
};

struct disk{
    uint8_t name[16];
    uint32_t mpart_num;
    uint32_t lpart_num;
    struct partition master_parts[4];
    struct partition logical_parts[LOGICAL_PARTITION_MAX_NUM];
};



// partition struction in MBR and EBR
struct part{
    uint32_t boot : 8;      // boot indicator, 0x80=bootable, 0=no bootable, else=illegal
    uint32_t start_head : 8;
    uint32_t start_sector : 6;
    uint32_t start_cylinder : 10;
    uint32_t part_type : 8;
    uint32_t end_head : 8;
    uint32_t end_sector : 6;
    uint32_t end_cylinder : 10;
    uint32_t sectors_offset : 32;
    uint32_t sectors_count : 32;
}__attribute__((packed));


extern struct disk sata0_master;

void hd_init();
void handler_harddisk();

void sys_hd_identify();
int32_t sata_read(uint32_t begin_sector, uint8_t count, void *buffer);
int32_t sata_write(uint32_t begin_sector, uint8_t sector_count, void *buffer);


#endif