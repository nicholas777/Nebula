#ifndef __COMMON_H
#define __COMMON_H

__attribute__((noreturn))
void kpanic();

__attribute__((noreturn))
void kpanic_err(const char* msg);

#endif
