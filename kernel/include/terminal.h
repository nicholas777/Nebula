#ifndef __TERMINAL_H
#define __TERMINAL_H

#include "vga.h"

#include <stddef.h>
#include <stdint.h>

typedef vga_color_t terminal_color_t;

void terminal_init();

// Does not affect the cursor
void terminal_writechar_pos(unsigned char c, size_t row, size_t column);
void terminal_writechar(unsigned char c);

// Does not affect the cursor
void terminal_writestring_pos(const char* str, size_t row, size_t column);
void terminal_writestring(const char* str);

void terminal_set_color(terminal_color_t fg, terminal_color_t bg);

// Does not affect the color used in other functions
void terminal_clear_color(terminal_color_t color);
void terminal_clear();

#endif


