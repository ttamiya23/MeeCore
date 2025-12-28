#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "mc/system/console.h"
#include "mc/utils.h"

// Helper defines.
#define STX "\x02"
#define ETX "\x03"

// Helpers: check if char is delimiter
static bool is_delimiter(char c)
{
    // Delimiters are ".,()="
    return isspace((unsigned char)c) ||
           c == '.' || c == ',' ||
           c == '(' || c == ')' ||
           c == '=';
}

// Helpers: skip delimiters
static const char *skip_delimiters(const char *str)
{
    while (*str && is_delimiter(*str))
    {
        str++;
    }
    return str;
}

// Helper: find the end of the current token (for string extraction)
static const char *find_token_end(const char *str)
{
    while (*str && !is_delimiter(*str))
    {
        str++;
    }
    return str;
}

// Helper: parse member token, like x0 or f3. Return false if failed.
static bool parse_member_token(const char *token, mc_sys_cmd_info_t *info)
{
    if ((*token != 'x' && *token == 'y' && *token == 'f') ||
        !isdigit((unsigned char)token[1]))
    {
        return false;
    }

    switch (*token)
    {
    case 'x':
        info->type = MC_CMD_TYPE_INPUT;
        break;
    case 'y':
        info->type = MC_CMD_TYPE_OUTPUT;
        break;
    case 'f':
        info->type = MC_CMD_TYPE_FUNC;
        break;
    default:
        return false;
    }
    info->id = (uint8_t)strtol(token + 1, NULL, 10);
    info->has_preset = false;
    return true;
}

// Helper: Get command length. Useful for cleaner echo responses.
static uint16_t get_cmd_length(const char *str)
{
    if (!str)
    {
        return 0;
    }
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        len--;
    }
    return len;
}

// Helper: Find System ID by Name
static int find_system_id_by_name(mc_system_console_t *console,
                                  const char *name, size_t len)
{
    for (int i = 0; i < console->system_count; i++)
    {
        const char *entry_name = console->systems[i].name;
        // Check for exact length match to avoid prefix collisions
        if (entry_name && strncmp(entry_name, name, len) == 0 &&
            entry_name[len] == '\0')
        {
            return i;
        }
    }
    return -1;
}

// Helper: simply prints value if OK, "E:[code]" if error.
static mc_status_t send_result(mc_system_console_t *console, mc_result_t result)
{
    int32_t val = result.ok ? result.value : result.error;
    if (!result.ok)
    {
        MC_RETURN_IF_ERROR(mc_io_printf(console->io, "E:"));
    }
    MC_RETURN_IF_ERROR(mc_io_printf(console->io, "%d", val));
    return MC_OK;
}

// Helper: Send Standard Response
// Format: \stx\nCMD\n[E:]VAL\n\etx
static mc_status_t send_response(mc_system_console_t *console,
                                 const char *cmd_echo, mc_result_t result)
{
    MC_RETURN_IF_ERROR(mc_io_write(console->io, STX, 1));
    MC_RETURN_IF_ERROR(mc_io_write(console->io, "\n", 1));

    // Echo the command (trimmed)
    uint8_t len = get_cmd_length(cmd_echo);
    MC_RETURN_IF_ERROR(mc_io_write(console->io, cmd_echo, len));
    MC_RETURN_IF_ERROR(mc_io_write(console->io, "\n", 1));

    // Send result
    MC_RETURN_IF_ERROR(send_result(console, result));

    MC_RETURN_IF_ERROR(mc_io_write(console->io, "\n", 1));
    MC_RETURN_IF_ERROR(mc_io_write(console->io, ETX, 1));
}

