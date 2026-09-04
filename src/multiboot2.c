#include <stdint.h>
#include "multiboot2.h"
#include "display.h"

// Walks the tag list starting from a given tag pointer until it matches the requested type
struct mb2_tag* mb2_get_tag(uint32_t info_addr, uint32_t type) {
    // Read the total size of the multiboot structure
    uint32_t total_size = *(volatile uint32_t *)(uintptr_t)info_addr;

    // Basic sanity check on the size provided by GRUB
    if (total_size < 16 || total_size > 0x100000) {
        return 0;
    }

    uint8_t *ptr = (uint8_t *)(uintptr_t)(info_addr + 8);
    uint8_t *end = (uint8_t *)(uintptr_t)(info_addr + total_size);

    // Loop only as long as our pointer is strictly inside the valid buffer
    while (ptr < end) {
        // Ensure there's enough space left for the 8-byte tag header
        if (ptr + 8 > end) {
            break;
        }

        struct mb2_tag *tag = (struct mb2_tag *)ptr;

        // Type 0 with size 0 is the official Multiboot2 end tag
        if (tag->type == 0 && tag->size == 0) {
            break;
        }

        if (tag->type == type) {
            return tag;
        }

        // Align the tag size to the next 8-byte boundary
        uint32_t size = (tag->size + 7) & ~7;

        // Ensure the tag's declared size doesn't overflow past the end of the block
        if (size < 8 || ptr + size > end) {
            break;
        }

        ptr += size;
    }

    return 0; // Tag not found
}
