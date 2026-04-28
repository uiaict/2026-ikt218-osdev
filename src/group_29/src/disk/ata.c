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

/** \file \todo Get rid of duplicate functions */
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

/** Just runs inb() against ATA disk 4 times
 * \see https://wiki.osdev.org/ATA_PIO_Mode#400ns_delays
 */
static void ata_delay_400ns(void) {
    inb(ATA_PRIMARY_CONTROL_BASE);
    inb(ATA_PRIMARY_CONTROL_BASE);
    inb(ATA_PRIMARY_CONTROL_BASE);
    inb(ATA_PRIMARY_CONTROL_BASE);
}

/** Check if the drive is ready before disk read/write.
 * This uses pooling so it is blocking.
 * \note This is blocking until the disk is ready
 * \returns true when done, false if there was an error.
 * \see https://wiki.osdev.org/ATA_PIO_Mode#Polling_the_Status_vs._IRQs
 * \see https://wiki.osdev.org/ATA_PIO_Mode#28_bit_PIO
 */
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

/** Select drive and/or head */
static void ata_select_drive(uint32_t lba) {
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_DRIVE_SELECT, (uint8_t)(0xF0U | ((lba >> 24) & 0x0FU)));
    ata_delay_400ns();
}

/** Move HDD head to the correct position on the disk for I/O */
static void ata_program_lba(uint32_t lba) {
    ata_select_drive(lba);
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_SECTOR_COUNT, 1U);
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_LBA_LOW, (uint8_t)(lba & 0xFFU));
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_LBA_MID, (uint8_t)((lba >> 8) & 0xFFU));
    outb(ATA_PRIMARY_IO_BASE + ATA_REG_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFFU));
}

/** Read a disk sector
 * 
 * This is done over ATA PIO, which means that bytes are read over I/O ports uring inw.
 * \returns true on sucsess, otherwise: false
 * \param lba Location on the disk to read from
 * \param buffer Where the read data is put
 */
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

/** Write to a disk sector
 * \param lba Destination byte address on disk
 * \param buffer Data to be written
 * \returns false on error, otherwise: ata_poll(false)
 */
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
