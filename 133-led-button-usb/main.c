#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint LED_PIN = 25;
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

void set_led(bool on)
{
    gpio_put(LED_PIN, on);
    printf("led %s\n", on ? "on" : "off");
}

bool handle_command(int command, bool led)
{
    if (command == 'e' || command == 'd')
    {
        led = command == 'e';
        set_led(led);
    }
    else
    {
        printf("unknown command: %c\n", command);
    }
    return led;
}

int main(void)
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    bool led = false;
    bool previous = true;

    while (1)
    {
        bool current = get_button_debounce(BUTTON_PIN);
        if (previous && !current)
        {
            led = !led;
            set_led(led);
        }
        previous = current;

        int command = getchar_timeout_us(0);
        if (command == PICO_ERROR_TIMEOUT)
        {
            continue;
        }
        led = handle_command(command, led);
    }
}
