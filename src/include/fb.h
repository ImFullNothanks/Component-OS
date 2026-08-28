#pragma once
#include <stdint.h>
#include "multiboot2.h"

struct mb2_framebuffer_tag {
    struct mb2_tag tag;
    uint64_t address;
    uint32_t pitch; // Bytes Per scanline
    uint32_t width;
    uint32_t height;
    uint8_t bpp; // Bit Per Pixel, Color Depth
    uint8_t type; // Framebuffer Type
    uint16_t reserved;
}__attribute__((packed));

void fb_init(uint64_t addr, uint32_t width, uint32_t height,uint32_t pitch, uint8_t bpp);
void fb_putpixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t fb_rgb(uint8_t r, uint8_t g, uint8_t b);
void fb_clear(uint32_t color);
void fb_putchar(char c, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);
void fb_putstr(const char *s, uint32_t fg, uint32_t bg);