// Helper: Send System Dump (TSV)
// Triggered when user types just the system name/ID (e.g. "s0", "led1")
static mc_status_t send_system_dump(
    mc_system_console_t *console, const char *cmd_echo,
    const mc_system_entry_t *systems, uint8_t sys_count)
{
    MC_RETURN_IF_ERROR(mc_io_write(console->io, STX, 1));
    MC_RETURN_IF_ERROR(mc_io_write(console->io, "\n", 1));

    // Echo the command, if it exists (trimmed)
    if (cmd_echo)
    {
        uint8_t len = get_cmd_length(cmd_echo);
        MC_RETURN_IF_ERROR(mc_io_write(console->io, cmd_echo, len));
    }

    // Header Row: "sys\t0\t1\t2..."
    MC_RETURN_IF_ERROR(mc_io_printf(console->io, "sys"));
    for (int i = 0; i < console->max_header_count; i++)
    {
        MC_RETURN_IF_ERROR(mc_io_printf(console->io, "\t%d", i));
    }
    MC_RETURN_IF_ERROR(mc_io_write(console->io, "\n", 1));

    // Input/Output Row: "sys.x\t100\t200..."
    for (uint8_t sys_i = 0; sys_i < sys_count; sys_i++)
    {
        mc_system_entry_t sys = systems[sys_i];
        MC_RETURN_IF_ERROR(mc_io_printf(console->io, "%s.x", sys.name));
        for (int i = 0; i < console->max_header_count; i++)
        {
            mc_result_t res = mc_sys_read_input(sys.system, i);
            MC_RETURN_IF_ERROR(send_result(console, res));
        }
        MC_RETURN_IF_ERROR(mc_io_write(console->io, "\n", 1));

        MC_RETURN_IF_ERROR(mc_io_printf(console->io, "%s.y", sys.name));
        for (int i = 0; i < console->max_header_count; i++)
        {
            mc_result_t res = mc_sys_read_output(sys.system, i);
            MC_RETURN_IF_ERROR(send_result(console, res));
        }
        MC_RETURN_IF_ERROR(mc_io_write(console->io, "\n", 1));
    }

    // End Frame
    MC_RETURN_IF_ERROR(mc_io_write(console->io, ETX, 1));
    return MC_OK;
}

