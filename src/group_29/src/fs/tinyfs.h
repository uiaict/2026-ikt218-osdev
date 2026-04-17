#pragma once

#include "../../include/libc/stdbool.h"
#include "../../include/libc/stdint.h"

#define TINYFS_MAGIC 0x53465954U
#define TINYFS_VERSION 1U
#define TINYFS_NAME_LENGTH 16U
#define TINYFS_MAX_FILES 64U

enum {
    TINYFS_STATUS_OK = 0,
    TINYFS_STATUS_DISK_ERROR = -1,
    TINYFS_STATUS_NOT_FORMATTED = -2,
    TINYFS_STATUS_FILE_NOT_FOUND = -3,
    TINYFS_STATUS_NO_SPACE = -4,
    TINYFS_STATUS_INVALID_NAME = -5,
    TINYFS_STATUS_OUT_OF_MEMORY = -6
};

typedef struct {
    char name[TINYFS_NAME_LENGTH];
    uint32_t start_sector;
    uint32_t size_bytes;
    uint8_t used;
    uint8_t reserved[7];
} TinyFsFileEntry;

void tinyfs_init(void);
bool tinyfs_is_ready(void);
int8_t tinyfs_format(void);
int8_t tinyfs_list(void);
int8_t tinyfs_write_file(const char name[], const char content[]);
int8_t tinyfs_read_file(const char name[], char** out_content, uint32_t* out_size);
