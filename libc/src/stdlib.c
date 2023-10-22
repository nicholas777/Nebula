#include "stdlib.h"
#include "string.h"

#include <stdint.h>

int atoi_expon(int num, int exp)
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
        result += c * atoi_expon(10, strlen(str) - i - 1);
    }

    return result;
}

char* itoa(int number, char* str, int base)
{
    if (number == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    int digits = 0;
    int tmp = number;
    while (tmp != 0)
    {
        tmp /= base;
        digits++;
    }

    if (number < 0) {
        str[0] = '-';
        str++;
    }

    tmp = number < 0 ? number * (-1) : number;
    for (int i = 1; i < digits + 1; i++)
    {
        uint8_t rest = tmp % base;
        tmp /= base;
        if (base <= 10)
            str[digits - i] = rest + '0';
        else
            str[digits - i] = rest > 9 ? 'a' + (rest - 10) : '0' + rest;
    }

    str[digits] = '\0';

    if (number < 0)
        return str - 1;

    return str;
}

char* uitoa(uint32_t number, char* str, int base)
{
    if (number == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    int digits = 0;
    int tmp = number;
    while (tmp != 0)
    {
        tmp /= base;
        digits++;
    }

    for (int i = 1; i < digits + 1; i++)
    {
        uint8_t rest = number % base;
        number /= base;
        if (base <= 10)
            str[digits - i] = rest + '0';
        else
            str[digits - i] = rest > 9 ? 'a' + (rest - 10) : '0' + rest;
    }

    str[digits] = '\0';

    return str;
}

