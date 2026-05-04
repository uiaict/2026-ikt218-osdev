#include <memory.h>

#include <libc/stdio.h>

void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void* memory) noexcept
{
    free(memory);
}

void operator delete[](void* memory) noexcept
{
    free(memory);
}

void operator delete(void* memory, size_t size) noexcept
{
    (void)size;
    free(memory);
}

void operator delete[](void* memory, size_t size) noexcept
{
    (void)size;
    free(memory);
}

extern "C" void test_new_operator(void)
{
    uint32_t* value = new uint32_t;
    *value = 218;
    printf("new allocated value: %u at 0x%x\n", *value, (uint32_t)value);
    delete value;
}
