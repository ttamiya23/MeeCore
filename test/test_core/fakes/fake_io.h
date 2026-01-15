#pragma once

#include "mc/io.h"

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
} fake_io_ctx_t;

// Driver to fake IO
extern const mc_io_driver_t fake_io_driver;

// Push string to input data.
void fake_io_push_string(fake_io_ctx_t *ctx, const char *str);

// Push char array of set length to input data.
void fake_io_push_char_array(fake_io_ctx_t *ctx, const char *str,
                             uint16_t length);