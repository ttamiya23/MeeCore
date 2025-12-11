#include "mc/status.h"

bool mc_status_is_critical(mc_status_t status)
{
    switch (status)
    {
    // Success is not critical
    case MC_OK:
        return false;

    // --- RECOVERABLE ---
    case MC_ERROR:
    case MC_ERROR_BUSY:
    case MC_ERROR_TIMEOUT:
    case MC_ERROR_NO_RESPONSE:
        return false;

    // --- CRITICAL ---
    case MC_ERROR_INVALID_ARGS:
    case MC_ERROR_NO_MEM:
    case MC_ERROR_NOT_SUPPORTED:
        return true;

    default:
        return false;
    }
}

const char *mc_status_to_string(mc_status_t status)
{
    switch (status)
    {
    case MC_OK:
        return "OK";
    case MC_ERROR:
        return "GENERIC_ERROR";
    case MC_ERROR_INVALID_ARGS:
        return "INVALID_ARGS";
    case MC_ERROR_BUSY:
        return "BUSY";
    case MC_ERROR_TIMEOUT:
        return "TIMEOUT";
    case MC_ERROR_NO_MEM:
        return "NO_MEM";
    case MC_ERROR_NOT_SUPPORTED:
        return "NOT_SUPPORTED";
    case MC_ERROR_NO_RESPONSE:
        return "NO_RESPONSE";
    default:
        return "UNKNOWN";
    }
}