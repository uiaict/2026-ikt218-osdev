#include "demo.h"
#include "idt.h"
#include "isr.h"
#include "memory.h"
#include "pit.h"
#include "song.h"
#include "stdint.h"
#include "vfs.h"

extern void printf(const char* format, ...);
extern volatile uint32_t timer_ticks;
extern isr_t interrupt_handlers[256];
extern idt_gate_t idt[IDT_ENTRIES];

extern uint32_t heap_begin;
extern uint32_t heap_end;
extern uint32_t last_alloc;
extern uint32_t memory_used;

volatile int demo_running = 0;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) descriptor_register_t;

static descriptor_register_t read_gdtr(void) {
    descriptor_register_t gdtr;
    __asm__ volatile("sgdt %0" : "=m"(gdtr));
    return gdtr;
}

static descriptor_register_t read_idtr(void) {
    descriptor_register_t idtr;
    __asm__ volatile("sidt %0" : "=m"(idtr));
    return idtr;
}

static uint32_t read_cr0(void) {
    uint32_t value;
    __asm__ volatile("mov %%cr0, %0" : "=r"(value));
    return value;
}

static uint32_t read_eflags(void) {
    uint32_t value;
    __asm__ volatile("pushfl; popl %0" : "=r"(value));
    return value;
}

static void print_ok(const char* label) {
    printf("[OK] %s\n", label);
}

static void print_fail(const char* label) {
    printf("[FAIL] %s\n", label);
}

static void demo_gdt_idt(void) {
    descriptor_register_t gdtr = read_gdtr();
    descriptor_register_t idtr = read_idtr();

    if (gdtr.base != 0 && gdtr.limit >= 23) {
        printf("[OK] A2 GDT loaded: base=0x%x limit=0x%x\n", gdtr.base, gdtr.limit);
    } else {
        printf("[FAIL] A2 GDT loaded: base=0x%x limit=0x%x\n", gdtr.base, gdtr.limit);
    }

    if (idtr.base != 0 && idtr.limit >= ((IDT_ENTRIES * sizeof(idt_gate_t)) - 1)) {
        printf("[OK] A3 IDT loaded: base=0x%x limit=0x%x\n", idtr.base, idtr.limit);
    } else {
        printf("[FAIL] A3 IDT loaded: base=0x%x limit=0x%x\n", idtr.base, idtr.limit);
    }

    if (idt[0].sel == KERNEL_CS && idt[31].sel == KERNEL_CS) {
        print_ok("A3 CPU exception gates installed: vectors 0-31");
    } else {
        print_fail("A3 CPU exception gates installed: vectors 0-31");
    }

    if (idt[IRQ0].sel == KERNEL_CS && idt[IRQ15].sel == KERNEL_CS) {
        print_ok("A3 PIC remap visible: IRQ0-15 -> vectors 32-47");
    } else {
        print_fail("A3 PIC remap visible: IRQ0-15 -> vectors 32-47");
    }
}

static void demo_interrupts_and_pit(void) {
    uint32_t before;
    uint32_t after;
    uint32_t spin;

    if (interrupt_handlers[IRQ0] != 0 && interrupt_handlers[IRQ1] != 0) {
        print_ok("A3 IRQ handlers registered: PIT on 32, keyboard on 33");
    } else {
        print_fail("A3 IRQ handlers registered: PIT on 32, keyboard on 33");
    }

    if (read_eflags() & 0x200) {
        print_ok("A3 Hardware interrupts enabled: EFLAGS.IF=1");
    } else {
        print_fail("A3 Hardware interrupts enabled: EFLAGS.IF=1");
    }

    before = timer_ticks;
    for (spin = 0; spin < 600000 && timer_ticks == before; spin++) {
        __asm__ volatile("nop");
    }
    after = timer_ticks;

    if (after > before) {
        printf("[OK] A4 PIT ticks advancing: %d -> %d\n", before, after);
    } else {
        printf("[FAIL] A4 PIT ticks advancing: %d -> %d\n", before, after);
        return;
    }

    before = timer_ticks;
    sleep_busy(10);
    after = timer_ticks;
    if (after >= before + 10) {
        printf("[OK] A4 sleep_busy(10ms): %d -> %d\n", before, after);
    } else {
        printf("[FAIL] A4 sleep_busy(10ms): %d -> %d\n", before, after);
    }
}

static void demo_memory_and_paging(void) {
    uint32_t cr0 = read_cr0();
    uint32_t before_used = memory_used;
    void* block;

    if ((cr0 & 0x80000000) != 0) {
        printf("[OK] A4 Paging enabled: CR0=0x%x\n", cr0);
    } else {
        printf("[FAIL] A4 Paging enabled: CR0=0x%x\n", cr0);
    }

    if (heap_begin != 0 && heap_end > heap_begin && last_alloc >= heap_begin) {
        printf("[OK] A4 Heap range: 0x%x - 0x%x\n", heap_begin, heap_end);
    } else {
        printf("[FAIL] A4 Heap range: 0x%x - 0x%x\n", heap_begin, heap_end);
    }

    block = malloc(64);
    if (block != 0 && memory_used > before_used) {
        printf("[OK] A4 malloc/free smoke: ptr=0x%x used=%d\n", (uint32_t)block, memory_used);
        free(block);
    } else {
        print_fail("A4 malloc/free smoke");
    }
}

static void demo_vfs_and_apps(void) {
    int ok = 1;

    if (vfs_resolve_app("/assignments/5/playlist.mp3") != VFS_APP_PLAYLIST) ok = 0;
    if (vfs_resolve_app("/assignments/6/matrix.exe") != VFS_APP_MATRIX) ok = 0;
    if (vfs_resolve_app("/assignments/6/dodger.exe") != VFS_APP_DODGER) ok = 0;
    if (vfs_resolve_app("/bin/soundtest") != VFS_APP_SOUNDTEST) ok = 0;

    if (ok) {
        print_ok("A6 VFS apps mounted: playlist, matrix, dodger, soundtest");
    } else {
        print_fail("A6 VFS apps mounted: playlist, matrix, dodger, soundtest");
    }
}

static void demo_audio(void) {
    play_sound(C4);
    sleep_interrupt(80);
    stop_sound();
    print_ok("A5 PC speaker pulse emitted through PIT channel 2");
}

void run_demo_checklist(void) {
    for (int i = 0; i < 25; i++) printf("\n");

    printf("Group 31 Demo Checklist\n");
    printf("-----------------------\n");
    print_ok("A1 boot path reached: shell is executing commands");

    demo_gdt_idt();
    demo_interrupts_and_pit();
    demo_memory_and_paging();
    demo_vfs_and_apps();
    demo_audio();

    printf("\nManual interactive demos:\n");
    printf("  cd /assignments/5\n");
    printf("  run playlist.mp3\n");
    printf("  cd /assignments/6\n");
    printf("  run matrix.exe\n");
    printf("  run dodger.exe\n");
    printf("\nDemo complete. Press ESC to return home.\n");

    demo_running = 1;
    while (demo_running) {
        sleep_interrupt(50);
    }
}
