#ifndef UTIL_H_
#define UTIL_H_ 

#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0
#ifndef NULL
    #define NULL 0
#endif /* NULL */
#define SUCCESS 0
#define ERROR 1

#define IS_BIT_SET(value, bit) (value & (1 << bit))
#define SET_BIT(value, bit) value |= (1 << bit)
#define UNSET_BIT(value, bit) value &= ~(1 << bit)
#define TOGGLE_BIT(value, bit) value ^= (1 << bit)

typedef char int8;
typedef short int16; 
typedef int int32; 
typedef long long int64; 
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
//typedef unsigned int bool;

typedef uint8 STATUS;

#endif /* UTIL_H_ */
