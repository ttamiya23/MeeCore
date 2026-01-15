#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "mc/status.h"
#include "mc/event.h"

// Macro for defining IO. Users should always use this.
#define MC_DEFINE_IO(NAME, DRIVER, CTX, RX_LEN, TX_LEN, MODE) \
    static char NAME##_rx_buffer[RX_LEN];                     \
    static char NAME##_tx_buffer[TX_LEN];                     \
    const mc_io_config_t NAME##_config = {                    \
        .rx_buffer = NAME##_rx_buffer,                        \
        .rx_buffer_len = RX_LEN,                              \
        .tx_buffer = NAME##_tx_buffer,                        \
        .tx_buffer_len = TX_LEN};                             \
    static mc_io_state_t NAME##_state = {                     \
        .rx_event = {0},                                      \
        .status = MC_IO_STATUS_OK,                            \
        .rx_index = 0,                                        \
        .mode = MODE,                                         \
        .is_initialized = 0};                                 \
    const mc_io_t NAME = {                                    \
        .driver = &DRIVER,                                    \
        .ctx = (void *)(&CTX),                                \
        .config = &NAME##_config,                             \
        .state = &NAME##_state};

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

    /* IO config. */
    typedef struct mc_io_config_t
    {
        char *rx_buffer;
        uint16_t rx_buffer_len;
        char *tx_buffer;
        uint16_t tx_buffer_len;
    } mc_io_config_t;

    /* IO mode. Can be text line mode or binary stream mode. */
    typedef enum mc_io_mode_t
    {
        MC_IO_MODE_TEXT_LINE = 0, // Default: Buffers until '\n' or '\r' is found
        MC_IO_MODE_BINARY_STREAM  // Fires event immediately if data exists
    } mc_io_mode_t;

    /* IO state. */
    typedef struct mc_io_state_t
    {
        // Triggered when a newline ('\n' or '\r') is received.
        // Event data: (char*) pointing to the received null-terminated string.
        mc_event_t rx_event;
        volatile uint8_t status;
        volatile uint16_t rx_index;
        mc_io_mode_t mode;
        uint8_t is_initialized;
    } mc_io_state_t;

    /* IO driver struct. */
    typedef struct mc_io_driver_t
    {
        // Init hardware
        void (*init)(void *ctx);

        // Blocking Write
        bool (*write_char)(void *ctx, char c);

        // Polling Read
        bool (*read_char)(void *ctx, char *c);

        // Get status
        uint8_t (*get_status)(void *ctx);
    } mc_io_driver_t;

    /* IO struct. */
    typedef struct mc_io_t
    {
        const mc_io_driver_t *driver;
        void *ctx; // The hardware configuration (e.g., UART handle, GPIO pin)
        const mc_io_config_t *config;
        mc_io_state_t *state;
    } mc_io_t;

    /* Event data struct for RX event callback. */
    typedef struct mc_io_event_data_t
    {
        char *message;
        uint16_t length;
    } mc_io_event_data_t;

    /* Initialize IO. */
    void mc_io_init(const mc_io_t *io);

    /* Write formatted string (printf style). */
    mc_status_t mc_io_printf(const mc_io_t *io, const char *format, ...);

    /* Write formatted string with va_list. */
    mc_status_t mc_io_vprintf(const mc_io_t *io, const char *format, va_list args);

    /* Write raw data. */
    mc_status_t mc_io_write(const mc_io_t *io, const char *data, uint32_t len);

    /* Update IO logic. Checks for received lines and fires events. */
    mc_status_t mc_io_update(const mc_io_t *io);

    /**
     * Get current status flags. Also clears sticky error flags (OVERFLOW) after reading.
     * Does not clear state flags (ERROR/BUSY).
     */
    uint8_t mc_io_get_status(const mc_io_t *io);

    /**
     * Register a callback for received text.
     * The callback will be executed when a message is ready.
     * Event data: (mc_io_event_data_t*) pointing to the received message and length.
     */
    void mc_io_register_rx_callback(const mc_io_t *io, mc_callback_t *callback);

    /* Set mode */
    void mc_io_set_mode(const mc_io_t *io, mc_io_mode_t mode);

#ifdef __cplusplus
}
#endif
