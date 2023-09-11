#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>
#include <stddef.h>

int memcmp(const void* buf1, const void* buf2, size_t count);
void* memcpy(void *dest, const void* src, size_t count);
void* memmove(void* dest, const void* src, size_t count);
void* memset(void* dest, int c, size_t count);

size_t strlen(const char* str);

#endif

