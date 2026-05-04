#ifndef MENU_H
#define MENU_H

#define MAIN_MENU 0
#define PAINT_MENU 1
#define TYPEGAME_MENU 2

#include "libc/stdbool.h"

struct button {
    const char* label;
    void (*action)();
};
extern int current_menu;

void init_menu();
void draw_window(const char* title);
void draw_vertical_line(int x);
void print_button(struct button* btn, bool is_selected, int x, int y);


#endif // MENU_H