int main() {
    char *video_memory = (char*) 0xB8000; // startadresse for VGA tekstmodus

    const char *str = "Velkommen til DaviDOS!";
    
    for (int i = 0; str[i] != '\0'; i++) {
        // 2x: char, 2x+1: attributt (farge)
        video_memory[i * 2] = str[i];
        video_memory[i * 2 + 1] = 0x1F; 
    }

    while (1) { // coming soon
    }

    return 0;
}