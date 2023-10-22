#ifndef __STDLIB_H
#define __STDLIB_H

#include <stdint.h>
#include <stddef.h>

#define NULL 0

int atoi(const char* str);

char* itoa(int number, char* str, int base);
char* uitoa(uint32_t number, char* str, int base);

#endif
