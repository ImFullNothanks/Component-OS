bits 64
global cli
global sti
global lidt
global isr_default
global isr0
global isr8
global isr13
global isr14
global irq0
global irq1

extern isr_default_handler
extern irq0_handler
extern irq1_handler

cli:
    cli
    ret

sti:
    sti
    ret

lidt:
    lidt [rdi]
    ret

%macro isr_stub 1
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8
    call %1
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq
%endmacro

%macro isr_stub_err 1
    add rsp, 8      ; skip the error code CPU pushed
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8
    call %1
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq
%endmacro

isr_default:
    isr_stub isr_default_handler

isr0:
    isr_stub isr_default_handler

isr8:
    isr_stub_err isr_default_handler

isr13:
    isr_stub_err isr_default_handler

isr14:
    isr_stub_err isr_default_handler

irq0:
    isr_stub irq0_handler

irq1:
    isr_stub irq1_handler
