#include "mc/status.h"

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
    case MC_ERROR_NO_RESOURCE:
        return "NO_RESOURCE";
    default:
        return "UNKNOWN";
    }
}