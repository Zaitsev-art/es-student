#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "command.h"
#include "device.h"
#include "led.h"
#include "log.h"
#include "memory.h"

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

static bool get_button_debounce(uint pin)
{
    static bool stable = true;
    bool first = gpio_get(pin);
    sleep_ms(DEBOUNCE_MS);
    bool second = gpio_get(pin);
    if (first == second)
    {
        stable = second;
    }
    return stable;
}

static void cmd_enable(void)
{
    led_set(true);
    LOG_INF("led %s\n", led_is_on() ? "on" : "off");
}

static void cmd_disable(void)
{
    led_set(false);
    LOG_INF("led %s\n", led_is_on() ? "on" : "off");
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

const struct command_t commands[] = {
    { "enable", cmd_enable },
    { "disable", cmd_disable },
    { "info", cmd_info },
    { "version", cmd_version },
    { "ping", cmd_ping },
    { "mem_info", cmd_mem_info },
    { "fw_info", cmd_fw_info },
    { "dev_info", cmd_dev_info },
    { "boot_info", cmd_boot_info },
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

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    bool previous = true;
    while (1)
    {
        bool current = get_button_debounce(BUTTON_PIN);
        if (previous && !current)
        {
            led_toggle();
            LOG_INF("led %s\n", led_is_on() ? "on" : "off");
        }
        previous = current;
        read_line();
    }
}
