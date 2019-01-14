#ifndef SYSTEMDEFINITION_H_
#define SYSTEMDEFINITION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Systems.h"

/* System struct */
typedef struct System
{
    uint8 id;
    int8 state;
    float targets[TARGET_NUM];
    float values[VALUE_NUM];
    STATUS (*setState)(System* sys, int8 state);
    STATUS (*setTarget)(System* sys, uint8 num, float value);
} System;

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMDEFINITION_H_ */
