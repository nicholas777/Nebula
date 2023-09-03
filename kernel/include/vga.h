#ifndef __VGA_H
#define __VGA_H

#include <stdint.h>
#include <stddef.h>

// VGA text mode 

#define VGA_TEXT_BUFFER 0xB8000
#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 25

typedef enum
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} vga_color_t;

typedef uint16_t vga_entry_t;

inline vga_entry_t format_vga_entry(vga_color_t fg, vga_color_t bg, unsigned char c) 
{
	return (uint16_t)fg << 8 | (uint16_t)bg << 12 | (uint16_t)c;
}

// Write a char at the specified location
void vga_put_char(vga_entry_t entry, size_t row, size_t column);

void vga_clear_textbuf_color(vga_color_t color);
void vga_clear_textbuf();

#endif

