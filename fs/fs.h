#ifndef _FS_FS_H
#define _FS_FS_H
#include "stdint.h"
#include "inode.h"
#include "bitmap.h"

// per part's max files
#define PART_MAX_FILES_NUM              4096

#define SECTOR_SIZE                     512
#define BLOCK_SIZE                      SECTOR_SIZE

#define FS_MAGIC                        0x12345678

enum file_types{
    FT_REGULAR, FT_DIRECTORY, FT_UNKNOWN
};


#define FD_TABLE_MAX_SIZE               (240)
struct file_descriptor{
    uint32_t f_mode;
    uint32_t f_pos;
    struct inode * f_inode;
};

struct file_descriptor fd_table[FD_TABLE_MAX_SIZE];

struct super_block *sb;
BITMAP block_bitmap;
BITMAP inode_bitmap;
struct inode *inode_table;


void fs_init();


#endif