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

// Macro for defining stream. Users should always use this.
#define MC_DEFINE_STREAM(NAME, DRIVER, CTX, RX_LEN, TX_LEN, MODE) \
    static char NAME##_rx_buffer[RX_LEN];                         \
    static char NAME##_tx_buffer[TX_LEN];                         \
    const mc_stream_config_t NAME##_config = {                    \
        .rx_buffer = NAME##_rx_buffer,                            \
        .rx_buffer_len = RX_LEN,                                  \
        .tx_buffer = NAME##_tx_buffer,                            \
        .tx_buffer_len = TX_LEN};                                 \
    static mc_stream_state_t NAME##_state = {                     \
        .rx_event = {0},                                          \
        .status = MC_STREAM_STATUS_OK,                            \
        .rx_index = 0,                                            \
        .mode = MODE,                                             \
        .is_initialized = 0};                                     \
    const mc_stream_t NAME = {                                    \
        .driver = &DRIVER,                                        \
        .ctx = (void *)(&CTX),                                    \
        .config = &NAME##_config,                                 \
        .state = &NAME##_state};

    /* Status bits for stream state. */
    typedef enum mc_stream_status_t
    {
        MC_STREAM_STATUS_OK = 0x00,
        MC_STREAM_STATUS_RX_OVERFLOW = 0x01,
        MC_STREAM_STATUS_RX_BUSY = 0x02,
        MC_STREAM_STATUS_HW_BUSY = 0x04,
        MC_STREAM_STATUS_NO_RESPONSE = 0x08,
        MC_STREAM_STATUS_ERROR = 0x10
    } mc_stream_status_t;

    /* Stream config. */
    typedef struct mc_stream_config_t
    {
        char *rx_buffer;
        uint16_t rx_buffer_len;
        char *tx_buffer;
        uint16_t tx_buffer_len;
    } mc_stream_config_t;

    /* Stream mode. Can be text line mode or binary stream mode. */
    typedef enum mc_stream_mode_t
    {
        MC_STREAM_MODE_TEXT_LINE = 0, // Default: Buffers until '\n' or '\r' is found
        MC_STREAM_MODE_BINARY_STREAM  // Fires event immediately if data exists
    } mc_stream_mode_t;

    /* Stream state. */
    typedef struct mc_stream_state_t
    {
        // Triggered when a newline ('\n' or '\r') is received.
        // Event data: (char*) pointing to the received null-terminated string.
        mc_event_t rx_event;
        volatile uint8_t status;
        volatile uint16_t rx_index;
        mc_stream_mode_t mode;
        uint8_t is_initialized;
    } mc_stream_state_t;

    /* Stream driver struct. */
    typedef struct mc_stream_driver_t
    {
        // Init hardware
        void (*init)(void *ctx);

        // Blocking Write
        bool (*write_char)(void *ctx, char c);

        // Polling Read
        bool (*read_char)(void *ctx, char *c);

        // Get status
        uint8_t (*get_status)(void *ctx);
    } mc_stream_driver_t;

    /* Stream struct. */
    typedef struct mc_stream_t
    {
        const mc_stream_driver_t *driver;
        void *ctx; // The hardware configuration (e.g., UART handle, GPIO pin)
        const mc_stream_config_t *config;
        mc_stream_state_t *state;
    } mc_stream_t;

    /* Event data struct for RX event callback. */
    typedef struct mc_stream_event_data_t
    {
        char *message;
        uint16_t length;
    } mc_stream_event_data_t;

    /* Initialize stream. */
    void mc_stream_init(const mc_stream_t *stream);

    /* Write formatted string (printf style). */
    mc_status_t mc_stream_printf(const mc_stream_t *stream, const char *format, ...);

    /* Write formatted string with va_list. */
    mc_status_t mc_stream_vprintf(const mc_stream_t *stream,
                                  const char *format, va_list args);

    /* Write raw data. */
    mc_status_t mc_stream_write(const mc_stream_t *stream, const char *data,
                                uint32_t len);

    /* Update stream logic. Checks for received lines and fires events. */
    mc_status_t mc_stream_update(const mc_stream_t *stream);

    /**
     * Get current status flags. Also clears sticky error flags (OVERFLOW) after reading.
     * Does not clear state flags (ERROR/BUSY).
     */
    uint8_t mc_stream_get_status(const mc_stream_t *stream);

    /**
     * Register a callback for received text.
     * The callback will be executed when a message is ready.
     * Event data: (mc_stream_event_data_t*) pointing to the received message and length.
     */
    void mc_stream_register_rx_callback(const mc_stream_t *stream,
                                        mc_callback_t *callback);

    /* Set mode */
    void mc_stream_set_mode(const mc_stream_t *stream, mc_stream_mode_t mode);

#ifdef __cplusplus
}
#endif
