#include "terminal.c"
#include "colors.h"
#define terminal_write print

int main() {
    char *video_memory = (char*) 0xB8000; // startadresse for VGA tekstmodus

    const char *str = "Velkommen til DaviDOS!";
    
    print("Velkommen til FreDDaviDOS!", COLOR(YELLOW, BLUE), 0, 0);

    while (1) { // coming soon
    }

    return 0;
}