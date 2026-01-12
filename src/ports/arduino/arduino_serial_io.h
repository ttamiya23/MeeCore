#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/io.h"

// Macro for defining analog context. Users should always use this.
#define MC_DEFINE_DEFAULT_ARDUINO_SERIAL_IO(NAME) \
    bool NAME##_is_connected()                    \
    {                                             \
        return (bool)Serial;                      \
    }                                             \
                                                  \
    static mc_arduino_serial_io_ctx_t NAME = {    \
        .stream = &Serial,                        \
        .is_connected = NAME##_is_connected}

    // Ctx for arduino serial IO driver
    typedef struct mc_arduino_serial_io_ctx_t
    {
        void *stream;               // Pointer to the Arduino Stream (Serial)
        bool (*is_connected)(void); // Callback to check connection status
    } mc_arduino_serial_io_ctx_t;

    // Driver to Arduino serial IO
    extern const mc_io_driver_t mc_arduino_serial_io_driver;

#ifdef __cplusplus
}
#endif
