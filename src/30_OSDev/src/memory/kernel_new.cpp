#include <stddef.h>

extern "C" void* malloc(unsigned int size);
extern "C" void free(void* ptr);

// Global new
void* operator new(unsigned long size)
{
    return malloc(size);
}

// Array new
void* operator new[](unsigned long size)
{
    return malloc(size);
}

// Delete
void operator delete(void* ptr)
{
    free(ptr);
}

// Array delete
void operator delete[](void* ptr)
{
    free(ptr);
}