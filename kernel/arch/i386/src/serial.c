#include "kernel/io.h"

#define PORT 0x3f8

int init_serial() {
    outb(PORT + 1, 0x00); // Clear all interrupts
    outb(PORT + 3, 0x80); // Enable DLAB (specify baud rate)
    outb(PORT + 0, 0x03); // Specify Baud rate to be max/3
    outb(PORT + 1, 0x00); // Higher byte  of baud rate (0)

    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(PORT + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(PORT + 4, 0x0F);
    return 0;
}

char read_serial() {
    while ((inb(PORT + 5) & 1) == 0)
        ; // No data has been received

    return inb(PORT);
}

void write_serial(char c) {
    while ((inb(PORT + 5) & 0x20) == 0)
        ; // Not empty

    outb(PORT, c);
}

