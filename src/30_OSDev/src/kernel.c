#include "gdt.h"
#include "terminal.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "memory/kernel_memory.h"
#include "memory/paging.h"
#include "pit/pit.h"
#include <stdint.h>               // for uint32_t
#include "song_player/song_player.h"
#include "song_player/song.h"
#include "game/snake.h"

extern uint32_t end; // Used to determine where free memory begins
extern volatile uint32_t tick;

struct MyClass { int a; int b; };   // Dummy class for testing new operator
extern void test_new(void);

uint32_t last_tick = 0;

// GLOBAL SPEED VARIABLE
int speed = 200;

void update()
{
    player_x++;

    // Keep within screen bounds
    if (player_x >= 79)
        player_x = 1;
}

void draw()
{
    terminal_clear();
    draw_border();
    terminal_put_at('O', 0x0A, player_x, player_y);
}

void main(void)
{
    gdt_init();
    idt_init();
    pic_remap();
    init_pit();

    asm volatile("sti"); // Enable interrupts

    init_kernel_memory(&end);
    init_paging();

    terminal_clear();
    draw_border();

    game_init();  

    uint32_t counter = 0;

    while (1)
    {
        if (tick != last_tick)
        {
            last_tick = tick;
            counter++;

            if (counter % speed == 0)
            {
                game_update(); 
                game_draw();    
            }
        }
    }
}