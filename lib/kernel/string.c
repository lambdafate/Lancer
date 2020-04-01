#include "stdint.h"


void memory_set(uint8_t* _destination, uint32_t size, uint8_t value){
    uint32_t index = 0;
    uint8_t* destination = _destination;
    while(index++ < size){
        *destination++ = value;
    }
}