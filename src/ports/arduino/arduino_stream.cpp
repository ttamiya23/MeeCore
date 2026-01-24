#include <Arduino.h>
#include "ports/arduino/arduino_stream.h"
#include "mc/stream.h"

uint8_t arduino_stream_get_status(void *ctx)
{
    mc_arduino_stream_ctx_t *arduino_ctx = (mc_arduino_stream_ctx_t *)ctx;
    if (arduino_ctx->is_connected && !arduino_ctx->is_connected())
    {
        return MC_STREAM_STATUS_NO_RESPONSE;
    }

    return MC_STREAM_STATUS_OK;
}

bool arduino_stream_write(void *ctx, char c)
{
    if (arduino_stream_get_status(ctx) != MC_STREAM_STATUS_OK)
    {
        return false;
    }

    mc_arduino_stream_ctx_t *arduino_ctx = (mc_arduino_stream_ctx_t *)ctx;
    Stream *s = (Stream *)arduino_ctx->stream;
    return s->write(c) == 1;
}

bool arduino_stream_read(void *ctx, char *c)
{
    if (arduino_stream_get_status(ctx) != MC_STREAM_STATUS_OK)
    {
        return false;
    }

    mc_arduino_stream_ctx_t *arduino_ctx = (mc_arduino_stream_ctx_t *)ctx;
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
    extern const mc_stream_driver_t mc_arduino_stream_driver = {
        .init = NULL,
        .write_char = arduino_stream_write,
        .read_char = arduino_stream_read,
        .get_status = arduino_stream_get_status};
}