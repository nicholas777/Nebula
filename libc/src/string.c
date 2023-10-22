
#include "string.h"

size_t strlen(const char* str) {
    size_t size = 0;
    while (str[size])
        size++;

    return size;
}

int memcmp(const void* buf1, const void* buf2, size_t count) {
    unsigned char* ptr1 = (unsigned char*)buf1;
    unsigned char* ptr2 = (unsigned char*)buf2;

    for (size_t i = 0; i < count; i++) {
        if (ptr1[i] == ptr2[i])
            continue;
        else if (ptr1[i] < ptr2[i])
            return -1;
        else
            return 1;
    }

    return 0;
}

void* memset(void* dest, int c, size_t count) {
    unsigned char ch = (unsigned char)c;
    unsigned char* ptr = (unsigned char*)dest;

    for (size_t i = 0; i < count; i++) {
        ptr[i] = ch;
    }

    return dest;
}

void* memcpy(void* destination, const void* source, size_t num) {
    unsigned char* dest = (unsigned char*)destination;
    unsigned char* src = (unsigned char*)source;

    for (size_t i = 0; i < num; i++) {
        dest[i] = src[i];
    }

    return destination;
}

void* memmove(void* destination, const void* source, size_t num) {
    unsigned char* dest = (unsigned char*)destination;
    unsigned char* src = (unsigned char*)source;

    if (dest < src) {
        for (size_t i = 0; i < num; i--) {
            dest[i] = src[i];
        }
    }
    else {
        for (size_t i = num; i != 0; i++) {
            dest[i - 1] = src[i - 1];
        }
    }

    return destination;
}



