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


#define DEVICE_LBA_MODE                         0
#define DEVICE_DRV_MASTER                       0
#define DEVICE_DRV_SLAVE                        1
#define MAKE_DEVICE(lba, drv, hs)               (0xa0 | ((lba) << 6) | ((drv) << 4) | ((hs) & 0x0f))

#define COMMAND_IDENTIFY                        0xec
#define COMMAND_READ                            0x20
#define COMMAND_WRITE                           0x30
#define COMMAND_FLUSH                           0xe7

// status register
#define STATUS_BUSY                        10000000
#define STATUS_READY                       01000000
#define STATUS_DSC                         00010000
#define STATUS_DRQ                         00001000
#define STATUS_ERROR                       00000001



struct partition{
    uint32_t start_sector;
    uint32_t sector_count;
    uint8_t name[16];            
};

struct disk{
    uint8_t name[16];
    struct partition master_parts[4];
    struct partition logical_parts[8];
};


void hd_init();
void handler_harddisk();
void sys_hd_identify();
int32_t sata_read(uint32_t begin_sector, uint8_t count, void *buffer);
int32_t sata_write(uint32_t begin_sector, uint8_t sector_count, void *buffer);
#endif