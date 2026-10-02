#pragma once
#include <stdint.h>

void pmm_init(uint64_t start, uint64_t end);
void* pmm_alloc(void);
