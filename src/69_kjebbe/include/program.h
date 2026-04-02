#ifndef PROGRAM_H
#define PROGRAM_H

typedef enum {
  PROGRAM_SHELL = 0,
  PROGRAM_PIANO,
} program_t;

extern program_t active_program;

#endif
