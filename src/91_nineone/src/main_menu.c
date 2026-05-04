#include "menu.h"
#include "main_menu.h"
#include "apps/paint/paint.h"
#include "terminal.h"
#include "libc/string.h"
#include "libc/stdbool.h"
#include "colors.h"
#include "Music_Player/song.h"
#include "kernel/memory.h"
#include "pit.h"
#include "libc/stdio.h"
#include "apps/typegame/typegame.h"

static int selected_item = 0;
static volatile int pending_action = -1; // -1 no action, < -1 pending action 
static volatile int action_running = 0;
static bool enter_down = false;
#define NUM_OPTIONS 4

void play_music(void);
void play_song_impl(Song* song);

void test_action() {
    terminal_clear(COLOR(WHITE, BLACK));
    draw_window("System tests");

    printf_color(COLOR(YELLOW, BLUE), "FreDDaviDOS system test");

    printf("IDT: loaded");

    print_memory_layout();

    uint32_t t1 = get_current_tick();
    sleep_interrupt(100);
    uint32_t t2 = get_current_tick();

    if (t2 > t1) {
        printf("PIT IRQ0: ok");
    } else {
        printf("PIT IRQ0: failed");
    }

    void* a = malloc(12345);
    void* b = malloc(54321);
    void* c = malloc(13331);

    if (a && b && c && a != b && b != c && a != c) {
        printf("malloc: ok");
    } else {
        printf("malloc: failed");
    }

    free(a);
    free(b);
    free(c);

    printf("a=0x%x b=0x%x", (uint32_t)a, (uint32_t)b);
    printf("c=0x%x", (uint32_t)c);

    printf("busy sleep: testing");
    sleep_busy(250);
    printf("busy sleep: done");

    printf("int sleep: testing");
    sleep_interrupt(250);
    printf("int sleep: done");

    printf("Returning to main menu...");
    sleep_interrupt(3000);
    enter_main_menu();
}

void play_music(void) {
    pending_action = -1;

    terminal_clear(COLOR(WHITE, BLACK));
    draw_window("Music Player");


    printf("Song of Storms started...");

    Song song = {
        music_1,
        sizeof(music_1) / sizeof(Note)
    };

    play_song_impl(&song);

    printf("Song finished.");
    printf("Returning to main menu...");

    sleep_interrupt(1000);
    enter_main_menu();
}

struct button start_menu[] = {
    {"Print info", test_action},
    {"Paint program", enter_paint_program},
    {"Play typegame", typegame_start},
    {"Play music", play_music}
};

// handles realtime updates. Like UI
void main_menu_update(void) {
    if (pending_action < 0) {
        return;
    }

    int action = pending_action;
    pending_action = -1;

    start_menu[action].action();
}

static void draw_buttons() {

    int num_buttons = sizeof(start_menu) / sizeof(start_menu[0]);
    int start_x = 30;
    int start_y = 10;

    for (int i = 0; i < num_buttons; i++) {
        bool is_selected = (i == selected_item);
        print_button(&start_menu[i], is_selected, start_x, start_y + i * 3);
    }
}

// handles inputs in main menu
void handle_main_menu_keyboard(uint8 scancode) {
    switch (scancode) {
        case 0x11: // W
            selected_item = (selected_item - 1 + NUM_OPTIONS) % NUM_OPTIONS;
            draw_buttons();
            break;

        case 0x1F: // S
            selected_item = (selected_item + 1) % NUM_OPTIONS;
            draw_buttons();
            break;
            
        case 0x1C: // Enter
            pending_action = selected_item;
            break;

        default:
            break;
    }
}

void enter_main_menu() {
    pending_action = -1;
    enter_down = false;


    terminal_clear(0);
    draw_window("Main Menu");
    draw_buttons();

    current_menu = MAIN_MENU;
}