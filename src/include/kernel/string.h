#pragma once

#include <stdint.h>
#include <stddef.h>

int memcmp(const void* s1, const void* s2, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char* str);
char* strncpy(char* dest, const char* src, size_t n);
char* itoa(uint32_t value, char* str, int base);
