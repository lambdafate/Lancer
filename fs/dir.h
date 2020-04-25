#ifndef _FS_DIR_H
#define _FS_DIR_H
#include "stdint.h"
#include "inode.h"
#include "fs.h"

#define MAX_FILE_NAME_LENGTH        16

struct dir{
    struct inode *inode;
    uint32_t dis_pos;
    uint8_t buffer[512];
};

struct dir_entry{
    uint8_t name[MAX_FILE_NAME_LENGTH];
    uint32_t i_no;    
    enum file_types file_type;
};

#endif