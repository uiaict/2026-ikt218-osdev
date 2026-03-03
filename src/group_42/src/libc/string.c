#include "libc/string.h"

#include "libc/stdint.h"

size_t strlen(const char* str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

int strcmp(const char* l, const char* r) {
  for (; *l == *r && *l; l++, r++)
    ;
  return *(unsigned char*)l - *(unsigned char*)r;
}
