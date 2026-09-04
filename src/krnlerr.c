#include "krnlerr.h"
#include "display.h"

void kernel_panic(const char *msg, struct cpu_state *state) {
    display_set_color(display_find_rgb_color(0,0,255),display_find_rgb_color(256,250,250));

    display_printstr("KERNEL PANIC: FATAL SYSTEM EXCEPTION\n");
    display_printstr(msg);
    display_printchar('\n');

    if (state) {
        // Format register states into a temporary string buffer and print
        display_printstr("CPU Exception Details:\n");
        // We'll later parse the data
    }

    display_printchar('\n');

    display_printstr("System halted. Please restart your machine.");

    // Hang kernel loop
    for (;;) {
        asm volatile ("cli");
        asm volatile ("hlt");
    }
}

void server_unexpected_exit(uint32_t server_id, const char *server_name, int exit_code) {
    // Fill screen or draw an amber warning banner (RGB: 0xFF8C00)
    // In a microkernel, instead of a full kernel panic, you log the failure,
    // isolate the crashed user-space server, and trigger the supervisor to restart it.

    // Log warning info or draw warning overlay on screen
    display_printstr("WARNING: User-Space Server Terminated Unexpectedly");

    // Trigger supervisor recovery logic
    // supervisor_restart_server(server_id);
}
