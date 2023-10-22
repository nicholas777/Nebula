#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <stdint.h>

#if defined(__IS_LIBK)
#include "kernel/terminal.h"
#endif

int putchar(int c) {
#if defined(__IS_LIBK)
    terminal_writechar((uint8_t)c);
    return c;
#else
    return 0;
#endif
}

int __puts_internal(const char* str) {
#if defined(__IS_LIBK)
    terminal_writestring(str);
    return 1;
#else
    return -1;
#endif
}

int puts(const char* str) {
    __puts_internal(str);
    putchar('\n');
    return 1;
}

#include <stdarg.h>

// Returns the amount of digits in i
int _uintlen(unsigned int i, int base) {
    if (i == 0)
        return 1;

    int result = 0;
    while (i != 0) {
        i /= base;
        result++;

        if (i == base)
            result--;
    }

    return result;
}

// Returns the amount of digits in i
int _intlen(int i, int base) {
    if (i == 0)
        return 1;

    int result = i < 0 ? 1 : 0;
    i = i < 0 ? i * (-1) : i;
    while (i != 0) {
        i /= base;
        result++;

        if (i == base)
            result--;
    }

    return result;
}

char* hex_toupper(char* hex) {
    for (int i = 0; i < strlen(hex); i++) {
        if (hex[i] >= 'a' && hex[i] <= 'f') 
            hex[i] += 'A' - 'a';
    }

    return hex;
}

int printf(const char* format, ...) {
    int numchars = 0;

    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            if (format[1] == '%') {
                putchar('%');
                numchars++;
            }
            else if (format[1] == 'c') {
                int arg = va_arg(args, int);
                putchar(arg);
                numchars++;
            }
            else if (format[1] == 's') {
                const char* arg = va_arg(args, const char*);
                __puts_internal(arg);
                numchars += strlen(arg);
            }
            else if (format[1] == 'u') {
                unsigned int arg = va_arg(args, unsigned int);
                int arglen = _uintlen(arg, 10);
                char buf[arglen+1];
                buf[arglen] = '\0';
                __puts_internal(uitoa(arg, buf, 10));
                numchars += arglen;
            }
            else if (format[1] == 'o') {
                unsigned int arg = va_arg(args, unsigned int);
                int arglen = _uintlen(arg, 8);
                char buf[arglen+1];
                buf[arglen] = '\0';
                __puts_internal(uitoa(arg, buf, 8));
                numchars += arglen;
            }
            else if (format[1] == 'x') {
                unsigned int arg = (uint32_t)va_arg(args, uint32_t);
                int arglen = _uintlen(arg, 16);
                char buf[arglen+1];
                buf[arglen] = '\0';
                __puts_internal(uitoa(arg, buf, 16));
                numchars += arglen;
            }
            else if (format[1] == 'X') {
                unsigned int arg = va_arg(args, unsigned int);
                int arglen = _uintlen(arg, 16);
                char buf[arglen+1];
                buf[arglen] = '\0';
                __puts_internal(hex_toupper(uitoa(arg, buf, 16)));
                numchars += arglen;
            }
            else if (format[1] == 'd' || format[1] == 'i') {
                int arg = va_arg(args, int);
                int arglen = _intlen(arg, 10);
                char buf[arglen+1];
                buf[arglen] = '\0';
                __puts_internal(itoa(arg, buf, 10));
                numchars += arglen;
            }

            format++;
        }
        else {
            putchar(*format);
            numchars++;
        }

        format++;
    }

    va_end(args);

    return numchars;
}




