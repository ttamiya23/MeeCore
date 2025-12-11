#ifndef MC_STATUS_H_
#define MC_STATUS_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Standard Status Codes for MeeCore
 */
typedef enum {
    MC_OK = 0,                  // Success (Always 0)
    MC_ERROR = -1,              // Generic Error
    MC_ERROR_INVALID_ARGS = -2, // NULL pointer or bad input
    MC_ERROR_BUSY = -3,         // Resource is locked/busy
    MC_ERROR_TIMEOUT = -4,      // Operation timed out (Software timer expired)
    MC_ERROR_NO_MEM = -5,       // Malloc failed or buffer full
    MC_ERROR_NOT_SUPPORTED = -6,// Feature not implemented on this MCU
    MC_ERROR_NO_RESPONSE = -7   // Peripheral did not acknowledge (NACK) or is disconnected
} mc_status_t;

/**
 * @brief Checks if an error is severe enough to warrant a system halt/reset.
 * * @details
 * * **CRITICAL (Returns true):**
 * These errors indicate a bug in the firmware logic, memory corruption, 
 * or a misconfiguration that runtime logic cannot fix.
 * - MC_ERROR_INVALID_ARGS:  Programmer error (e.g., passing NULL).
 * - MC_ERROR_NO_MEM:        Heap exhaustion or buffer under-provisioning.
 * - MC_ERROR_NOT_SUPPORTED: Invalid system configuration.
 * * **RECOVERABLE (Returns false):**
 * These errors are environmental or transient. The system should 
 * log the warning and retry the operation.
 * - MC_OK:                  Success is never critical.
 * - MC_ERROR:               Generic runtime failure.
 * - MC_ERROR_BUSY:          Resource contention (wait and retry).
 * - MC_ERROR_TIMEOUT:       Timing glitch (watchdog kick or retry).
 * - MC_ERROR_NO_RESPONSE:   Hardware disconnected or sleeping.
 * * @param status The status code to check.
 * @return true if the error is CRITICAL (System should Assert/Reset).
 * @return false if the error is RECOVERABLE (System should Retry).
 */
bool mc_status_is_critical(mc_status_t status);

/**
 * @brief Converts status code to string for logging.
 */
const char* mc_status_to_string(mc_status_t status);

#endif /* MC_STATUS_H_ */