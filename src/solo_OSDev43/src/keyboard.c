typedef unsigned char uint8_t;

char keymap[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', // 0-9
    '9', '0', '-', '=', '\b',                      // Backspace
    '\t',                                          // Tab
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', // 10-19
    '[', ']', '\n',                                // Enter
    0,                                             // Ctrl
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',    // 20-29
    ';', '\'', '`',
    0,                                             // Left Shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',             // 30-36
    'm', ',', '.', '/', 
    0,                                             // Right Shift
    '*',
    0,                                             // Alt
    ' ',                                           // Space
    0,                                             // Caps Lock
    // Rest unused
};