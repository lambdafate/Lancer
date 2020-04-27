#include "stdint.h"
#include "fs_api.h"
#include "bitmap.h"
#include "schedule.h"
#include "dir.h"
#include "hd.h"
#include "fs.h"
#include "debug.h"


void print_entrys(){
    uint32_t num = inode_table[0].i_size / sizeof(struct dir_entry);
    struct dir_entry *entry = dir_buffer;
    while(num--){
        printk("i-no: %x i-name: %s  i-type: %x\n", entry->i_no, entry->name, entry->file_type);
        entry++;
    }
}


struct inode *create_new_inode(){
    int32_t index = bitmap_malloc(&inode_bitmap, 1);
    if(index == -1){
        return NULL;
    }
    struct inode *inode = inode_table + index;
    ASSERT(inode->i_nlink == 0);
    inode->i_no = index;
    inode->i_size = 0;
    inode->i_nlink = 1;
    inode->i_open_times = 0;
    inode->i_write_deny = 0;
    ASSERT(inode->i_blocks[0] == 0);

    return inode;
}

struct file_descriptor *find_fd(){
    uint32_t i;   
    for (i = 3; i < FD_TABLE_MAX_SIZE; i++){
        if(fd_table[i].f_inode == NULL){
            break;
        }
    }
    return i == FD_TABLE_MAX_SIZE ? NULL: &fd_table[i];
}

int32_t dir_add_item(struct inode *dir_inode, uint8_t *filepath, struct inode *file_inode){
    ASSERT(strlen(filepath) < MAX_FILE_NAME_LENGTH);
    ASSERT(dir_inode->i_size + sizeof(struct dir_entry) <= BLOCK_SIZE * INODE_MAX_BLOCKS_NUM);
    uint32_t index = dir_inode->i_size / sizeof(struct dir_entry);
    struct dir_entry *entry = dir_buffer + index;
    strcpy(entry->name, filepath);
    entry->file_type = FT_REGULAR;
    entry->i_no = file_inode->i_no;

    dir_inode->i_size += sizeof(struct dir_entry);
}



int32_t open_file(TASK *task, uint8_t *filepath, uint32_t flags){
    ASSERT(task != NULL);
    if(strlen(filepath) == 0){
        return -1;
    }
    uint32_t i;
    for (i  = 0; i  < TASK_MAX_FD_NUM;  i++){
        if(task->fds[i] == NULL){
            break;
        }
    }
    if(i == TASK_MAX_FD_NUM){
        return -1;
    }
    task->fds[i] = find_fd();
    if(task->fds[i] == NULL){
        return -1;
    }
    task->fds[i]->f_inode = create_new_inode();
    if(task->fds[i]->f_inode == NULL){
        return -1;
    }
    
    dir_add_item(inode_table, filepath, task->fds[i]->f_inode);
    task->fds[i]->f_pos = 0;
    task->fds[i]->f_mode = flags;

    return i;
}


void sync(void *memory, uint32_t bytes, uint32_t start_lba){
    sata_write(start_lba, bytes/SECTOR_SIZE, memory);
}
