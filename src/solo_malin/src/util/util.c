#include "util.h"

//void memset(void *dest, char val, uint32_t count){
//    char *temp = (char*) dest;
//    for (; count != 0; count --){
//        *temp++ = val;
//    }
//}

void outPortB(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a" (value), "Nd" (port));

}

void panic(const char* message)
{
    write_string("KERNEL PANIC: ");
    write_string(message);
    write_string("\n");

    for (;;) {
        __asm__ __volatile__("cli; hlt");
    }
}