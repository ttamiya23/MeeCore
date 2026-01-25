#pragma once

#include "mc/communication/stream.h"

#define FAKE_RX_SIZE 1024
#define FAKE_TX_SIZE 1024

typedef struct
{
    // RX Simulation (Computer -> MCU)
    char input_data[FAKE_RX_SIZE];
    int input_head;
    int input_tail;

    // TX Simulation (MCU -> Computer)
    char output_data[FAKE_TX_SIZE];
    int output_index;

    uint8_t status; // For mocking status
} fake_stream_ctx_t;

// Driver to fake stream
extern const mc_stream_driver_t fake_stream_driver;

// Push string to input data.
void fake_stream_push_string(fake_stream_ctx_t *ctx, const char *str);

// Push char array of set length to input data.
void fake_stream_push_char_array(fake_stream_ctx_t *ctx, const char *str,
                                 uint16_t length);