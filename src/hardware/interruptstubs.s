.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15HandleInterruptEhj
.global _ZN16InterruptManager15InterruptIgnoreEv

.macro ISR_STUB num
.global _ZN16InterruptManager19HandleException\num\()Ev
_ZN16InterruptManager19HandleException\num\()Ev:
    movb $\num, (active_int_id)
    jmp isr_common_entry
.endm

.macro IRQ_STUB num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (active_int_id)
    jmp isr_common_entry
.endm

ISR_STUB 0x00
ISR_STUB 0x01
ISR_STUB 0x02
ISR_STUB 0x03
ISR_STUB 0x04
ISR_STUB 0x05
ISR_STUB 0x06
ISR_STUB 0x07
ISR_STUB 0x08
ISR_STUB 0x09
ISR_STUB 0x0A
ISR_STUB 0x0B
ISR_STUB 0x0C
ISR_STUB 0x0D
ISR_STUB 0x0E
ISR_STUB 0x0F
ISR_STUB 0x10
ISR_STUB 0x11
ISR_STUB 0x12
ISR_STUB 0x13

IRQ_STUB 0x00
IRQ_STUB 0x01
IRQ_STUB 0x02
IRQ_STUB 0x03
IRQ_STUB 0x04
IRQ_STUB 0x05
IRQ_STUB 0x06
IRQ_STUB 0x07
IRQ_STUB 0x08
IRQ_STUB 0x09
IRQ_STUB 0x0A
IRQ_STUB 0x0B
IRQ_STUB 0x0C
IRQ_STUB 0x0D
IRQ_STUB 0x0E
IRQ_STUB 0x0F
IRQ_STUB 0x31

isr_common_entry:
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    pushl %esp
    movzbl (active_int_id), %eax
    pushl %eax
    call _ZN16InterruptManager15HandleInterruptEhj
    movl %eax, %esp

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp

_ZN16InterruptManager15InterruptIgnoreEv:
    iret

.section .data
active_int_id:
    .byte 0