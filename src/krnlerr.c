#include "krnlerr.h"
#include "display.h"

void kernel_panic(const char *msg, struct cpu_state *state) {
    __asm__ volatile ("cli");

    uint32_t red = display_find_rgb_color(160, 0, 0);
    uint32_t blue = display_find_rgb_color(0, 0, 255);
    uint32_t white = display_find_rgb_color(255, 255, 255);
    uint32_t yellow = display_find_rgb_color(255, 255, 0);

    display_set_color(white, blue);
    display_clear();

    display_printstr("KERNEL PANIC: FATAL SYSTEM EXCEPTION\n");
    display_printstr(msg);
    display_printstr("\n\n");

    if (state) {
        display_set_color(yellow, blue);
        display_printstr("CPU Registers:\n");
        display_set_color(white, blue);

        display_printstr("RIP: ");
        display_printhex(state->rip);
        display_printstr("  RSP: ");
        display_printhex(state->rsp);
        display_printstr("\n");

        display_printstr("INT: ");
        display_printhex(state->irq_no);
        display_printstr("  ERR: ");
        display_printhex(state->err_code);
        display_printstr("\n");
    }

    display_printstr("\nSystem halted. Manual reboot required.");

    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}

void server_unexpected_exit(uint32_t server_id, const char *server_name, int exit_code) {
    uint32_t amber = display_find_rgb_color(255, 140, 0);
    uint32_t black = display_find_rgb_color(0, 0, 0);

    display_set_color(amber, black);
    display_printstr("\n[WARNING] User-space server crashed: ");
    display_printstr(server_name);
    display_printstr(" (ID: ");
    display_printhex(server_id);
    display_printstr(")\n");

    // Hand off to your microkernel supervisor to restart the server
    // supervisor_restart(server_id);
}
