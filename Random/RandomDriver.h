#ifndef RANDOMDRIVER_H_
#define RANDOMDRIVER_H_

#include "util.h"

/* Initialize random number generator */
STATUS rndDriver_Initialize(void);

/* Initialize using seed */
STATUS rndDriver_InitializeSeed(uint32 seed);

/* Get random byte */
uint8 rndDriver_GetByte(void);

#endif /* RANDOMDRIVER_H_ */
