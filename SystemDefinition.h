#ifndef SYSTEMDEFINITION_H_
#define SYSTEMDEFINITION_H_

#include "Systems.h"

/* System struct */
typedef struct System
{
    uint8 id;
    int8 state;
    float targets[TARGET_NUM];
    float values[VALUE_NUM];
    STATUS (*setState)(struct System* sys, int8 state);
    STATUS (*setTarget)(struct System* sys, uint8 num, float value);
} System;

#endif /* SYSTEMDEFINITION_H_ */
