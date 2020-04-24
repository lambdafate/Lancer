#include "stdint.h"
#include "print.h"
#include "string.h"
#include "bitmap.h"
#include "debug.h"


void bitmap_init(BITMAP* bitmap){
    ASSERT(bitmap != NULL && bitmap->bits != NULL);
    memory_set(bitmap->bits, bitmap->size / 8, 0);
}


uint8_t bitmap_status(BITMAP* bitmap, uint32_t bit_index){
    ASSERT(bitmap != NULL && bitmap->bits != NULL);
    ASSERT(bit_index < bitmap->size);

    uint32_t index = bit_index / 8;
    uint32_t bit = bit_index % 8;

    // printk("\nbt_index: %x, index: %x, bit: %x, res: %x, value: %x, mask: %x\n ", bit_index, index, bit, bitmap->bits[index] & (BIT_MASK << bit), bitmap->bits[index], BIT_MASK<<bit);

    return (bitmap->bits[index] & (BIT_MASK << bit)) ? BIT_OCCUPIED: BIT_FREE;
}


int32_t bitmap_malloc(BITMAP* bitmap, uint32_t bit_count){
    ASSERT(bitmap != NULL && bitmap->bits != NULL);

    if(bit_count > bitmap->size || bit_count == 0){
        return -1;
    }
    uint32_t bit_index = 0;
    uint32_t res = 0;
    while(bit_index < bitmap->size){
        if(bitmap_status(bitmap, bit_index) == BIT_OCCUPIED){
            bit_index++; 
            continue;
        }
        uint32_t i = bit_index;
        uint32_t findit = bit_index + bit_count;
        for(; i<findit; i++){
            if(bitmap_status(bitmap, i) == BIT_OCCUPIED){ break; }
        }
        if(i == findit){
            res = bit_index;
            break;
        }
        bit_index = i + 1;
    }
    if(bit_index >= bitmap->size){
        return -1;
    }
    for(uint32_t i=res; i<res+bit_count; i++){
        bitmap_set(bitmap, i, BIT_OCCUPIED);
    }
    return res;
}


void bitmap_set(BITMAP* bitmap, uint32_t bit_index, uint8_t value){
    ASSERT(bitmap != NULL && bitmap->bits != NULL);
    ASSERT(bit_index < bitmap->size);
    ASSERT(value == BIT_OCCUPIED || value == BIT_FREE);

    uint32_t index = bit_index / 8;
    uint8_t  mask  = BIT_MASK << (bit_index % 8);
    if(value == BIT_OCCUPIED){
        bitmap->bits[index] |=  mask; 
    }else{
        bitmap->bits[index] &= ~mask;
    }
}