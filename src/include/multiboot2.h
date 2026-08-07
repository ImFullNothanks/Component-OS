#pragma once
#include <stdint.h>

#define MB2_FRAMEBUFFER_TAG 8

struct mb2_tag {
    uint32_t type;
    uint32_t size;
}__attribute__((packed));

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

struct mb2_framebuffer_tag* mb2_get_framebuffer(uint32_t info_addr);