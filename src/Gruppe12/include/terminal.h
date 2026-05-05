#ifndef TERMINAL_H
#define TERMINAL_H

// Initialiserer VGA-terminalen (tømmer skjermen)
void terminal_init(void);

// Skriver tekst til skjermen
// Bruker VGA tekstmodus for å vise output fra OS-et
void terminal_write(const char* str);

#endif