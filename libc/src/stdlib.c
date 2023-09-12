#include "stdlib.h"
#include "string.h"

#include <stdint.h>

int expon(int num, int exp)
{
    if (exp == 1)
        return 10;
    if (exp == 0)
        return 1;

    int result = num;
    for (int i = 1; i < exp; i++)
        result *= num;

    return result;
}

int atoi(const char* str)
{
    int result = 0;

    for (int i = strlen(str) - 1; i >= 0; i--)
    {
        int c = str[i] - '0';
        result += c * expon(10, strlen(str) - i - 1);
    }

    return result;
}

char* itoa(int number, char* str, int base)
{
    int digits = 0;
    int tmp = number;
    while (tmp > 0)
    {
        tmp /= base;
        digits++;
    }

    for (int i = 1; i < digits + 1; i++)
    {
        uint8_t rest = number % base;
        number /= base;
        str[digits - i] = rest + '0';
    }

    str[digits] = '\0';

    return str;
}
