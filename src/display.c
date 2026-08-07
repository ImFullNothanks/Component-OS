#include "display.h"
#include "print.h"
#include "fb.h"

static display_backend_t backend;

// --- VGA wrappers ---
static void vga_clear_wrap(void)              { print_clear(); }
static void vga_putchar_wrap(char c)          { print_char(c); }
static void vga_puts_wrap(const char *s)      { print_str(s); }
static void vga_setcolor_wrap(uint32_t fg, uint32_t bg) {
    // convert 32-bit rgb back to vga color index — just use low nibble
    print_set_color((uint8_t)fg, (uint8_t)bg);
}

// --- FB wrappers ---
static uint32_t fb_fg = 0xFFFFFF;
static uint32_t fb_bg = 0x000000;

static void fb_clear_wrap(void)              { fb_clear(fb_bg); }
static void fb_putchar_wrap(char c)          { fb_putstr((char[]){c,0}, fb_fg, fb_bg); }
static void fb_puts_wrap(const char *s)      { fb_putstr(s, fb_fg, fb_bg); }
static void fb_setcolor_wrap(uint32_t fg, uint32_t bg) {
    fb_fg = fg; fb_bg = bg;
}

// --- public init ---
void display_set_vga(void) {
    backend.clear     = vga_clear_wrap;
    backend.printchar   = vga_putchar_wrap;
    backend.printstr      = vga_puts_wrap;
    backend.set_color = vga_setcolor_wrap;
}

void display_set_fb(void) {
    backend.clear     = fb_clear_wrap;
    backend.printchar   = fb_putchar_wrap;
    backend.printstr      = fb_puts_wrap;
    backend.set_color = fb_setcolor_wrap;
}

// --- unified API ---
void display_clear(void)             { backend.clear(); }
void display_printchar(char c)         { backend.putchar(c); }
void display_printstr(const char *s)     { backend.puts(s); }
void display_set_color(uint32_t fg, uint32_t bg) { backend.set_color(fg, bg); }
void display_printhex(uint32_t val) {
    char hex[] = "0x00000000";
    char digits[] = "0123456789ABCDEF";
    for (int i = 9; i >= 2; i--) {
        hex[i] = digits[val & 0xF];
        val >>= 4;
    }
    display_printstr(hex);
}
