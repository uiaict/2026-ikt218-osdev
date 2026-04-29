typedef __SIZE_TYPE__ size_t;

extern "C" void *malloc(size_t);
extern "C" void  free(void *);

void *operator new  (size_t s)           { return malloc(s); }
void *operator new[](size_t s)           { return malloc(s); }
void  operator delete  (void *p)         noexcept { free(p); }
void  operator delete[](void *p)         noexcept { free(p); }
void  operator delete  (void *p, size_t) noexcept { free(p); }
void  operator delete[](void *p, size_t) noexcept { free(p); }
