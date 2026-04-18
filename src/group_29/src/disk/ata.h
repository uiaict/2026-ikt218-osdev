#pragma once

#include "../../include/libc/stdbool.h"
#include "../../include/libc/stdint.h"

#define ATA_SECTOR_SIZE 512U

bool ata_read_sector(uint32_t lba, uint8_t buffer[ATA_SECTOR_SIZE]);
bool ata_write_sector(uint32_t lba, const uint8_t buffer[ATA_SECTOR_SIZE]);
