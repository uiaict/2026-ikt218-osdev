#include "util.h"
#include "screen.h"

//void memset(void *dest, char val, uint32_t count){
//    char *temp = (char*) dest;
//    for (; count != 0; count --){
//        *temp++ = val;
//    }
//}

void outPortB(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a" (value), "Nd" (port));

}

uint8_t inPortB(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
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