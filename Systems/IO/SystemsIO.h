#ifndef SYSTEMSIO_H_
#define SYSTEMSIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "Systems/Systems.h"

/* Query the system and output result */
STATUS sysio_Query(uint16 sysId);

/* Get help from system and output result */
STATUS sysio_Help(uint16 sysId);

/* Parse input command and make appropriate system call */
STATUS sysio_ParseCommand(char* command);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMSIO_H_ */
