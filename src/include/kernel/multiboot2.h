#pragma once
#include <stdint.h>

struct mb2_tag {
    uint32_t type;
    uint32_t size;
}__attribute__((packed));

struct mb2_tag* mb2_get_tag(uint32_t info_addr, uint32_t type);
