#ifndef _LIB_STDINT_H
#define _LIB_STDINT_H

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define bool    uint8_t
#define true    1
#define false   0

#define NULL    ((void*)0)

#define DEBUG

#endif