#include "../include/matrix.h"
#include "../include/keyboard.h"
#include "../include/memory.h"
#include "../include/pit.h"

// This is a simple matrix rain application
// It will produce random letters that appear to be falling down in the window
// made accessible as an option in the main menu function from kernel.c

// Simple Random Number Generator 
static unsigned int rand_seed = 12345;

void srand(unsigned int seed) {
    rand_seed = seed;
}

unsigned int rand(void) {
    // Linear Congruential Generator (simple but effective for our purposes)
    rand_seed = (rand_seed * 1103515245 + 12345) & 0x7fffffff;
    return rand_seed;
}

// Get random number in range [min, max]
unsigned int rand_range(unsigned int min, unsigned int max) {
    if (min >= max) return min;
    return min + (rand() % (max - min + 1));
}


// Matrix Rain Animation


typedef struct {
    int x;
    int y;
    int speed;
    int length;
    int frame_counter;
} MatrixDrop;

#define MAX_DROPS 15
#define MATRIX_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()"

// Get a random character from the matrix character set
static char get_matrix_char(void) {
    const char* chars = MATRIX_CHARS;
    int len = 0;
    while (chars[len]) len++;
    return chars[rand() % len];
}

// function for running the mattrix rain simulation
void run_matrix_rain(void) {
    // Clear screen and set up for animation
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    
    // Animation starts after the header (row 5)
    int animation_start_y = 5;
    
    // Allocate drops
    MatrixDrop* drops = (MatrixDrop*)malloc(sizeof(MatrixDrop) * MAX_DROPS);
    if (!drops) {
        printf("Memory allocation failed!\n");
        return;
    }
    // Clear again after malloc in case it printed anything
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK); 
    
    // Initialize drops
    for (int i = 0; i < MAX_DROPS; i++) {
        drops[i].x = rand_range(0, VGA_WIDTH - 1);
        drops[i].y = rand_range(animation_start_y, VGA_HEIGHT - 1);
        drops[i].speed = rand_range(3, 8);  // frames between moves
        drops[i].length = rand_range(5, 15);
        drops[i].frame_counter = 0;
    }
    
    int animation_running = 1;
    unsigned long frame_count = 0;
    
    while (animation_running) {
        frame_count++;
        
        // Check for '0' key press (non-blocking) - check every frame
        char c = keyboard_check();
        if (c == '0') {
            animation_running = 0;
            break;
        }
        
        // Update and draw each drop
        for (int i = 0; i < MAX_DROPS; i++) {
            MatrixDrop* drop = &drops[i];
            
            // Increment frame counter
            drop->frame_counter++;
            
            // Move drop down at its own speed
            if (drop->frame_counter >= drop->speed) {
                drop->frame_counter = 0;
                drop->y++;
                
                // Reset if off screen
                if (drop->y >= VGA_HEIGHT) {
                    drop->y = animation_start_y;
                    drop->x = rand_range(0, VGA_WIDTH - 1);
                    drop->speed = rand_range(3, 8);
                    drop->length = rand_range(5, 15);
                }
            }
            
            // Clear the old position by drawing a space
            int clear_y = drop->y + drop->length;
            if (clear_y >= animation_start_y && clear_y < VGA_HEIGHT && drop->x < VGA_WIDTH) {
                vga_set_color(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
                vga_set_cursor(drop->x, clear_y);
                vga_putchar(' ');
            }
            
            // Draw the cool trail effect
            for (int j = 0; j < drop->length; j++) {
                int draw_y = drop->y - j;
                
                if (draw_y >= animation_start_y && draw_y < VGA_HEIGHT && drop->x < VGA_WIDTH) {
                    // make it brighter at top, less bright at bottom
                    if (j == 0) {
                        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    } else if (j < drop->length / 2) {
                        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
                    } else {
                        vga_set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
                    }
                    
                    // Draw a random english character
                    vga_set_cursor(drop->x, draw_y);
                    vga_putchar(get_matrix_char());
                }
            }
        }
        
        // Small delay for animation speed
        sleep_interrupt(50);
    }
    
    // Cleanup
    free(drops);
    vga_clear();
    
    // Reset colors
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("Returning to menu...\n");
}
