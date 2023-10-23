#include "kernel/common.h"
#include "kernel/io.h"
#include "kernel/terminal.h"

#include <stdbool.h>
#include <stdint.h>

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

uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile ("inb %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

