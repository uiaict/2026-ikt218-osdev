#pragma once
#include <stdint.h>
#include <stdbool.h>

static inline void bit_set(uint8_t* var, uint8_t bit) {
  *var |= (uint8_t)(1u << bit);
}
static inline void bit_clear(uint8_t* var, uint8_t bit) {
  *var &= ~(uint8_t)(1u << bit);
}

static inline void bit_toggle(uint8_t* var, uint8_t bit) {
  *var ^= (uint8_t)(1u << bit);
}

static inline bool bit_test(uint8_t var, uint8_t bit) {
  return (var & (1 << bit)) != 0;
}
