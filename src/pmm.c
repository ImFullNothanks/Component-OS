#include "pmm.h"
#include "display.h"

static uint64_t pmm_next = 0;
static uint64_t pmm_end  = 0;

void pmm_init(uint64_t start, uint64_t end) {
    // align start to 4KB boundary
    pmm_next = (start + 0xFFF) & ~0xFFF;
    pmm_end  = end;
}

void* pmm_alloc(void) {
    if (pmm_next + 0x1000 > pmm_end) {
        display_printstr("src/pmm.c: out of memory\n");
        for(;;) __asm__ volatile ("hlt");
    }
    void *page = (void *)(uint64_t)(uintptr_t)pmm_next;
    pmm_next += 0x1000;
    // zero the page
    uint64_t *p = (uint64_t *)page;
    for (int i = 0; i < 512; i++) p[i] = 0;
    return page;
}
