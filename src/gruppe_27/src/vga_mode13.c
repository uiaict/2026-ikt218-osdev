#include "vga_mode13.h"
#include "common.h"

// ─── VGA register port addresses ─────────────────────────────────────────────
#define VGA_MISC_WRITE      0x3C2
#define VGA_SEQ_INDEX       0x3C4
#define VGA_SEQ_DATA        0x3C5
#define VGA_PELADDR_WRITE   0x3C8
#define VGA_PELDATA         0x3C9
#define VGA_GC_INDEX        0x3CE
#define VGA_GC_DATA         0x3CF
#define VGA_AC_INDEX        0x3C0
#define VGA_AC_WRITE        0x3C0
#define VGA_AC_READ         0x3C1
#define VGA_CRTC_INDEX      0x3D4
#define VGA_CRTC_DATA       0x3D5
#define VGA_INSTAT_READ     0x3DA

// ─── Mode 13h register table ─────────────────────────────────────────────────
// Format: { index, value } pairs, terminated by 0xFF

static const uint8_t misc_val = 0x63;

static const uint8_t seq_regs[] = {
    0x03, 0x01, 0x0F, 0x00, 0x0E
};

static const uint8_t crtc_regs[] = {
    0x5F,0x4F,0x50,0x82,0x54,0x80,0xBF,0x1F,
    0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00,
    0x9C,0x0E,0x8F,0x28,0x40,0x96,0xB9,0xA3,
    0xFF
};

static const uint8_t gc_regs[] = {
    0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0F,0xFF
};

static const uint8_t ac_regs[] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x41,0x00,0x0F,0x00,0x00
};

static void write_regs() {
    // Miscellaneous output
    outb(VGA_MISC_WRITE, misc_val);

    // Sequencer
    for (uint8_t i = 0; i < 5; i++) {
        outb(VGA_SEQ_INDEX, i);
        outb(VGA_SEQ_DATA, seq_regs[i]);
    }

    // Unlock CRTC registers 0-7
    outb(VGA_CRTC_INDEX, 0x03);
    outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);

    // CRTC
    for (uint8_t i = 0; i < 25; i++) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, crtc_regs[i]);
    }

    // Graphics controller
    for (uint8_t i = 0; i < 9; i++) {
        outb(VGA_GC_INDEX, i);
        outb(VGA_GC_DATA, gc_regs[i]);
    }

    // Attribute controller — must read 0x3DA first to reset flip-flop
    inb(VGA_INSTAT_READ);
    for (uint8_t i = 0; i < 21; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, ac_regs[i]);
    }
    // Re-enable display
    outb(VGA_AC_INDEX, 0x20);
}

void vga_enter_mode13() {
    write_regs();
}

// ─── Return to VGA text mode 3 (80x25) ───────────────────────────────────────
// Standard register values for mode 3
static const uint8_t text_misc   = 0x67;
static const uint8_t text_seq[]  = { 0x03,0x00,0x03,0x00,0x02 };
static const uint8_t text_crtc[] = {
    0x5F,0x4F,0x50,0x82,0x55,0x81,0xBF,0x1F,
    0x00,0x4F,0x0D,0x0E,0x00,0x00,0x00,0x50,
    0x9C,0x0E,0x8F,0x28,0x1F,0x96,0xB9,0xA3,
    0xFF
};
static const uint8_t text_gc[]   = { 0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0xFF };
static const uint8_t text_ac[]   = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x0C,0x00,0x0F,0x08,0x00
};

void vga_exit_mode13() {
    outb(VGA_MISC_WRITE, text_misc);

    for (uint8_t i = 0; i < 5; i++) {
        outb(VGA_SEQ_INDEX, i);
        outb(VGA_SEQ_DATA, text_seq[i]);
    }

    outb(VGA_CRTC_INDEX, 0x03);
    outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);

    for (uint8_t i = 0; i < 25; i++) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, text_crtc[i]);
    }

    for (uint8_t i = 0; i < 9; i++) {
        outb(VGA_GC_INDEX, i);
        outb(VGA_GC_DATA, text_gc[i]);
    }

    inb(VGA_INSTAT_READ);
    for (uint8_t i = 0; i < 21; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, text_ac[i]);
    }
    outb(VGA_AC_INDEX, 0x20);
}

// ─── Pixel drawing ───────────────────────────────────────────────────────────

void vga_put_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= VGA_MODE13_WIDTH)  return;
    if (y < 0 || y >= VGA_MODE13_HEIGHT) return;
    VGA_FRAMEBUFFER[y * VGA_MODE13_WIDTH + x] = color;
}

void vga_clear(uint8_t color) {
    for (int i = 0; i < VGA_MODE13_WIDTH * VGA_MODE13_HEIGHT; i++) {
        VGA_FRAMEBUFFER[i] = color;
    }
}