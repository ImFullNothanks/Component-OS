#include <kernel/multiboot2.h>
#include <x86_64/mb2_mmap.h>

struct mb2_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} __attribute__((packed));

struct mb2_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct mb2_mmap_entry entries[];
} __attribute__((packed));

uint64_t get_total_memory(uint32_t info_addr) {
    // Type 6 is the Multiboot2 memory map tag
    struct mb2_tag_mmap *mmap_tag = (struct mb2_tag_mmap *)mb2_get_tag(info_addr, 6);
    if (!mmap_tag) {
        return 0; // No memory map found!
    }

    uint64_t total_ram = 0;

    // Calculate how many entries exist based on total size and individual entry size
    int entry_count = (mmap_tag->size - sizeof(struct mb2_tag_mmap)) / mmap_tag->entry_size;

    for (int i = 0; i < entry_count; i++) {
        struct mb2_mmap_entry *entry = &mmap_tag->entries[i];

        // Type 1 signifies usable RAM regions
        if (entry->type == 1) {
            total_ram += entry->len;
        }
    }

    return total_ram; // Total bytes of usable system memory
}
