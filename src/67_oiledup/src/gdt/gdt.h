#ifndef GDT_H
#define GDT_H

// If this header is included in C++ code, tell it to use C linkage
#ifdef __cplusplus
extern "C" {
#endif

struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

// Use 'extern' to declare these, but don't allocate memory for them here
extern struct gdt_entry gdt[3];
extern struct gdt_ptr gp;

extern void gdt_flush();
extern void gdt_install();

#ifdef __cplusplus
}
#endif

#endif // GDT_H