#pragma once
#include <stdint.h>

void fb_init(uint64_t addr, uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp);
void fb_putpixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t fb_rgb(uint8_t r, uint8_t g, uint8_t b);
void fb_clear(uint32_t color);
void fb_putchar(char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);
void fb_putstr(const char *s, uint32_t fg, uint32_t bg);
