#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "device.h"
#include "led.h"
#include "log.h"

const uint BUTTON_PIN = 15;

const uint DEBOUNCE_MS = 20;

bool get_button_debounce(uint pin)
{
    // The two samples must agree before the single button changes level.
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

void handle_command(int command)
{
    switch (command)
    {
    case 'e':
    case 'd':
        led_set(command == 'e');
        LOG_INF("led %s\n", led_is_on() ? "on" : "off");
        break;
    case 'v':
        log_version();
        break;
    case 'i':
        device_info();
        break;
    default:
        LOG_ERR("unknown command: %c\n", command);
        break;
    }
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

        int command = getchar_timeout_us(0);
        if (command == PICO_ERROR_TIMEOUT)
        {
            continue;
        }
        LOG_DBG("got %c\n", command);
        handle_command(command);
    }
}
