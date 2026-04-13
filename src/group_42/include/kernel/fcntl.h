#pragma once
#include <stdint.h>

/*
 * FILE OPEN FLAGS
 * Values match Linux/i386 fcntl.h.
 * Source: https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/fcntl.h
 */

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 64
#define O_APPEND 8
#define O_EXCL 128
#define O_TRUNC 512
