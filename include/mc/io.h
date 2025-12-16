#ifndef MC_IO_H_
#define MC_IO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "mc/common.h"
#include "mc/event.h"

    /* Status bits for IO state. */
    typedef enum mc_io_status_t
    {
        MC_IO_STATUS_OK = 0x00,
        MC_IO_STATUS_RX_OVERFLOW = 0x01,
        MC_IO_STATUS_RX_BUSY = 0x02,
        MC_IO_STATUS_HW_BUSY = 0x04,
        MC_IO_STATUS_NO_RESPONSE = 0x08,
        MC_IO_STATUS_ERROR = 0x10
    } mc_io_status_t;

    /* IO struct. */
    typedef struct mc_io_t
    {
        // Triggered when a newline ('\n' or '\r') is received.
        // Event data: (char*) pointing to the received null-terminated string.
        mc_event_t rx_event;
        volatile uint8_t status;
        const struct mc_io_driver_t *driver;
        void *driver_ctx; // The hardware configuration (e.g., UART handle, GPIO pin)
        char *rx_buffer;
        uint16_t rx_buffer_len;
        char *tx_buffer;
        uint16_t tx_buffer_len;
        volatile uint16_t rx_index;
        uint8_t is_initialized;
    } mc_io_t;

    /* IO driver struct. */
    typedef struct mc_io_driver_t
    {
        // Blocking Write
        bool (*write_char)(void *ctx, char c);

        // Polling Read
        bool (*read_char)(void *ctx, char *c);

        // Get status
        uint8_t (*get_status)(void *ctx);
    } mc_io_driver_t;

    /* Initialize IO. */
    void mc_io_init(mc_io_t *io, const mc_io_driver_t *driver,
                    void *driver_ctx, char *rx_buffer, uint16_t rx_len,
                    char *tx_buffer, uint16_t tx_len);

    /* Write formatted string (printf style). */
    mc_status_t mc_io_printf(mc_io_t *io, const char *format, ...);

    /* Write formatted string with va_list. */
    mc_status_t mc_io_vprintf(mc_io_t *io, const char *format, va_list args);

    /* Write raw data. */
    mc_status_t mc_io_write(mc_io_t *io, const char *data, uint32_t len);

    /* Update IO logic. Checks for received lines and fires events. */
    mc_status_t mc_io_update(mc_io_t *io);

    /**
     * Get current status flags. Also clears sticky error flags (OVERFLOW) after reading.
     * Does not clear state flags (ERROR/BUSY).
     */
    uint8_t mc_io_get_status(mc_io_t *io);

    /**
     * Register a callback for received text.
     * The callback will be executed when a newline is detected.
     * Event data: (char*) pointing to the received null-terminated string.
     */
    void mc_io_register_rx_callback(mc_io_t *io, mc_callback_t *callback);

#ifdef __cplusplus
}
#endif

#endif /* MC_IO_H_ */
