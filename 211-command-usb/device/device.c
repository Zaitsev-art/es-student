#include "device.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"
#include "pico/unique_id.h"
#include "pico/version.h"

struct info_t device_card = {
    .version = 0x00010000u,
    .name = DEVICE_NAME,
    .revision = 0,
};


static uint32_t read_chip_id(void)
{
    const volatile uint32_t *const chip_id =
        (const volatile uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    return *chip_id;
}

static uint32_t chip_revision(uint32_t id)
{
    return (id & SYSINFO_CHIP_ID_REVISION_BITS) >> SYSINFO_CHIP_ID_REVISION_LSB;
}

void device_info(void)
{
    char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(board_id, sizeof(board_id));

    const uint32_t id = read_chip_id();
    const uint32_t manufacturer =
        (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    const uint32_t part = (id & SYSINFO_CHIP_ID_PART_BITS) >> SYSINFO_CHIP_ID_PART_LSB;
    const uint32_t revision = chip_revision(id);

    printf("project: %s\n", DEVICE_PROJECT);
    printf("repo: %s\n", DEVICE_REPO);
    printf("board: %s\n", DEVICE_BOARD);
    printf("serial: %s\n", board_id);
    printf("chip: manufacturer 0x%03" PRIx32 ", part 0x%04" PRIx32
           ", revision %" PRIu32 "\n", manufacturer, part, revision);
    printf("pico-sdk: %s\n", PICO_SDK_VERSION_STRING);
}

void dev_info(void)
{
    device_card.revision = (uint8_t)chip_revision(read_chip_id());
    const size_t fields =
        sizeof(device_card.version) + sizeof(device_card.name) + sizeof(device_card.revision);

    printf("struct          address     size offset value\n");
    printf("device_card     0x%08x %5u\n", (unsigned)(uintptr_t)&device_card, (unsigned)sizeof(device_card));
    printf("- version       0x%08x %5u %6u 0x%08" PRIx32 "\n",
           (unsigned)(uintptr_t)&device_card.version, (unsigned)sizeof(device_card.version),
           (unsigned)offsetof(struct info_t, version), device_card.version);
    printf("- name          0x%08x %5u %6u %s\n",
           (unsigned)(uintptr_t)&device_card.name, (unsigned)sizeof(device_card.name),
           (unsigned)offsetof(struct info_t, name), device_card.name);
    printf("- revision      0x%08x %5u %6u %u\n",
           (unsigned)(uintptr_t)&device_card.revision, (unsigned)sizeof(device_card.revision),
           (unsigned)offsetof(struct info_t, revision), (unsigned)device_card.revision);
    printf("fields %u, sizeof %u, padding %u\n",
           (unsigned)fields, (unsigned)sizeof(device_card), (unsigned)(sizeof(device_card) - fields));
}
