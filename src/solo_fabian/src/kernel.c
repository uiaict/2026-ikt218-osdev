typedef unsigned short uint16_t;
typedef unsigned int size_t;

void main(void) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    const char* msg = "IKT218 kernel loaded";

    for (size_t i = 0; msg[i] != '\0'; i++) {
        vga[i] = (uint16_t)msg[i] | (uint16_t)(0x0F << 8);
    }

    for (;;) {
        __asm__ volatile ("hlt");
    }
}