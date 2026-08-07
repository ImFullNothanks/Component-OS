#include "krnlshell.h"
#include "display.h"
#include "pci.h"
#include "smbios.h"
#include "string.h"  // we'll need this too

#define CMD_BUF_SIZE 256

static char buf[CMD_BUF_SIZE];
static uint64_t  buf_len = 0;

static void cmd_run(char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        display_printstr("commands: help, clear, echo, pcilist, smbiosinfo\n");
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        display_printstr(cmd + 5);
        display_printchar('\n');
    } else if (strcmp(cmd, "clear") == 0) {
        display_clear();
    } else if (strcmp(cmd, "pcilist") == 0) {
        pci_enumerate();
    } else if (strcmp(cmd, "smbiosinfo") == 0 ) {
        smbios_info();
    }  else {
        display_printstr("unknown command: ");
        display_printstr(cmd);
        display_printchar('\n');
    }
}

void cmd_init(void) {
    display_printstr("> ");
}

void cmd_process(char c) {
    if (c == '\b') {
        if (buf_len > 0) {
            buf_len--;
            // move cursor back and clear character
            display_printchar('\b');
        }
        return;
    }
    if (c == '\n') {
        display_printchar('\n');
        buf[buf_len] = '\0';
        cmd_run(buf);
        buf_len = 0;
        display_printstr("> ");
        return;
    }
    if (buf_len < CMD_BUF_SIZE - 1) {
        buf[buf_len++] = c;
        display_printchar(c);
    }
}
