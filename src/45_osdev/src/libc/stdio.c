#include "../../include/libc/stdio.h"
#include "../../include/libc/stdarg.h"
#include "../../include/vga.h"

// Printf implementation
int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int count = 0;
    
    while (*format) {
        if (*format == '%' && *(format + 1)) {
            format++;
            
            if (*format == 's') {
                const char *str = va_arg(args, const char*);
                while (*str) {
                    vga_putchar(*str++);
                    count++;
                }
            } else if (*format == 'd') {
                int num = va_arg(args, int);
                
                // for handeling negative numbers
                if (num < 0) {
                    vga_putchar('-');
                    count++;
                    num = -num;
                }
                
                // Converts to string and print
                char digits[20];
                int idx = 0;
                
                if (num == 0) {
                    digits[idx++] = '0';
                } else {
                    while (num > 0) {
                        digits[idx++] = '0' + (num % 10);
                        num /= 10;
                    }
                }
                
                // print in the reverse order
                for (int i = idx - 1; i >= 0; i--) {
                    vga_putchar(digits[i]);
                    count++;
                }
            } else if (*format == 'c') {
                char ch = (char)va_arg(args, int);
                vga_putchar(ch);
                count++;
            } else if (*format == 'x' || *format == 'X') {
    		uint32_t num = va_arg(args, uint32_t);
    
    		char digits[9];
    		int idx = 0;
    
    		if (num == 0) {
        	digits[idx++] = '0';
    		} else {
        	    while (num > 0) {
            		int nibble = num % 16;
            		digits[idx++] = nibble < 10 ? '0' + nibble : 'a' + nibble - 10;
            		num /= 16;
        	    }
    		}
    
    		for (int i = idx - 1; i >= 0; i--) {
        	    vga_putchar(digits[i]);
        	    count++;
    		}

	    } else if (*format == 'p') {
    		uint32_t num = va_arg(args, uint32_t);
    
    		vga_putchar('0'); vga_putchar('x');
    		count += 2;
    
   		// Always print 8 hex digits for pointers
    		for (int i = 7; i >= 0; i--) {
        	    int nibble = (num >> (i * 4)) & 0xF;
        	    vga_putchar(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
        	    count++;
    		}
	    } else if (*format == '%') {
                vga_putchar('%');
                count++;
            }
        } else if (*format == '\\' && *(format + 1) == 'n') {
            vga_putchar('\n');
            count++;
            format++;
        } else {
            vga_putchar(*format);
            count++;
        }
        
        format++;
    }
    
    va_end(args);
    return count;
}



