#pragma once
#include <stdint.h>

// unified display backend
typedef struct {
    void (*clear)(void);
    void (*putchar)(char c);
    void (*puts)(const char *s);
    void (*set_color)(uint32_t fg, uint32_t bg);
} display_backend_t;

void display_init_vga(void);
void display_init_fb(uint64_t addr, uint32_t w, uint32_t h,
                     uint32_t pitch, uint8_t bpp);

// these are what everything else calls
void display_clear(void);
void display_printchar(char c);
void display_printstr(const char *s);
void display_set_color(uint32_t fg, uint32_t bg);
void display_printhex(uint32_t val);
