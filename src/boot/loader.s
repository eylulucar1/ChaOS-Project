.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1 | 1<<2)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0
.long 0
.long 0
.long 0
.long 0
.long 0
.long 1024
.long 768
.long 32

.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
    cli
    mov $kernel_stack, %esp
    push %eax
    push %ebx
    call callConstructors
    pop %ebx
    pop %eax
    push %eax
    push %ebx
    call kernelMain

_stop:
    cli
    hlt
    jmp _stop

.section .bss
.align 16
.space 2*1024*1024
kernel_stack: