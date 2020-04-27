#ifndef _FS_FS_API_H
#define _FS_FS_API_H
#include "stdint.h"
#include "inode.h"
#include "schedule.h"

// int32_t open(uint8_t *filepath, uint32_t flags);

struct inode *create_new_inode();
int32_t open_file(TASK *task, uint8_t *filepath, uint32_t flags);

void print_entrys();

#endif