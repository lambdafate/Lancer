/*  file: /fs/fs.c
    a simple file system

        author: lambdafate
        date: 2020/04/25 21:53
*/


#include "stdint.h"
#include "super_block.h"
#include "dir.h"
#include "inode.h"
#include "hd.h"
#include "string.h"
#include "print.h"
#include "debug.h"


void fs_test(){
    // test master0 partition
    uint8_t *buffer = (uint8_t*)(0xc0000000);
    sata_read(sata0_master.master_parts[0].start_sector+1, 1, buffer);

    struct super_block *sb = (struct super_block*)(buffer);
    ASSERT(sb->magic = FS_MAGIC);
    ASSERT(sb->root_inode == 0);

    sata_read(sb->inode_table_lba, 1, buffer);
    struct inode *inodes = (struct inode*)(buffer);
    ASSERT(inodes->i_no == 0);
    ASSERT(inodes->i_size == sizeof(struct dir_entry)*2);

    sata_read(inodes->i_blocks[0], 1, buffer);
    struct dir_entry *des = (struct dir_entry*)(buffer);
    printk("/\n");
    for(uint32_t i=0; i < 2; i++){
        printk("name: %s   i_no: %x    file-type: %x\n", des->name, des->i_no, des->file_type);
        des++;
    }

    while(1){

    }
}

void fs_init(){
    struct super_block *sb = (struct super_block*)(0xc0000000);
    
    for(uint32_t i = 0; i < 1; i++){
        sata_read(sata0_master.master_parts[i].start_sector+1, 1, sb);
        if(sb->magic != FS_MAGIC){
            format_part(&sata0_master.master_parts[i]);
        }
    }
    /*
    for(uint32_t i = 0; i < sata0_master.lpart_num; i++){
        sata_read(sata0_master.logical_parts[i].start_sector+1, 1, sb);
        if(sb->magic != FS_MAGIC){
            format_part(&sata0_master.logical_parts[i]);
        }
    }
    */

   fs_test();
}


void format_part(struct partition *part){
    struct super_block *sb = (struct super_block*)(0xc0000000);
    uint32_t inode_bitmap_sectors = (PART_MAX_FILES_NUM / 8) / SECTOR_SIZE;
    uint32_t inode_table_sectors = (inode_bitmap_sectors * SECTOR_SIZE * 8 * sizeof(struct inode)) / SECTOR_SIZE;
    uint32_t free_sectors = part->sector_count - 2 - inode_bitmap_sectors - inode_table_sectors;
    uint32_t block_bitmap_sectors = free_sectors / (BLOCK_SIZE * 8 + 1);

    sb->magic = FS_MAGIC;
    sb->sector_num = part->sector_count;
    sb->inode_num = inode_bitmap_sectors * SECTOR_SIZE * 8;

    sb->part_lba = part->start_sector;
    
    sb->block_bitmap_lba = sb->part_lba + 2;            // part_base+OBR+SUPER_BLOCK
    sb->block_bitmap_sectors = block_bitmap_sectors;

    sb->inode_bitmap_lba = sb->block_bitmap_lba + sb->block_bitmap_sectors;
    sb->inode_bitmap_sectors = inode_bitmap_sectors;

    sb->inode_table_lba = sb->inode_bitmap_lba + sb->inode_bitmap_sectors;
    sb->inode_table_sectors = inode_table_sectors;

    sb->data_lba = sb->inode_table_lba + sb->inode_table_sectors;

    sb->root_inode = 0;
    sb->dir_entry_size = sizeof(struct dir_entry);
    

    uint32_t super_block_lba  = sb->part_lba + 1;
    uint8_t *buffer = (uint8_t*)(0xc0000200);
    memset(buffer, SECTOR_SIZE, 0);

    // write super block
    sata_write(super_block_lba, 1, sb);

    // write block_bitmap
    *buffer = 1;
    sata_write(sb->block_bitmap_lba, 1, buffer);
    *buffer = 0;
    for(uint32_t i = 1; i < sb->block_bitmap_sectors; i++){
        sata_write(sb->block_bitmap_lba+i, 1, buffer);
    }

    // write inode_bitmap
    *buffer = 1;
    sata_write(sb->inode_bitmap_lba, 1, buffer);
    *buffer = 0;
    for(uint32_t i = 1; i < sb->inode_bitmap_sectors; i++){
        sata_write(sb->inode_bitmap_lba+i, 1, buffer);
    }

    // write inode table
    struct inode *root = (struct inode*)(buffer);
    root->i_no = 0;
    root->i_size = sb->dir_entry_size * 2;          // root dir: . and ..
    root->i_blocks[0] = sb->data_lba;
    sata_write(sb->inode_table_lba, 1, buffer);
    for(uint32_t i = 1; i < sb->inode_table_sectors; i++){
        sata_write(sb->inode_table_lba+i, 1, buffer);
    }

    // write (. and .. dir entry) in root dir
    memset(buffer, SECTOR_SIZE, 0);
    struct dir_entry *de = (struct dir_entry*)(buffer);
    de->file_type = FT_DIRECTORY;
    strcpy(de->name, ".");
    de->i_no = 0;
    de++;                       // .................. bang bang bang
    de->i_no = 0;
    strcpy(de->name, "..");
    de->file_type = FT_DIRECTORY;

    sata_write(sb->data_lba, 1, buffer); 
}





