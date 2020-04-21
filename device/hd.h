#ifndef _DEVICE_HD_H
#define _DEVICE_HD_H

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
#define MAKE_DEVICE(lba, drv, hs)               (0xa0 | (lba << 6) | (drv << 4) | (hs & 0x0f))

#define COMMAND_IDENTIFY                        0xec

void hd_init();
void handler_harddisk();
void sys_hd_identify();

#endif