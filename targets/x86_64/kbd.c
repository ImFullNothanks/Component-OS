#include <kernel/kbd.h>
#include <kernel/krnlshell.h>

void kbd_handler(uint8_t scancode) {
    if (scancode & 0x80) return;  // ignore key release
    if (scancode < sizeof(scancode_table)) {
        char c = scancode_table[scancode];
        if (c) {
            cmd_process(c);
        }
    }
}

