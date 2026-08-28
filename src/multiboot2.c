#include <stdint.h>
#include "multiboot2.h"

// Walks the tag list starting from a given tag pointer until it matches the requested type
struct mb2_tag* mb2_get_tag(uint32_t info_addr, uint32_t type) {
    // The first 4 bytes hold the total size of the multiboot structure
    uint32_t total_size = *(uint32_t *)(uintptr_t)info_addr;

    // Tags start at offset 8 (after total_size and reserved)
    uint8_t *current_ptr = (uint8_t *)(uintptr_t)(info_addr + 8);
    uint32_t bytes_checked = 8;

    while (bytes_checked < total_size) {
        struct mb2_tag *tag = (struct mb2_tag *)current_ptr;

        // If we hit a type 0 end tag or a zeroed-out header, we can stop safely
        if (tag->type == 0 && tag->size == 0) {
            break;
        }

        if (tag->type == type) {
            return tag;
        }

        // Align tag size to the next 8-byte boundary
        uint32_t size = (tag->size + 7) & ~7;

        // Prevent infinite loops from bad sizes
        if (size == 0 || bytes_checked + size > total_size) {
            break;
        }

        current_ptr += size;
        bytes_checked += size;
    }

    return 0; // Tag not found or reached end safely
}
