#include <Arduino.h>
#include "ports/arduino/arduino_serial_io.h"
#include "mc/io.h"

uint8_t arduino_serial_io_get_status(void *ctx)
{
    mc_arduino_serial_io_ctx_t *arduino_ctx = (mc_arduino_serial_io_ctx_t *)ctx;
    if (arduino_ctx->is_connected && !arduino_ctx->is_connected())
    {
        return MC_IO_STATUS_NO_RESPONSE;
    }

    return MC_IO_STATUS_OK;
}

bool arduino_serial_io_write(void *ctx, char c)
{
    if (arduino_serial_io_get_status(ctx) != MC_IO_STATUS_OK)
    {
        return false;
    }

    mc_arduino_serial_io_ctx_t *arduino_ctx = (mc_arduino_serial_io_ctx_t *)ctx;
    Stream *s = (Stream *)arduino_ctx->stream;
    return s->write(c) == 1;
}

bool arduino_serial_io_read(void *ctx, char *c)
{
    if (arduino_serial_io_get_status(ctx) != MC_IO_STATUS_OK)
    {
        return false;
    }

    mc_arduino_serial_io_ctx_t *arduino_ctx = (mc_arduino_serial_io_ctx_t *)ctx;
    Stream *s = (Stream *)arduino_ctx->stream;
    if (s->available() > 0)
    {
        *c = s->read();
        return true;
    }
    return false;
}

extern "C"
{
    extern const mc_io_driver_t mc_arduino_serial_io_driver = {
        .write_char = arduino_serial_io_write,
        .read_char = arduino_serial_io_read,
        .get_status = arduino_serial_io_get_status};
}