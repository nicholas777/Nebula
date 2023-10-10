
#include <string.h>
#include "kernel/terminal.h"

size_t column, row;
uint16_t* vga_textbuf;
terminal_color_t fg_color, bg_color;

void terminal_init() {
    column = 0;
    row = 0;
    vga_textbuf = (uint16_t*)VGA_TEXT_BUFFER;

    terminal_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    terminal_clear();
}

// Does not affect the cursor
void terminal_writechar_pos(unsigned char c, size_t terminal_row, size_t terminal_column) {
    vga_textbuf[terminal_row * VGA_TEXT_WIDTH + terminal_column] = format_vga_entry(fg_color, bg_color, c);
}

void terminal_writechar(unsigned char c) {
    if (c == '\n') {
        column = 0;
        row++;
        if (row == VGA_TEXT_HEIGHT)
            row = 0;

        return;
    }

    terminal_writechar_pos(c, row, column);
    column++;

    if (column == VGA_TEXT_WIDTH) {
        column = 0;
        row++;

        if (row == VGA_TEXT_HEIGHT) {
            row = 0;
        }
    }
}

// Does not affect the cursor
void terminal_writestring_pos(const char* str, size_t row, size_t column) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\n') {
            column = 0;
            row++;
            if (row >= VGA_TEXT_HEIGHT)
                row = 0;

            continue;
        }

        terminal_writechar_pos(str[i], row, column);
        column++;

        if (column >= VGA_TEXT_WIDTH) {
            column = 0;
            row++;

            if (row >= VGA_TEXT_HEIGHT)
                row = 0;
        }
    }
}

void terminal_writestring(const char* str) {
    for (size_t i = 0; i < strlen(str); i++) {
        terminal_writechar(str[i]);
    }
}

void terminal_set_color(terminal_color_t fg, terminal_color_t bg) {
    fg_color = fg;
    bg_color = bg;
}

// Does not affect the color used in other functions
void terminal_clear_color(terminal_color_t color) {
    for (size_t i = 0; i < VGA_TEXT_HEIGHT; i++) {
        for (size_t j = 0; j < VGA_TEXT_WIDTH; j++)
            vga_textbuf[i * VGA_TEXT_WIDTH + j] = format_vga_entry(VGA_COLOR_BLACK, color, ' ');
    }
}

void terminal_clear() {
    terminal_clear_color(bg_color);
}


