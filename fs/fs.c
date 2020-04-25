#include "stdint.h"
#include "super_block.h"
#include "dir.h"
#include "inode.h"
#include "hd.h"

void fs_init(){
    struct super_block *sb = (struct super_block*)(0xc0000000);
    
    for(uint32_t i = 0; i < sata0_master.mpart_num; i++){
        sata_read(sata0_master.master_parts[i].start_sector+1, 1, sb);
        if(sb->magic != FS_MAGIC){
            format_part(&sata0_master.master_parts[i]);
        }
    }

    for(uint32_t i = 0; i < sata0_master.lpart_num; i++){
        sata_read(sata0_master.logical_parts[i].start_sector+1, 1, sb);
        if(sb->magic != FS_MAGIC){
            format_part(&sata0_master.logical_parts[i]);
        }
    }
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

    sata_write(sb->part_lba+1, 1, sb);
}





