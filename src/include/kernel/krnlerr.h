#pragma once
#include "display.h"

struct cpu_state {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t irq_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
};

void kernel_panic(const char *msg, struct cpu_state *state);
void server_unexpected_exit(uint32_t server_id, const char *server_name, int exit_code);
