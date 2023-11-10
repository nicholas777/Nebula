#ifndef _COMMON_H
#define _COMMON_H

__attribute__((noreturn))
void kpanic();

__attribute__((noreturn))
void kpanic_err(const char* msg);

#endif
