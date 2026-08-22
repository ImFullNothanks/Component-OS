#pragma once
#include <stdint.h>
#include "pic.h"
#include "io.h"
#include "display.h"

void pit_init(uint32_t frequency);
void pit_handler(void);
uint32_t get_pit_tick_count(void);
uint32_t pit_kernel_uptime_seconds(void);
