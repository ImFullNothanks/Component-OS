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
extern cpu_fault_isr_handler
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
    push 0          ; Push dummy error code to match error-code stack layout
    push %1         ; Push interrupt vector number (int_no)

    ; Save caller-saved and callee-saved registers in order of struct cpu_state
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp    ; Pass pointer to struct cpu_state as 1st argument
    call %1

    ; Restore registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16     ; Clean up int_no (8 bytes) and err_code (8 bytes) off the stack
    iretq
%endmacro

%macro isr_stub_err 1
    ; Note: CPU already pushed the error code automatically for this stub.
    push %1         ; Push interrupt vector number (int_no)

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp    ; Pass pointer to struct cpu_state as 1st argument
    call %1

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16     ; Clean up int_no and the CPU-provided err_code
    iretq
%endmacro


isr_default:
    isr_stub isr_default_handler

isr0:
    isr_stub isr_default_handler

isr8:
    isr_stub_err cpu_fault_isr_handler

isr13:
    isr_stub_err cpu_fault_isr_handler

isr14:
    isr_stub_err cpu_fault_isr_handler

irq0:
    isr_stub irq0_handler

irq1:
    isr_stub irq1_handler
