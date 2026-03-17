#pragma once
#include <stdbool.h>
#include <stdint.h>


/**
 * Initialize the programmable interval timer
 */
void init_pit();

/**
 * Sleep using interrupts to wake up
 * @param milliseconds time to sleep for
 */
void sleep_interrupt(uint32_t milliseconds);

/**
 * Sleep by constantly checking for time left
 * @param milliseconds time to sleep for
 */
void sleep_busy(uint32_t milliseconds);
