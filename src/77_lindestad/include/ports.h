#pragma once

#include <libc/stdint.h>

uint8_t port_byte_in(uint16_t port);
void port_byte_out(uint16_t port, uint8_t data);
void port_io_wait(void);
