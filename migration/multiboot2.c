#include "multiboot2.h"

struct mb2_framebuffer_tag* mb2_get_framebuffer(uint32_t info_addr) {
    // First 8 bytes of The info struct are The total_size + reserved
    uint8_t *ptr = (uint8_t *)(uintptr_t)(info_addr + 8);

    struct mb2_tag *tag = (struct mb2_tag *)ptr;

    while (tag->type != 0) { // type 0 = end tag
        if (tag->type == MB2_FRAMEBUFFER_TAG)
            return (struct mb2_framebuffer_tag *)tag;

        // Tags are 8-byte aligned
        uint32_t size = (tag->size + 7) & ~7;
        tag = (struct mb2_tag *)((uint8_t *)tag + size);
    }
    return 0;
}