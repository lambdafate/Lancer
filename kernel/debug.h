#ifndef _KERNEL_DEBUG_H
#define _KERNEL_DEBUG_H
#include "stdint.h"

#ifdef DEBUG
    void panic(uint8_t* condition, uint8_t* basefile, uint8_t* file, const uint8_t* function , uint32_t line);
    #define ASSERT(condition)   \
        if(condition){}     \
        else{       \
            panic(#condition, __BASE_FILE__, __FILE__, __FUNCTION__ , __LINE__); \ 
        }
#else 
    #define ASSERT(condition)   \
        (0)

#endif      // endif DEBUG


#endif      // endif _KERNEL_DEBUG_H