#pragma once
#include <stdbool.h>
#include <stdint.h>


#define ring_buffer_SIZE 256

typedef struct {
  void** buffer;
  size_t element_size;
  volatile uint32_t head;
  volatile uint32_t tail;
} ring_buffer_t;

typedef struct {
  uint8_t buffer[ring_buffer_SIZE];
  volatile uint32_t head;
  volatile uint32_t tail;
} ring_buffer8_t;

void rb_init(ring_buffer8_t* rb);

// Returns true if successful, false if the buffer is full
bool rb_push(ring_buffer8_t* rb, uint8_t data);

// Returns true if successful, false if the buffer is empty.
bool rb_pop(ring_buffer8_t* rb, uint8_t* out_data);

// Returns true if head == tail
bool rb_is_empty(const ring_buffer8_t* rb);
