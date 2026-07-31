#include "paging.h"
#include "pmm.h"
#include "display.h"
#include <stdint.h>

// section symbols from linker script
extern uint64_t _text_start,   _text_end;
extern uint64_t _rodata_start, _rodata_end;
extern uint64_t _data_start,   _data_end;
extern uint64_t _bss_start,    _bss_end;
extern uint64_t _kernel_end;
#define KERNEL_PHYS_BASE 0x100000ULL
#define KERNEL_VIRT_BASE 0xFFFFFFFF80100000ULL
#define VIRT_TO_PHYS(v)  ((v) - KERNEL_VIRT_BASE + KERNEL_PHYS_BASE)

#define PML4_IDX(va) (((va) >> 39) & 0x1FF)
#define PDPT_IDX(va) (((va) >> 30) & 0x1FF)
#define PD_IDX(va)   (((va) >> 21) & 0x1FF)
#define PT_IDX(va)   (((va) >> 12) & 0x1FF)

#define PAGE_ALIGN_DOWN(a) ((a) & ~0xFFFULL)
#define PAGE_ALIGN_UP(a)   (((a) + 0xFFFULL) & ~0xFFFULL)

static uint64_t *get_or_create(uint64_t *table, int idx) {
    if (!(table[idx] & PAGE_PRESENT)) {
        uint64_t *t = (uint64_t *)pmm_alloc();
        table[idx] = (uint64_t)(uintptr_t)t | PAGE_PRESENT | PAGE_WRITE;
    }
    return (uint64_t *)(uintptr_t)(table[idx] & ~0xFFFULL);
}

static void map_page(uint64_t *pml4, uint64_t va, uint64_t pa, uint64_t flags) {
    uint64_t *pdpt = get_or_create(pml4, PML4_IDX(va));
    uint64_t *pd   = get_or_create(pdpt, PDPT_IDX(va));
    uint64_t *pt   = get_or_create(pd,   PD_IDX(va));
    pt[PT_IDX(va)] = (pa & ~0xFFFULL) | flags;
}

static void map_range(uint64_t *pml4, uint64_t va_start, uint64_t va_end,
                      uint64_t pa_start, uint64_t flags) {
    uint64_t va = PAGE_ALIGN_DOWN(va_start);
    uint64_t pa = PAGE_ALIGN_DOWN(pa_start);
    uint64_t end = PAGE_ALIGN_UP(va_end);
    while (va < end) {
        map_page(pml4, va, pa, flags);
        va += 0x1000;
        pa += 0x1000;
    }
}

void paging_remap(void) {
    uint64_t *new_pml4 = (uint64_t *)pmm_alloc();

    // keep low identity map for devices (VGA, BIOS, MMIO)
    map_range(new_pml4, 0, 0x100000000ULL, 0,
              PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

    uint64_t text_start   = (uint64_t)(uintptr_t)&_text_start;
    uint64_t text_end     = (uint64_t)(uintptr_t)&_text_end;
    uint64_t rodata_start = (uint64_t)(uintptr_t)&_rodata_start;
    uint64_t rodata_end   = (uint64_t)(uintptr_t)&_rodata_end;
    uint64_t data_start   = (uint64_t)(uintptr_t)&_data_start;
    uint64_t data_end     = (uint64_t)(uintptr_t)&_data_end;
    uint64_t bss_start    = (uint64_t)(uintptr_t)&_bss_start;
    uint64_t bss_end      = (uint64_t)(uintptr_t)&_bss_end;

    // identity map low memory (0 - 1MB) — BIOS data, VGA, etc.
    // RW + NX — data only, not executable
    map_range(new_pml4, 0, 0x100000, 0,
              PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

    // .text — present, NOT writable, executable (no NX)
    map_range(new_pml4, text_start, text_end, text_start,
              PAGE_PRESENT);

    // .rodata — present, NOT writable, NOT executable
    map_range(new_pml4, rodata_start, rodata_end, rodata_start,
              PAGE_PRESENT | PAGE_NX);

    // .data — present, writable, NOT executable
    map_range(new_pml4, data_start, data_end, data_start,
              PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

    // .bss + stack — present, writable, NOT executable
    map_range(new_pml4, bss_start, 0x200000, bss_start,
              PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

    // framebuffer — present, writable, NOT executable, NX
    // map entire 4GB upper region to cover any FB address
    map_range(new_pml4, 0xC0000000, 0x100000000ULL, 0xC0000000,
              PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

    display_printstr("kernel sections:\n");
    display_printstr("  .text   RO  X  ");
    display_printstr("  .rodata RO NX  ");
    display_printstr("  .data   RW NX  ");
    display_printstr("  .bss    RW NX  ");

    // flush TLB by reloading CR3
    __asm__ volatile (
        "mov %0, %%cr3"
        : : "r"((uint64_t)(uintptr_t)new_pml4)
        : "memory"
    );

    display_printstr("4KB pages + protection active\n");
}
