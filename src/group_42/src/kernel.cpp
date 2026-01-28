extern "C" int kernel_main(){
    // VGA text mode buffer at 0xB8000
    volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    
    // White text on black background (0x0F = white, 0x00 = black)
    const char* message = "Hello from group42 OS!";
    
    for (int i = 0; message[i] != '\0'; i++) {
        vga[i] = (unsigned short)message[i] | 0x0F00;
    }
    
    // Infinite loop to keep the kernel running
    while(1) {
        asm volatile("hlt");
    }
    
    return 0;
}