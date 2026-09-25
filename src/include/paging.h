#pragma once
#include <stdint.h>
#include <stddef.h>

#define PAGE_PRESENT  (1ULL << 0)
#define PAGE_WRITE    (1ULL << 1)
#define PAGE_USER     (1ULL << 2)
#define PAGE_NX       (1ULL << 63)  // requires EFER.NXE

void paging_remap(void);
void map_page(uint64_t *pml4, uint64_t va, uint64_t pa, uint64_t flags);
void map_range(uint64_t *pml4, uint64_t va_start, uint64_t va_end, uint64_t pa_start, uint64_t flags);
void vmm_map_page(uint64_t va, uint64_t pa, uint64_t flags);
void vmm_map_range(uint64_t va_start, uint64_t va_end, uint64_t pa_start, uint64_t flags);
