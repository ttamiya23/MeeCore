#ifndef RANDOM_H_
#define RANDOM_H_

#include "util.h"

/* Initialize random number generator */
STATUS rnd_Initialize(void);

/* Initialize using seed */
STATUS rnd_InitializeSeed(uint32 seed);

/* Get random number between start (inclusive) and end (exclusive) */
STATUS rnd_Random(int32 start, int32 end, int32* result);

#endif /* RANDOM_H_ */
