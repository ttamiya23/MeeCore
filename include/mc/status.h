#ifndef MC_STATUS_H_
#define MC_STATUS_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Standard Status Codes for MeeCore
 */
typedef enum
{
    MC_OK = 0,                     // Success (Always 0)
    MC_ERROR = -1,                 // Generic Error
    MC_ERROR_INVALID_ARGS = -2,    // NULL pointer or bad input
    MC_ERROR_BUSY = -3,            // Resource is locked/busy
    MC_ERROR_TIMEOUT = -4,         // Operation timed out (Software timer expired)
    MC_ERROR_NO_MEM = -5,          // Malloc failed or heap full
    MC_ERROR_NOT_SUPPORTED = -6,   // Feature not implemented on this MCU
    MC_ERROR_NO_RESPONSE = -7,     // Peripheral did not acknowledge (NACK) or is disconnected
    MC_ERROR_NO_RESOURCE = -8,     // The fixed resource (buffer) limit was hit.
    MC_ERROR_CHECKSUM = -9,        // CRC/Parity mismatch (Data corrupted)
    MC_ERROR_OVERRUN = -10,        // HW FIFO overflow (Data lost)
    MC_ERROR_HARDWARE_FAULT = -11, // Internal peripheral failure (Clock/DMA/Brownout)
    MC_ERROR_WRITE_PROTECTED = -12 // Flash/EEPROM is locked
} mc_status_t;

/**
 * @brief Converts status code to string for logging.
 */
const char *mc_status_to_string(mc_status_t status);

#define MC_RETURN_IF_ERROR(x)   \
    do                          \
    {                           \
        mc_status_t _err = (x); \
        if (_err != MC_OK)      \
        {                       \
            return _err;        \
        }                       \
    } while (0)

#endif /* MC_STATUS_H_ */