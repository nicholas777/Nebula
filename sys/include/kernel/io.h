#ifndef __IO_H
#define __IO_H

#include <stddef.h>
#include <stdint.h>

inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8_t inb(uint16_t port); 
#endif
