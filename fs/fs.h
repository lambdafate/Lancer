#ifndef _FS_FS_H
#define _FS_FS_H
#include "stdint.h"

// per part's max files
#define PART_MAX_FILES_NUM              4096

#define SECTOR_SIZE                     512
#define BLOCK_SIZE                      SECTOR_SIZE

#define FS_MAGIC                        0x12345678


enum file_types{
    FT_REGULAR, FT_DIRECTORY, FT_UNKNOWN
};


void fs_init();



#endif