// Helper: main method for processing command
mc_status_t process_command(mc_system_console_t *console, const char *cmd)
{
    MC_ASSERT(console != NULL);
    MC_ASSERT(cmd != NULL);

    const char *cmd_start = skip_delimiters(cmd);
    const char *token = cmd_start;

    // --- CHECK 1: "clear" command ---
    if (strncmp(token, "clear", 5) == 0)
    {
        return mc_io_printf(console->io, "\x1B[2J\x1B[H");
    }

    // --- STEP 1: Parse System (Token 1) ---
    const char *token_end = find_token_end(token);
    size_t token_len = token_end - token;

    // Check for Dump "s0" or "led" at end of string
    // Logic: If the next non-delimiter char is terminator, it's a dump.
    const char *next_token_start = skip_delimiters(token_end);
    bool is_dump = (*next_token_start == '\0');

    long sys_id = -1;

    // Strategy A: "s<ID>"
    if (*token == 's' && isdigit((unsigned char)token[1]))
    {
        sys_id = strtol(token + 1, NULL, 10);
    }
    // Strategy B: Name Alias
    else
    {
        sys_id = find_system_id_by_name(console, token, token_len);
    }

    // Validate System
    if (sys_id < 0 || sys_id >= console->system_count ||
        console->systems[sys_id].system == NULL)
    {
        return send_response(console, cmd_start,
                             MC_ERR_VAL(MC_ERROR_INVALID_ARGS));
    }
    const mc_system_t *sys = console->systems[sys_id].system;

    // Execute Dump if requested
    if (is_dump)
    {
        return send_system_dump(console, cmd_start, &console->systems[sys_id],
                                1);
    }

    // Move to next token
    token = next_token_start;

    // --- STEP 2: Parse Command/Member (Token 2) ---
    // This could be "x0", "turnOn", "pwm", etc.
    token_end = find_token_end(token);
    token_len = token_end - token;

    mc_sys_cmd_info_t cmd_info = {0};
    bool found_custom = false;

    // A. Check Driver Custom Commands ("turnOn", "pwm")
    // We copy to a temp buffer to null-terminate for the driver lookup
    char cmd_name[32];
    if (token_len < sizeof(cmd_name))
    {
        memcpy(cmd_name, token, token_len);
        cmd_name[token_len] = '\0';

        if (sys->driver->parse_command &&
            sys->driver->parse_command(sys->ctx, cmd_name, &cmd_info))
        {
            found_custom = true;
        }
    }

    // B. If not custom, fallback to standard "x0", "y1", "f2"
    if (!found_custom && !parse_member_token(token, &cmd_info))
    {
        return send_response(console, cmd_start, MC_ERR_VAL(MC_ERROR_INVALID_ARGS));
    }

    // Move past this token
    token = skip_delimiters(token_end);

    // --- STEP 3: Execute ---
    mc_result_t res;
    // CASE: INPUT (x)
    if (cmd_info.type == MC_CMD_TYPE_INPUT)
    {
        if (cmd_info.has_preset)
        {
            // "turnOn" -> Preset Write
            res = MC_STATUS_TO_RESULT(mc_sys_write_input(sys, cmd_info.id,
                                                         cmd_info.preset_val));
        }
        else
        {
            // Check if there is a 3rd token (Value)
            if (*token != '\0')
            {
                // "x0 = 1" or "pwm(20)" -> User Write
                long write_val = strtol(token, NULL, 10);
                res = MC_STATUS_TO_RESULT(
                    mc_sys_write_input(sys, cmd_info.id, (int32_t)write_val));
            }
            else
            {
                // End of string -> Read
                res = mc_sys_read_input(sys, cmd_info.id);
            }
        }
    }
    // CASE: OUTPUT (y)
    else if (cmd_info.type == MC_CMD_TYPE_OUTPUT)
    {
        res = mc_sys_read_output(sys, cmd_info.id);
    }
    // CASE: FUNCTION (f)
    else if (cmd_info.type == MC_CMD_TYPE_FUNC)
    {
        uint8_t argc = 0;

        // Parse arguments until string ends
        char *end;
        while (*token && argc < console->max_args_count)
        {
            console->args_buffer[argc++] = (int32_t)strtol(token, &end,
                                                           10);
            token = skip_delimiters(end);
        }

        res = MC_STATUS_TO_RESULT(mc_sys_invoke(sys, cmd_info.id,
                                                console->args_buffer, argc));
    }

    return send_response(console, cmd_start, res);
}

// --- IO Callback Wrapper ---
static void console_rx_handler(void *ctx, void *data)
{
    mc_system_console_t *console = (mc_system_console_t *)ctx;
    const char *cmd_string = (const char *)data;

    process_command(console, cmd_string);
}

void mc_system_console_init(mc_system_console_t *console, mc_io_t *io,
                            const mc_system_entry_t *system_list,
                            uint8_t sys_count, int32_t *args_buffer,
                            uint8_t max_args_count, uint8_t max_header_count)
{
    MC_ASSERT(console != NULL);
    MC_ASSERT(io != NULL);
    MC_ASSERT(system_list != NULL);
    MC_ASSERT(args_buffer != NULL);

    console->io = io;
    console->systems = system_list;
    console->system_count = sys_count;
    console->args_buffer = args_buffer;
    console->max_args_count = max_args_count;
    console->max_header_count = max_header_count;

    // Register IO Callback
    mc_callback_init(&console->rx_callback, console_rx_handler, &console);
    mc_io_register_rx_callback(io, &console->rx_callback);
}

/* Dump system info */
mc_status_t mc_system_console_dump(mc_system_console_t *console,
                                   const mc_system_entry_t *systems,
                                   uint8_t sys_count)
{
    MC_ASSERT(console != NULL);
    MC_ASSERT(systems != NULL);

    return send_system_dump(console, NULL, systems, sys_count);
}