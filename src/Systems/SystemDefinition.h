#ifndef SYSTEMDEFINITION_H_
#define SYSTEMDEFINITION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Systems.h"
#include "SystemsSettings.h"

/* System struct */
#pragma pack(1)
typedef struct System
{
    uint8 id;
    int8 state;
    float parameters[PARAMETER_NUM];
    float values[VALUE_NUM];
    const char* help;
    STATUS (*setState)(System* sys, int8 state);
    STATUS (*setParameter)(System* sys, uint8 parameterNum, float parameter);
    STATUS (*update)(System* sys);
} System;

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMDEFINITION_H_ */
