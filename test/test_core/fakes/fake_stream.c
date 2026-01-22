#include "fakes/fake_stream.h"
#include <string.h>

// Driver: Init (Initializes ctx)
void fake_stream_init(void *ctx)
{
    fake_stream_ctx_t *h = (fake_stream_ctx_t *)ctx;
    h->input_head = 0;
    h->input_tail = 0;
    h->output_index = 0;
    h->status = MC_STREAM_STATUS_OK;
    memset(h->output_data, 0, FAKE_TX_SIZE);
}

// Driver: Write (Stores in output buffer)
static bool fake_stream_write(void *ctx, char c)
{
    fake_stream_ctx_t *h = (fake_stream_ctx_t *)ctx;
    if (h->output_index < FAKE_TX_SIZE - 1)
    {
        h->output_data[h->output_index++] = c;
        h->output_data[h->output_index] = '\0'; // Keep null terminated for easy printing
        return true;
    }
    return false;
}

// Driver: Read (Reads from input buffer)
static bool fake_stream_read(void *ctx, char *c)
{
    fake_stream_ctx_t *h = (fake_stream_ctx_t *)ctx;
    if (h->input_head != h->input_tail)
    {
        *c = h->input_data[h->input_tail++];
        return true;
    }
    return false;
}

static uint8_t fake_stream_get_status(void *ctx)
{
    fake_stream_ctx_t *h = (fake_stream_ctx_t *)ctx;
    return h->status;
}

const mc_stream_driver_t fake_stream_driver = {
    .init = fake_stream_init,
    .write_char = fake_stream_write,
    .read_char = fake_stream_read,
    .get_status = fake_stream_get_status};

void fake_stream_push_string(fake_stream_ctx_t *ctx, const char *str)
{
    while (*str)
    {
        ctx->input_data[ctx->input_head++] = *str++;
    }
}

void fake_stream_push_char_array(fake_stream_ctx_t *ctx, const char *str,
                                 uint16_t length)
{
    for (int i = 0; i < length; i++)
    {
        ctx->input_data[ctx->input_head++] = str[i];
    }
}
