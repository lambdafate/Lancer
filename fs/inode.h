#ifndef _FS_INODE_H
#define _FS_INODE_H
#include "stdint.h"

#define INODE_MAX_BLOCKS_NUM        8

struct inode{
    uint32_t i_no;          // inode number, which is index in inode table.
    uint32_t i_size;

    uint32_t i_nlink;

    uint32_t i_open_times;
    uint32_t i_write_deny;

    uint32_t i_blocks[8];  
};


#endif