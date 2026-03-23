// Standart kütüphane yasak olduğu için temel tipler
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

/* --- 1. GDT MEKANİZMASI --- */
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

extern void gdt_flush(uint32_t);

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (uint32_t)&gdt;
    
    gdt_set_gate(0, 0, 0, 0, 0);                // NULL Segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // CODE Segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // DATA Segment
    
    gdt_flush((uint32_t)&gp);
}

/* --- 2. YAZDIRMA FONKSİYONLARI --- */

// Ekrana (VGA) yazdır
void terminal_write(const char* str) {
    volatile uint16_t* video = (volatile uint16_t*)0xB8000;
    while(*str) *video++ = (uint16_t)*str++ | 0x0F00;
}

// VS Code Terminaline (Seri Port) yazdır
void serial_write(const char* str) {
    while(*str) {
        __asm__ volatile ( "outb %0, %1" : : "a"((uint8_t)*str++), "Nd"((uint16_t)0x3F8) );
    }
}

/* --- 3. ANA FONKSİYON (ENTRY POINT) --- */
void main(void) {
    gdt_install();
    
    terminal_write("Hello World");
    serial_write("Hello World\n");

    // Sistemi güvene al ve dondur
    __asm__ volatile ("cli");
    while (1) {
        __asm__ volatile ("hlt");
    }
}