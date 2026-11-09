#include "profiling.h"

#include "pico/time.h"

// При 2 мкс на итерацию получается 500 000 итераций/с. 2^19 = 524 288, примерно одна секунда.
#define AVG_SHIFT 19
#define FRACTION_SHIFT 8

static uint32_t previous_us;
static uint32_t max_us;
static uint64_t avg_sum;

void profiling_init(void)
{
    previous_us = time_us_32();
    max_us = 0;
    avg_sum = 0;
}

void profiling_iteration(void)
{
    const uint32_t now_us = time_us_32();
    const uint32_t iteration_us = now_us - previous_us;
    previous_us = now_us;
    if (iteration_us > max_us)
    {
        max_us = iteration_us;
    }
    avg_sum = avg_sum - (avg_sum >> AVG_SHIFT) + ((uint64_t)iteration_us << FRACTION_SHIFT);
}

float profiling_avg_us(void)
{
    return (float)(avg_sum >> AVG_SHIFT) / (1u << FRACTION_SHIFT);
}

uint32_t profiling_max_us(void)
{
    return max_us;
}

void profiling_reset_max(void)
{
    max_us = 0;
}
