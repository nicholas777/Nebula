#ifndef _IO_H
#define _IO_H

#include <stddef.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port); 

// Serial
int init_serial();

char read_serial();
void write_serial(char c);

#endif
