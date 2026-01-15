#include "mc/io.h"
#include "mc/utils.h"
#include <stdio.h>

#define CHECK_IO(io)                                            \
    do                                                          \
    {                                                           \
        MC_ASSERT(io != NULL);                                  \
        MC_ASSERT(io->state->is_initialized == MC_INITIALIZED); \
    } while (0)

// Helper: convert mc_io_status_t to mc_status_t
static mc_status_t convert_status(uint8_t status)
{
    if (status & MC_IO_STATUS_ERROR)
    {
        return MC_ERROR;
    }
    if (status & MC_IO_STATUS_NO_RESPONSE)
    {
        return MC_ERROR_NO_RESPONSE;
    }
    if (status & MC_IO_STATUS_HW_BUSY || status & MC_IO_STATUS_RX_BUSY)
    {
        return MC_ERROR_BUSY;
    }
    if (status & MC_IO_STATUS_RX_OVERFLOW)
    {
        return MC_ERROR_NO_RESOURCE;
    }
    return MC_OK;
}

// Helper: update status while conserving RX_OVERFLOW bit
static void update_status(const mc_io_t *io)
{
    uint8_t rx_overflow_bit = io->state->status & MC_IO_STATUS_RX_OVERFLOW;
    io->state->status = io->driver->get_status(io->ctx) | rx_overflow_bit;
}

void mc_io_init(const mc_io_t *io)
{
    MC_ASSERT(io != NULL);
    io->state->status = MC_IO_STATUS_OK;
    io->state->rx_index = 0;
    io->state->is_initialized = MC_INITIALIZED;
    // Init hw if exists.
    if (io->driver->init)
    {
        io->driver->init(io->ctx);
    }
}

mc_status_t mc_io_printf(const mc_io_t *io, const char *format, ...)
{
    mc_status_t ret;
    va_list args;
    va_start(args, format);
    ret = mc_io_vprintf(io, format, args);
    va_end(args);
    return ret;
}

mc_status_t mc_io_vprintf(const mc_io_t *io, const char *format, va_list args)
{
    CHECK_IO(io);
    MC_ASSERT(format != NULL);

    mc_status_t ret = MC_OK;

    int len = vsnprintf(io->config->tx_buffer, io->config->tx_buffer_len,
                        format, args);
    if (len > 0)
    {
        uint32_t write_len = (uint32_t)((len < io->config->tx_buffer_len)
                                            ? len
                                            : (io->config->tx_buffer_len - 1));
        ret = mc_io_write(io, io->config->tx_buffer, write_len);
    }
    return ret;
}

mc_status_t mc_io_write(const mc_io_t *io, const char *data, uint32_t len)
{
    CHECK_IO(io);
    MC_ASSERT(data != NULL);
    if (io->driver->write_char == NULL)
    {
        return MC_ERROR_NOT_SUPPORTED;
    }

    for (uint32_t i = 0; i < len; i++)
    {
        if (!io->driver->write_char(io->ctx, data[i]))
        {
            break;
        }
    }

    update_status(io);
    return convert_status(io->state->status);
}

mc_status_t mc_io_update(const mc_io_t *io)
{
    CHECK_IO(io);
    if (io->driver->read_char == NULL)
    {
        return MC_ERROR_NOT_SUPPORTED;
    }

    char c;
    bool was_overflow = io->state->status & MC_IO_STATUS_RX_OVERFLOW;
    bool is_overflow = false;
    bool line_ready = false;
    while (io->driver->read_char(io->ctx, &c))
    {
        bool is_delimiter = c == '\n' || c == '\r';
        // If line text mode and delimiter found.
        if (io->state->mode == MC_IO_MODE_TEXT_LINE && is_delimiter)
        {
            // Set line_ready if other characters are already in the buffer.
            if (io->state->rx_index > 0)
            {
                io->config->rx_buffer[io->state->rx_index] = '\0';
                line_ready = true;
                break;
            }
        }
        // Can still add character to RX buffer.
        else if (io->state->rx_index < (io->config->rx_buffer_len - 1))
        {
            io->config->rx_buffer[io->state->rx_index++] = c;
        }
        // RX buffer full, set overflow bit. Continue iteration until no more
        // characters are available.
        else
        {
            io->state->status |= MC_IO_STATUS_RX_OVERFLOW;
            is_overflow = true;
        }

        // Always set line_ready if binary streaming mode
        if (io->state->mode == MC_IO_MODE_BINARY_STREAM)
        {
            line_ready = true;
        }
    }

    // Event Dispatch
    if (line_ready)
    {
        // Prepare event data payload
        mc_io_event_data_t event_data;
        event_data.message = io->config->rx_buffer;
        event_data.length = io->state->rx_index;
        mc_event_trigger(&io->state->rx_event, &event_data);

        io->state->rx_index = 0;
        // Reset RX overflow bit only if last message was overflow, but this
        // message is no longer overflow.
        if (was_overflow && !is_overflow)
        {
            io->state->status &= ~MC_IO_STATUS_RX_OVERFLOW;
        }
    }

    update_status(io);
    return convert_status(io->state->status);
}

uint8_t mc_io_get_status(const mc_io_t *io)
{
    if (!io || io->state->is_initialized != MC_INITIALIZED)
    {
        return MC_IO_STATUS_ERROR;
    }
    return io->state->status;
}

void mc_io_register_rx_callback(const mc_io_t *io, mc_callback_t *callback)
{
    CHECK_IO(io);
    MC_ASSERT(callback != NULL);

    mc_event_register(&io->state->rx_event, callback);
}

void mc_io_set_mode(const mc_io_t *io, mc_io_mode_t mode)
{
    io->state->mode = mode;
}