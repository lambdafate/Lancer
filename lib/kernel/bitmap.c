#include "stdint.h"
#include "string.h"
#include "bitmap.h"
#include "debug.h"


void bitmap_init(BITMAP* bitmap){
    ASSERT(bitmap != NULL);
    memory_set(bitmap->bits, bitmap->size, 0);
}


bool bit_status(BITMAP* bitmap, uint32_t bit_index){

    return 0;
}

int bitmap_malloc(BITMAP* bitmap, uint32_t bit_count){

}

void bit_set(BITMAP* bitmap, uint8_t value){

}