#include "string.h"

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] < p2[i]) return -1;
        if (p1[i] > p2[i]) return 1;
    }
    return 0;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t strlen(const char* str) {
    const char* s = str;
    while (*s) {
        s++;
    }
    return s - str; // starting position - ending position
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    // if src is short add \0 until done
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- && *s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return n == (size_t)-1 ? 0 : *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* itoa(uint32_t value, char* str, int base) {
    char* rc;
    char* ptr;
    char* low;
    // Check for supported base
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Handle specific base 10/16 cases or general division
    do {
        unsigned int remainder = value % base;
        *ptr++ = (remainder < 10) ? (remainder + '0') : (remainder + 'a' - 10);
        value /= base;
    } while (value > 0);

    *ptr = '\0';
    low = rc;

    // Reverse the generated string
    ptr--;
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}
