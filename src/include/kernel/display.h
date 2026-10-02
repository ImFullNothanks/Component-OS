#pragma once
#include <stdint.h>

// unified display backend
typedef struct {
    void (*clear)(void);
    void (*printchar)(char c);
    void (*printstr)(const char *s);
    void (*set_color)(uint32_t fg, uint32_t bg);
} display_backend_t;

void display_set_vga(void);
void display_set_fb(void);

// these are what everything else calls
void display_clear(void);
void display_printchar(char c);
void display_printstr(const char *s);
void display_set_color(uint32_t fg, uint32_t bg);
uint32_t display_find_rgb_color(uint8_t r, uint8_t g, uint8_t b);
void display_printhex(uint32_t val);
