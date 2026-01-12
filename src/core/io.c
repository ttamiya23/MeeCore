#include "mc/io.h"
#include "mc/utils.h"
#include <stdio.h>

#define CHECK_IO(io)                                     \
    do                                                   \
    {                                                    \
        MC_ASSERT(io != NULL);                           \
        MC_ASSERT(io->is_initialized == MC_INITIALIZED); \
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
static void update_status(mc_io_t *io)
{
    uint8_t rx_overflow_bit = io->status & MC_IO_STATUS_RX_OVERFLOW;
    io->status = io->driver->get_status(io->driver_ctx) | rx_overflow_bit;
}

void mc_io_init(mc_io_t *io, const mc_io_driver_t *driver,
                void *driver_ctx, char *rx_buffer, uint16_t rx_len,
                char *tx_buffer, uint16_t tx_len)
{
    MC_ASSERT(io != NULL);
    MC_ASSERT(driver != NULL);
    MC_ASSERT(driver->read_char != NULL);
    MC_ASSERT(driver->write_char != NULL);
    MC_ASSERT(driver->get_status != NULL);
    MC_ASSERT(rx_buffer != NULL);
    MC_ASSERT(rx_len > 0);
    MC_ASSERT(tx_buffer != NULL);
    MC_ASSERT(tx_len > 0);

    io->driver = driver;
    io->driver_ctx = driver_ctx;
    io->status = MC_IO_STATUS_OK;
    io->rx_buffer = rx_buffer;
    io->rx_buffer_len = rx_len;
    io->rx_index = 0;
    io->tx_buffer = tx_buffer;
    io->tx_buffer_len = tx_len;
    io->is_initialized = MC_INITIALIZED;

    mc_event_init(&io->rx_event);
}

mc_status_t mc_io_printf(mc_io_t *io, const char *format, ...)
{
    mc_status_t ret;
    va_list args;
    va_start(args, format);
    ret = mc_io_vprintf(io, format, args);
    va_end(args);
    return ret;
}

mc_status_t mc_io_vprintf(mc_io_t *io, const char *format, va_list args)
{
    CHECK_IO(io);
    MC_ASSERT(format != NULL);

    mc_status_t ret = MC_OK;

    int len = vsnprintf(io->tx_buffer, io->tx_buffer_len, format, args);
    if (len > 0)
    {
        uint32_t write_len = (uint32_t)((len < io->tx_buffer_len) ? len : (io->tx_buffer_len - 1));
        ret = mc_io_write(io, io->tx_buffer, write_len);
    }
    return ret;
}

mc_status_t mc_io_write(mc_io_t *io, const char *data, uint32_t len)
{
    CHECK_IO(io);
    MC_ASSERT(data != NULL);

    for (uint32_t i = 0; i < len; i++)
    {
        if (!io->driver->write_char(io->driver_ctx, data[i]))
        {
            break;
        }
    }

    update_status(io);
    return convert_status(io->status);
}

mc_status_t mc_io_update(mc_io_t *io)
{
    CHECK_IO(io);

    char c;
    bool was_overflow = io->status & MC_IO_STATUS_RX_OVERFLOW;
    bool is_overflow = false;
    bool line_ready = false;
    while (!line_ready && io->driver->read_char(io->driver_ctx, &c))
    {
        // Delimiter found. Set line_ready.
        if (c == '\n' || c == '\r')
        {
            if (io->rx_index > 0)
            {
                io->rx_buffer[io->rx_index] = '\0';
                line_ready = true;
            }
        }
        // Can still add character to RX buffer.
        else if (io->rx_index < (io->rx_buffer_len - 1))
        {
            io->rx_buffer[io->rx_index++] = c;
        }
        // RX buffer full, set overflow bit. Continue iteration until no more
        // characters are available.
        else
        {
            io->status |= MC_IO_STATUS_RX_OVERFLOW;
            is_overflow = true;
        }
    }

    // Event Dispatch
    if (line_ready)
    {
        mc_event_trigger(&io->rx_event, io->rx_buffer);
        io->rx_index = 0;
        // Reset RX overflow bit only if last message was overflow, but this
        // message is no longer overflow.
        if (was_overflow && !is_overflow)
        {
            io->status &= ~MC_IO_STATUS_RX_OVERFLOW;
        }
    }

    update_status(io);
    return convert_status(io->status);
}

uint8_t mc_io_get_status(mc_io_t *io)
{
    if (!io || io->is_initialized != MC_INITIALIZED)
    {
        return MC_IO_STATUS_ERROR;
    }
    return io->status;
}

void mc_io_register_rx_callback(mc_io_t *io, mc_callback_t *callback)
{
    CHECK_IO(io);
    MC_ASSERT(callback != NULL);

    mc_event_register(&io->rx_event, callback);
}