#include <stdlib.h>
#include <string.h>

#include "kernel/log.h"
#include "kernel/memory.h"
#include "kernel/panic.h"


extern memory_info_t memory_info;


void free(void* mem) {
  alloc_t* alloc = (mem - sizeof(alloc_t));
  memory_info.memory_used -= alloc->size + sizeof(alloc_t);
  alloc->status = 0;
}

void* malloc(size_t size) {
  if (!size)
    return NULL;

  uint8_t* mem = (uint8_t*)memory_info.heap_begin;
  while ((uint32_t)mem < memory_info.last_alloc) {
    alloc_t* a = (alloc_t*)mem;
    log_debug("mem=0x%x a={.status=%d, .size=%d}\n", mem, a->status, a->size);

    if (!a->size)
      goto nalloc;
    if (a->status) {
      mem += a->size;
      mem += sizeof(alloc_t);
      mem += 4;
      continue;
    }
    // Not allocated but big enough, adjust size, set status, and return location
    if (a->size >= size) {
      a->status = 1;
      log_debug("RE:Allocated %d bytes from 0x%x to 0x%x\n", size, mem + sizeof(alloc_t),
                mem + sizeof(alloc_t) + size);
      memset(mem + sizeof(alloc_t), 0, size);
      memory_info.memory_used += size + sizeof(alloc_t);
      return (char*)(mem + sizeof(alloc_t));
    }

    mem += a->size;
    mem += sizeof(alloc_t);
    mem += 4;
  }

nalloc:;
  if (memory_info.last_alloc + size + sizeof(alloc_t) >= memory_info.heap_end) {
    kernel_panic("Cannot allocate bytes! Out of memory.\n");
  }
  alloc_t* alloc = (alloc_t*)memory_info.last_alloc;
  alloc->status = 1;
  alloc->size = size;

  memory_info.last_alloc += size;
  memory_info.last_alloc += sizeof(alloc_t);
  memory_info.last_alloc += 4;
  log_debug("Allocated %d bytes from 0x%x to 0x%x\n", size, (uint32_t)alloc + sizeof(alloc_t),
            memory_info.last_alloc);
  memory_info.memory_used += size + 4 + sizeof(alloc_t);
  memset((char*)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
  return (char*)((uint32_t)alloc + sizeof(alloc_t));
}
