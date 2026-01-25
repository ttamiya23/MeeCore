#include "mc/communication/stream.h"
#include "mc/utils.h"
#include <stdio.h>

#define CHECK_STREAM(stream)                                        \
    do                                                              \
    {                                                               \
        MC_ASSERT(stream != NULL);                                  \
        MC_ASSERT(stream->state->is_initialized == MC_INITIALIZED); \
    } while (0)

// Helper: convert mc_stream_status_t to mc_status_t
static mc_status_t convert_status(uint8_t status)
{
    if (status & MC_STREAM_STATUS_ERROR)
    {
        return MC_ERROR;
    }
    if (status & MC_STREAM_STATUS_NO_RESPONSE)
    {
        return MC_ERROR_NO_RESPONSE;
    }
    if (status & MC_STREAM_STATUS_HW_BUSY || status & MC_STREAM_STATUS_RX_BUSY)
    {
        return MC_ERROR_BUSY;
    }
    if (status & MC_STREAM_STATUS_RX_OVERFLOW)
    {
        return MC_ERROR_NO_RESOURCE;
    }
    return MC_OK;
}

// Helper: update status while conserving RX_OVERFLOW bit
static void update_status(const mc_stream_t *stream)
{
    uint8_t rx_overflow_bit = stream->state->status & MC_STREAM_STATUS_RX_OVERFLOW;
    stream->state->status = stream->driver->get_status(stream->ctx) |
                            rx_overflow_bit;
}

void mc_stream_init(const mc_stream_t *stream)
{
    MC_ASSERT(stream != NULL);
    stream->state->status = MC_STREAM_STATUS_OK;
    stream->state->rx_index = 0;
    stream->state->is_initialized = MC_INITIALIZED;
    mc_event_init(&stream->state->rx_event);
    // Init hw if exists.
    if (stream->driver->init)
    {
        stream->driver->init(stream->ctx);
    }
}

mc_status_t mc_stream_printf(const mc_stream_t *stream, const char *format, ...)
{
    mc_status_t ret;
    va_list args;
    va_start(args, format);
    ret = mc_stream_vprintf(stream, format, args);
    va_end(args);
    return ret;
}

mc_status_t mc_stream_vprintf(const mc_stream_t *stream, const char *format,
                              va_list args)
{
    CHECK_STREAM(stream);
    MC_ASSERT(format != NULL);

    mc_status_t ret = MC_OK;

    int len = vsnprintf(stream->config->tx_buffer,
                        stream->config->tx_buffer_len, format, args);
    if (len > 0)
    {
        uint32_t write_len = (uint32_t)((len < stream->config->tx_buffer_len)
                                            ? len
                                            : (stream->config->tx_buffer_len - 1));
        ret = mc_stream_write(stream, stream->config->tx_buffer, write_len);
    }
    return ret;
}

mc_status_t mc_stream_write(const mc_stream_t *stream, const char *data, uint32_t len)
{
    CHECK_STREAM(stream);
    MC_ASSERT(data != NULL);
    if (stream->driver->write_char == NULL)
    {
        return MC_ERROR_NOT_SUPPORTED;
    }

    for (uint32_t i = 0; i < len; i++)
    {
        if (!stream->driver->write_char(stream->ctx, data[i]))
        {
            break;
        }
    }

    update_status(stream);
    return convert_status(stream->state->status);
}

mc_status_t mc_stream_update(const mc_stream_t *stream)
{
    CHECK_STREAM(stream);
    if (stream->driver->read_char == NULL)
    {
        return MC_ERROR_NOT_SUPPORTED;
    }

    char c;
    bool was_overflow = stream->state->status & MC_STREAM_STATUS_RX_OVERFLOW;
    bool is_overflow = false;
    bool line_ready = false;
    while (stream->driver->read_char(stream->ctx, &c))
    {
        bool is_delimiter = c == '\n' || c == '\r';
        // If line text mode and delimiter found.
        if (stream->state->mode == MC_STREAM_MODE_TEXT_LINE && is_delimiter)
        {
            // Set line_ready if other characters are already in the buffer.
            if (stream->state->rx_index > 0)
            {
                stream->config->rx_buffer[stream->state->rx_index] = '\0';
                line_ready = true;
                break;
            }
        }
        // Can still add character to RX buffer.
        else if (stream->state->rx_index < (stream->config->rx_buffer_len - 1))
        {
            stream->config->rx_buffer[stream->state->rx_index++] = c;
        }
        // RX buffer full, set overflow bit. Continue iteration until no more
        // characters are available.
        else
        {
            stream->state->status |= MC_STREAM_STATUS_RX_OVERFLOW;
            is_overflow = true;
        }

        // Always set line_ready if binary streaming mode
        if (stream->state->mode == MC_STREAM_MODE_BINARY_STREAM)
        {
            line_ready = true;
        }
    }

    // Event Dispatch
    if (line_ready)
    {
        // Prepare event data payload
        mc_stream_event_data_t event_data;
        event_data.message = stream->config->rx_buffer;
        event_data.length = stream->state->rx_index;
        mc_event_trigger(&stream->state->rx_event, &event_data);

        stream->state->rx_index = 0;
        // Reset RX overflow bit only if last message was overflow, but this
        // message is no longer overflow.
        if (was_overflow && !is_overflow)
        {
            stream->state->status &= ~MC_STREAM_STATUS_RX_OVERFLOW;
        }
    }

    update_status(stream);
    return convert_status(stream->state->status);
}

uint8_t mc_stream_get_status(const mc_stream_t *stream)
{
    if (!stream || stream->state->is_initialized != MC_INITIALIZED)
    {
        return MC_STREAM_STATUS_ERROR;
    }
    return stream->state->status;
}

void mc_stream_register_rx_callback(const mc_stream_t *stream,
                                    mc_callback_t *callback)
{
    CHECK_STREAM(stream);
    MC_ASSERT(callback != NULL);

    mc_event_register(&stream->state->rx_event, callback);
}

void mc_stream_set_mode(const mc_stream_t *stream, mc_stream_mode_t mode)
{
    stream->state->mode = mode;
}