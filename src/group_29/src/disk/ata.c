#include "ata.h"

#define ATA_PRIMARY_IO_BASE 0x1F0U
#define ATA_PRIMARY_CONTROL_BASE 0x3F6U

#define ATA_REG_DATA 0x00U
#define ATA_REG_SECTOR_COUNT 0x02U
#define ATA_REG_LBA_LOW 0x03U
#define ATA_REG_LBA_MID 0x04U
#define ATA_REG_LBA_HIGH 0x05U
#define ATA_REG_DRIVE_SELECT 0x06U
#define ATA_REG_STATUS_COMMAND 0x07U

#define ATA_STATUS_ERR 0x01U
#define ATA_STATUS_DRQ 0x08U
#define ATA_STATUS_DF 0x20U
#define ATA_STATUS_BSY 0x80U

#define ATA_CMD_READ_SECTORS 0x20U
#define ATA_CMD_WRITE_SECTORS 0x30U
#define ATA_CMD_CACHE_FLUSH 0xE7U

static void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void outw(uint16_t port, uint16_t value) {
    __asm__ __volatile__("outw %0, %1" : : "a"(value), "Nd"(port));
}

static uint16_t inw(uint16_t port) {
    uint16_t value;
    __asm__ __volatile__("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void ata_delay_400ns(void) {
    inb(ATA_PRIMARY_CONTROL_BASE);
    inb(ATA_PRIMARY_CONTROL_BASE);
    inb(ATA_PRIMARY_CONTROL_BASE);
    inb(ATA_PRIMARY_CONTROL_BASE);
}

static bool ata_poll(bool wait_for_drq) {
    uint8_t status;

    ata_delay_400ns();

    do {
        status = inb(ATA_PRIMARY_IO_BASE + ATA_REG_STATUS_COMMAND);
    } while ((status & ATA_STATUS_BSY) != 0U);

    if ((status & (ATA_STATUS_ERR | ATA_STATUS_DF)) != 0U) {
        return false;
    }

    if (wait_for_drq && (status & ATA_STATUS_DRQ) == 0U) {
        return false;
    }

    return true;
}

static void ata_select_drive(uint32_t lba) {
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_DRIVE_SELECT, (uint8_t)(0xF0U | ((lba >> 24) & 0x0FU)));
    ata_delay_400ns();
}

static void ata_program_lba(uint32_t lba) {
    ata_select_drive(lba);
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_SECTOR_COUNT, 1U);
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_LBA_LOW, (uint8_t)(lba & 0xFFU));
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_LBA_MID, (uint8_t)((lba >> 8) & 0xFFU));
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFFU));
}

bool ata_read_sector(uint32_t lba, uint8_t buffer[ATA_SECTOR_SIZE]) {
    uint16_t* words = (uint16_t*)buffer;
    uint16_t index;

    ata_program_lba(lba);
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_STATUS_COMMAND, ATA_CMD_READ_SECTORS);

    if (!ata_poll(true)) {
        return false;
    }

    for (index = 0U; index < (ATA_SECTOR_SIZE / 2U); ++index) {
        words[index] = inw(ATA_PRIMARY_IO_BASE + ATA_REG_DATA);
    }

    return true;
}

bool ata_write_sector(uint32_t lba, const uint8_t buffer[ATA_SECTOR_SIZE]) {
    const uint16_t* words = (const uint16_t*)buffer;
    uint16_t index;

    ata_program_lba(lba);
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_STATUS_COMMAND, ATA_CMD_WRITE_SECTORS);

    if (!ata_poll(true)) {
        return false;
    }

    for (index = 0U; index < (ATA_SECTOR_SIZE / 2U); ++index) {
        outw(ATA_PRIMARY_IO_BASE + ATA_REG_DATA, words[index]);
    }

    outb(ATA_PRIMARY_IO_BASE + ATA_REG_STATUS_COMMAND, ATA_CMD_CACHE_FLUSH);
    return ata_poll(false);
}
