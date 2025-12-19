#ifndef MC_STATUS_H_
#define MC_STATUS_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Standard Status Codes for MeeCore
 */
typedef enum
{
    MC_OK = 0,                   // Success (Always 0)
    MC_ERROR = -1,               // Generic Error
    MC_ERROR_INVALID_ARGS = -2,  // NULL pointer or bad input
    MC_ERROR_BUSY = -3,          // Resource is locked/busy
    MC_ERROR_TIMEOUT = -4,       // Operation timed out (Software timer expired)
    MC_ERROR_NO_MEM = -5,        // Malloc failed or heap full
    MC_ERROR_NOT_SUPPORTED = -6, // Feature not implemented on this MCU
    MC_ERROR_NO_RESPONSE = -7,   // Peripheral did not acknowledge (NACK) or is disconnected
    MC_ERROR_NO_RESOURCE = -8,   // The fixed resource (buffer) limit was hit.
} mc_status_t;

/**
 * @brief Converts status code to string for logging.
 */
const char *mc_status_to_string(mc_status_t status);

#endif /* MC_STATUS_H_ */