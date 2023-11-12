#include "kernel/common.h"
#include "kernel/io.h"
#include "kernel/terminal.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void kpanic() {
    while (true)
        ;
}

void kpanic_err(const char* msg) {
    terminal_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    terminal_writestring(msg);

    while (true)
        ;
}

void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile ("inb %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

void serial_print(const char *msg) {
    for (size_t i = 0; i < strlen(msg); i++) {
        write_serial(msg[i]);
    }
}

