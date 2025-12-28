#include "fake_io.h"
#include <string.h>

// RX buffer used for fake IO
static char fake_rx[FAKE_RX_SIZE];

// TX buffer used for fake IO
static char fake_tx[FAKE_TX_SIZE];

// Driver: Write (Stores in output buffer)
static bool fake_io_write(void *ctx, char c)
{
    fake_io_ctx_t *h = (fake_io_ctx_t *)ctx;
    if (h->output_index < FAKE_TX_SIZE - 1)
    {
        h->output_data[h->output_index++] = c;
        h->output_data[h->output_index] = '\0'; // Keep null terminated for easy printing
    }
    return true;
}

// Driver: Read (Reads from input buffer)
static bool fake_io_read(void *ctx, char *c)
{
    fake_io_ctx_t *h = (fake_io_ctx_t *)ctx;
    if (h->input_head != h->input_tail)
    {
        *c = h->input_data[h->input_tail++];
        return true;
    }
    return false;
}

static uint8_t fake_io_get_status(void *ctx)
{
    fake_io_ctx_t *h = (fake_io_ctx_t *)ctx;
    return h->status;
}

const mc_io_driver_t fake_io_driver = {
    .write_char = fake_io_write,
    .read_char = fake_io_read,
    .get_status = fake_io_get_status};

void fake_io_init(mc_io_t *io, fake_io_ctx_t *ctx)
{
    ctx->input_head = 0;
    ctx->input_tail = 0;
    ctx->output_index = 0;
    ctx->status = MC_IO_STATUS_OK;
    memset(ctx->output_data, 0, FAKE_TX_SIZE);
    memset(fake_rx, 0, FAKE_RX_SIZE);
    memset(fake_tx, 0, FAKE_TX_SIZE);
    mc_io_init(io, &fake_io_driver, ctx, fake_rx, FAKE_RX_SIZE, fake_tx,
               FAKE_TX_SIZE);
}