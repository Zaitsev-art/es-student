#include "device.h"

#include <inttypes.h>
#include <stdio.h>

#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"
#include "pico/unique_id.h"

void device_info(void)
{
    char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(board_id, sizeof(board_id));

    const volatile uint32_t *const chip_id =
        (const volatile uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    const uint32_t id = *chip_id;
    const uint32_t manufacturer =
        (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    const uint32_t part = (id & SYSINFO_CHIP_ID_PART_BITS) >> SYSINFO_CHIP_ID_PART_LSB;
    const uint32_t revision = (id & SYSINFO_CHIP_ID_REVISION_BITS) >> SYSINFO_CHIP_ID_REVISION_LSB;

    printf("project: %s\n", DEVICE_PROJECT);
    printf("repo: %s\n", DEVICE_REPO);
    printf("board: %s\n", DEVICE_BOARD);
    printf("serial: %s\n", board_id);
    printf("chip: manufacturer 0x%03" PRIx32 ", part 0x%04" PRIx32
           ", revision %" PRIu32 "\n", manufacturer, part, revision);
}
