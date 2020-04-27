#ifndef _FS_SUPER_BLOCK_H
#define _FS_SUPER_BLOCK_H
#include "stdint.h"

struct super_block{
    uint32_t magic;

    uint32_t sector_num;         // this partition's all sectors
    uint32_t inode_num;          // this partition's inode number

    uint32_t part_lba;           // this partition's base lba
    
    uint32_t block_bitmap_lba;
    uint32_t block_bitmap_sectors;

    uint32_t inode_bitmap_lba;
    uint32_t inode_bitmap_sectors;

    uint32_t inode_table_lba;
    uint32_t inode_table_sectors;

    uint32_t data_lba;          // data part, allocate from there
    uint32_t root_inode;
    uint32_t dir_entry_size;

    uint8_t padding[460];
}__attribute((packed));


#endif