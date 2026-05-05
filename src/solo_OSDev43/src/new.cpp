typedef unsigned long size_t;

extern "C" void* malloc(size_t size);

void* operator new(size_t size) {
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    (void)ptr;
}

void operator delete[](void* ptr) noexcept {
    (void)ptr;
}

void operator delete(void* ptr, size_t size) noexcept {
    (void)ptr;
    (void)size;
}

void operator delete[](void* ptr, size_t size) noexcept {
    (void)ptr;
    (void)size;
}