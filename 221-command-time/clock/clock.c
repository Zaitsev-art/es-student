#include "clock.h"

#include "hardware/clocks.h"
#include "log.h"
#include "pico/time.h"

static const uint32_t CLK_SYS_LOW_KHZ = 62500;

static void row(const char *name, clock_handle_t clock, uint32_t source)
{
    const uint32_t set_khz = clock_get_hz(clock) / 1000u;
    const uint32_t measured_khz = frequency_count_khz(source);
    printf("%-8s %9u %12u\n", name, (unsigned)set_khz, (unsigned)measured_khz);
}

void clk_info(void)
{
    printf("signal     set_khz measured_khz\n");
    row("clk_ref", clk_ref, CLOCKS_FC0_SRC_VALUE_CLK_REF);
    row("clk_sys", clk_sys, CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    row("clk_peri", clk_peri, CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    row("clk_usb", clk_usb, CLOCKS_FC0_SRC_VALUE_CLK_USB);
    row("clk_adc", clk_adc, CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    printf("rosc             - %12u\n", (unsigned)frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC));
}

void uptime(void)
{
    printf("uptime: %llu ms\n", (unsigned long long)(time_us_64() / 1000u));
}

// set_sys_clock_pll() переводит clk_peri на PLL USB (48 МГц), поскольку
// PICO_CLOCK_ADJUST_PERI_CLOCK_WITH_SYS_CLOCK по умолчанию равен 0.
static void clk_sys_set(uint32_t khz)
{
    if (set_sys_clock_khz(khz, false))
    {
        LOG_INF("clk_sys %u kHz\n", (unsigned)khz);
    }
    else
    {
        LOG_ERR("clk_sys %u kHz is not set\n", (unsigned)khz);
    }
}

void clk_sys_low(void)
{
    clk_sys_set(CLK_SYS_LOW_KHZ);
}

void clk_sys_default(void)
{
    clk_sys_set(SYS_CLK_KHZ);
}
