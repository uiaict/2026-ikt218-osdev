#include "apps/typegame/typegame.h"
#include "libc/stdbool.h"
#include "libc/stdio.h"
#include "terminal.h"
#include "colors.h"
#include "libc/stdint.h"
#include "keyboard.h"
#include "main_menu.h"
#include "../src/pit.h"


#define MIN_X 1
#define MAX_X (VGA_WIDTH - 2)

#define MIN_Y 4
#define MAX_Y (VGA_HEIGHT - 2)

#define WPM_TEXT_POSITION_X 5
#define MISTAKES_TEXT_POSITION_X 40
#define TIME_TEXT_POSITION_X 17

static void typegame_draw();
static void typegame_end();
static void typegame_keyboard_handler(char c, int8 scancode);

static const char* text = "this sentence walks sideways into a quiet refrigerator. the clouds are thinking about spoons again. a bicycle whispers loudly under the carpet. nothing agrees with the color of yesterday and it hums anyway. the floor forgets to be flat while a pocket of oranges sings silently. everything continues but not in the direction it started.";
static int current_index = 0;
static int mistakes = 0;
static bool isRunning = false;

static float wpm = 0;
static uint32 start_ticks = 0;
static uint32 last_second_tracker = 0;



void handle_typegame_keyboard(uint8 scancode)
{
    char c = keyboard_scancode_to_ascii(scancode);

    if (scancode == 0x01) {
        typegame_end();
        return;
    }

    if (c != 0) {
        typegame_handle_key(c);
    }
}

void typegame_start() 
{
    current_index = 0;
    mistakes = 0;
    isRunning = true;

    // Clear screen
    terminal_clear(COLOR(WHITE, BLACK));
    
    // Draw
    typegame_draw();

    keyboard_set_event_handler(typegame_keyboard_handler);

    // Start timer
    start_ticks = get_current_tick();
}

void typegame_update()
{
    // Update timer
    // Update other UI (wpm) 
    
    if(isRunning == false) {
        return;
    }

    uint32 elapsed_ticks = get_current_tick() - start_ticks;
    uint32 elapsed_seconds = elapsed_ticks / PIT_TARGET_FREQUENCY;

    if (elapsed_seconds != last_second_tracker) {
        last_second_tracker = elapsed_seconds;

        // update timer display
        print_uint(elapsed_seconds, COLOR(LIGHT_RED, BLACK), TIME_TEXT_POSITION_X, 3);
        
        // update WPM (kind of fake WPM)
        if (elapsed_seconds > 0) {
            int correct = current_index - mistakes;
            int wpm = (correct * 60) / (5 * elapsed_seconds);
            terminal_write("     ", 0, WPM_TEXT_POSITION_X, 3);
            print_uint(wpm, COLOR(WHITE, BLACK), WPM_TEXT_POSITION_X, 3);
        }
        
    }

    if (elapsed_seconds >= 15) {
        isRunning = false;
    }
    
}

static void typegame_keyboard_handler(char character, int8 scancode) 
{
    (void)scancode;

    if(isRunning == false) return;

    typegame_handle_key(character);
}

void typegame_handle_key(char c)
{
    if(isRunning == false) 
    {
        return;
    }

    char expected = text[current_index];

    int width = MAX_X - MIN_X + 1;
    int x = MIN_X + (current_index % width);
    int y = MIN_Y + 1 + (current_index / width);

    if(c == '\b') 
    {
        if(current_index > 0) 
        {
            current_index--;

            int width = MAX_X - MIN_X + 1;
            int x = MIN_X + (current_index % width);
            int y = MIN_Y + 1 + (current_index / width);
            terminal_putchar(text[current_index], COLOR(LIGHT_GREY, BLACK), x, y);
        }

        return;
    }
    
    if(c == expected) {
        terminal_putchar(expected, COLOR(WHITE, GREEN), x, y);
    }
    else 
    {
        terminal_putchar(expected, COLOR(WHITE, RED), x, y);
        mistakes++;
        print_uint(mistakes, COLOR(RED, BLACK), MISTAKES_TEXT_POSITION_X, 3);
    }

    current_index++;

    if(text[current_index] == '\0') {
        typegame_end();
    }

}

static void typegame_end() 
{
    isRunning = false;
    enter_main_menu();
}

static void typegame_draw() 
{    
    
    terminal_write("______________________________________________________________________________"
        , COLOR(WHITE, BLACK), 1, 4);

        

    terminal_write(text, COLOR(LIGHT_GREY, BLACK), 1, MIN_Y+1);
    terminal_write("Mistakes", COLOR(WHITE, BLACK), MISTAKES_TEXT_POSITION_X, 2);
    print_uint(mistakes, COLOR(RED, BLACK), MISTAKES_TEXT_POSITION_X, 3);
    terminal_write("Time Elapsed", COLOR(WHITE, BLACK), TIME_TEXT_POSITION_X, 2);
    terminal_write("WPM", COLOR(WHITE, BLACK), WPM_TEXT_POSITION_X, 2);
    terminal_write("Press [ esc ] to exit", COLOR(WHITE, BLACK), 1, 23);
}
