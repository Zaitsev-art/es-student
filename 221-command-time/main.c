#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "clock.h"
#include "profiling.h"
#include "command.h"
#include "device.h"
#include "led.h"
#include "log.h"
#include "memory.h"

const uint BLINK_HALF_PERIOD_MS = 500;
// Оценка: 585 тактов на член ряда × 1 000 000 членов / 125 МГц ≈ 4,7 с.
const uint CALC_PI_TERMS = 1000000;
static uint64_t last_toggle_us;
static volatile double pi_result;

static void blink(void)
{
    uint64_t now_us = time_us_64();
    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000u)
    {
        last_toggle_us = now_us;
        led_toggle();
    }
}

static double calc_pi(uint terms)
{
    double sum = 0.0;
    double sign = 1.0;
    for (uint k = 0; k < terms; ++k)
    {
        sum += sign / (2u * k + 1u);
        sign = -sign;
    }
    return 4.0 * sum;
}

static void cmd_info(void)
{
    device_info();
}

static void cmd_version(void)
{
    log_version();
}

static void cmd_ping(void)
{
    printf("pong\n");
}

static void cmd_mem_info(void)
{
    mem_info();
}

static void cmd_fw_info(void)
{
    fw_info();
}

static void cmd_dev_info(void)
{
    dev_info();
}

static void cmd_boot_info(void)
{
    boot_info();
}

static void cmd_clk_info(void)
{
    clk_info();
}

static void cmd_uptime(void)
{
    uptime();
}

static void cmd_calc_pi(void)
{
    uint64_t start_us = time_us_64();
    pi_result = calc_pi(CALC_PI_TERMS);
    uint64_t spent_us = time_us_64() - start_us;

    printf("pi: %.8f\n", pi_result);
    printf("time: %llu ms\n", (unsigned long long)(spent_us / 1000u));
}

static void cmd_main_time_exec(void)
{
    printf("iteration avg %.2f us, max %u us\n",
           (double)profiling_avg_us(), (unsigned)profiling_max_us());
}

static void cmd_main_time_reset(void)
{
    profiling_reset_max();
    printf("max reset\n");
}

static void cmd_clk_sys_low(void)
{
    clk_sys_low();
}

static void cmd_clk_sys_default(void)
{
    clk_sys_default();
}

const struct command_t commands[] = {
    { "info", cmd_info },
    { "version", cmd_version },
    { "ping", cmd_ping },
    { "mem_info", cmd_mem_info },
    { "fw_info", cmd_fw_info },
    { "dev_info", cmd_dev_info },
    { "boot_info", cmd_boot_info },
    { "clk_info", cmd_clk_info },
    { "uptime", cmd_uptime },
    { "calc_pi", cmd_calc_pi },
    { "main_time_exec", cmd_main_time_exec },
    { "main_time_reset", cmd_main_time_reset },
    { "clk_sys_low", cmd_clk_sys_low },
    { "clk_sys_default", cmd_clk_sys_default },
};

const uint command_count = sizeof(commands) / sizeof(commands[0]);

static void handle_command(const char *command)
{
    for (uint i = 0; i < command_count; ++i)
    {
        if (strcmp(command, commands[i].name) == 0)
        {
            if (commands[i].handler != NULL)
            {
                commands[i].handler();
            }
            return;
        }
    }
    LOG_ERR("unknown command: %s\n", command);
}

#define LINE_SIZE 32

static char line[LINE_SIZE];
static uint line_length;
static bool line_overflow;
static bool previous_cr;

static void read_line(void)
{
    int symbol = getchar_timeout_us(0);
    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        const bool skip_lf = symbol == '\n' && previous_cr;
        previous_cr = symbol == '\r';
        if (skip_lf)
        {
            return;
        }

        putchar('\n');
        line[line_length] = '\0';
        if (line_overflow)
        {
            LOG_ERR("command too long\n");
        }
        else if (line_length > 0)
        {
            LOG_DBG("got %s\n", line);
            handle_command(line);
        }
        line_length = 0;
        line_overflow = false;
        return;
    }

    previous_cr = false;
    if (line_overflow)
    {
        return;
    }

    if (symbol == '\b' || symbol == 0x7f)
    {
        if (line_length > 0)
        {
            --line_length;
            fputs("\b \b", stdout);
        }
        return;
    }

    if (symbol < ' ' || symbol > '~')
    {
        return;
    }
    if (line_length + 1 >= LINE_SIZE)
    {
        line_overflow = true;
        return;
    }

    line[line_length++] = (char)symbol;
    putchar(symbol);
}

int main(void)
{
    stdio_init_all();
    led_init();
    last_toggle_us = time_us_64();
    profiling_init();

    while (1)
    {
        profiling_iteration();
        blink();
        read_line();
    }
}
