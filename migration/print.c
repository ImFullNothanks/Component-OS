#include "print.h"

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char* scrbuf = VGA_MMIO;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

void clear_row(size_t row) {
    struct Char empty = (struct Char) {
        character: ' ',
        color: color,
    };
    for (size_t col = 0; col < VGA_COLS; col++) {
        scrbuf[col + VGA_COLS * row] = empty;
    }
}

void print_clear() {
    for (size_t i = 0; i < VGA_ROWS; i++) {
        clear_row(i);
    }
}

void print_newline() {
    col = 0;

    if (row < VGA_ROWS - 1) {
        row++;
        return;
    }

    for (size_t row = 1; row < VGA_ROWS; row++) {
        for (size_t col = 0; col < VGA_COLS; col++) {
            struct Char character = scrbuf[col + VGA_COLS * row];
            scrbuf[col + VGA_COLS * (row - 1)] = character;
        }
    }

    clear_row(VGA_ROWS - 1);
}

void print_char(char character) {
    if (character == '\b') {
        if (col > 0) {
            col--;
            scrbuf[col + VGA_COLS * row] = (struct Char){
                character: ' ',
                color: color,
            };
        }
        return;
    }

    if (character == '\n') {
        print_newline();
        return;
    }
    if (col > VGA_COLS) {
        print_newline();
    }
    scrbuf[col + VGA_COLS * row] = (struct Char) {
        character: (uint8_t) character,
        color: color,
    };

    col++;
}

void print_str(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        char character = (uint8_t) str[i];

        if (character == '\0') {
            return;
        }

        print_char(character);
    }
}

void print_set_color(uint8_t foreground, uint8_t background) {
    color = foreground + (background << 4);
}

void print_hex(uint32_t val) {
    char hex[] = "0x00000000";
    char digits[] = "0123456789ABCDEF";
    for (int i = 9; i >= 2; i--) {
        hex[i] = digits[val & 0xF];
        val >>= 4;
    }
    print_str(hex);
}
