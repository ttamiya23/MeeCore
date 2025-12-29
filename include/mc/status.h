#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Standard Status Codes for MeeCore */
typedef enum
{
    MC_OK = 0,                    // Success (Always 0)
    MC_ERROR = 1,                 // Generic Error
    MC_ERROR_INVALID_ARGS = 2,    // NULL pointer or bad input
    MC_ERROR_BUSY = 3,            // Resource is locked/busy
    MC_ERROR_TIMEOUT = 4,         // Operation timed out (Software timer expired)
    MC_ERROR_NO_MEM = 5,          // Malloc failed or heap full
    MC_ERROR_NOT_SUPPORTED = 6,   // Feature not implemented on this MCU
    MC_ERROR_NO_RESPONSE = 7,     // Peripheral did not acknowledge (NACK) or is disconnected
    MC_ERROR_NO_RESOURCE = 8,     // The fixed resource (buffer) limit was hit.
    MC_ERROR_CHECKSUM = 9,        // CRC/Parity mismatch (Data corrupted)
    MC_ERROR_OVERRUN = 10,        // HW FIFO overflow (Data lost)
    MC_ERROR_HARDWARE_FAULT = 11, // Internal peripheral failure (Clock/DMA/Brownout)
    MC_ERROR_WRITE_PROTECTED = 12 // Flash/EEPROM is locked
} mc_status_t;

/* Struct containing value or error */
typedef struct mc_result_t
{
    bool ok;
    union
    {
        int32_t value;
        mc_status_t error;
    };
} mc_result_t;

// Helper for setting mc_result_t with value
static inline mc_result_t MC_OK_VAL(int32_t val)
{
    mc_result_t res;
    res.ok = true;
    res.value = val;
    return res;
}

// Helper for setting mc_result_t with error
static inline mc_result_t MC_ERR_VAL(mc_status_t err)
{
    mc_result_t res;
    res.ok = false;
    res.error = err;
    return res;
}

// Helper for converting mc_stauts_t to mc_result_t. If ok, value is 0.
static inline mc_result_t MC_STATUS_TO_RESULT(mc_status_t status)
{
    return status == MC_OK ? MC_OK_VAL(0) : MC_ERR_VAL(status);
}

/* Converts status code to string for logging. */
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

#define MC_ASSIGN_OR_RETURN(out_var, expr) \
    int32_t out_var;                       \
    do                                     \
    {                                      \
        mc_result_t _res = (expr);         \
        if (!_res.ok)                      \
        {                                  \
            return _res.error;             \
        }                                  \
        (out_var) = _res.value;            \
    } while (0)
