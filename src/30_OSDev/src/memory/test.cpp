#include <stdint.h>
extern "C" {
#include "../terminal.h"
}

class MyClass {
public:
    int a;
    int b;
};

extern "C" void test_new()
{
    int* number = new int(42);
    MyClass* obj = new MyClass();

    terminal_write("number allocated at: 0x");
    terminal_write_hex((uint32_t)number);
    terminal_write("\n");

    terminal_write("object allocated at: 0x");
    terminal_write_hex((uint32_t)obj);
    terminal_write("\n");
}