bits 64
global cli
global sti
global lidt
global isr_default
global isr0
global isr3
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

%macro isr_stub 2
    push 0          ; Push dummy error code
    push %1         ; Push interrupt vector number (int_no) - e.g., 0, 3

    ; Save all general-purpose registers in order of struct cpu_state
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
    call %2         ; Call the handler function passed as second argument

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

    add rsp, 16     ; Clean up int_no and err_code off the stack
    iretq
%endmacro

%macro isr_stub_err 2
    ; Note: CPU already pushed the hardware error code automatically.
    push %1         ; Push interrupt vector number (int_no) - e.g., 8, 13, 14

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
    call %2         ; Call the handler function

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

    add rsp, 16     ; Clean up int_no and CPU-provided err_code
    iretq
%endmacro

; --- Interrupt Vector Mappings ---

isr_default:
    isr_stub 255, isr_default_handler

isr0:
    isr_stub 0, cpu_fault_isr_handler

isr3:
    isr_stub 3, cpu_fault_isr_handler

isr8:
    isr_stub_err 8, cpu_fault_isr_handler

isr13:
    isr_stub_err 13, cpu_fault_isr_handler

isr14:
    isr_stub_err 14, cpu_fault_isr_handler

irq0:
    isr_stub 32, irq0_handler

irq1:
    isr_stub 33, irq1_handler
