    .section .init, "ax", @progbits
    .global _start

_start:
    option push

1:  .option push
    .option norelax
    la   gp, __global_pointer$     .option pop

    la   sp, _eusrstack

    la   t0, _sidata   
    la   t1, _sdata    
    la   t2, _edata     
1:  bgeu t1, t2, 2f
    lw   a0, 0(t0)
    sw   a0, 0(t1)
    addi t0, t0, 4
    addi t1, t1, 4
    j    1b
2:
    la   t0, _sbss
    la   t1, _ebss
1:  bgeu t0, t1, 2f
    sw   zero, 0(t0)
    addi t0, t0, 4
    j    1b
2:
    la   t0, trap_handler
    csrw mtvec, t0

    call SystemInit
    call main

1:  wfi
    j    1b

    .align 2 
    .global trap_handler
trap_handler:
    addi sp, sp, -48
    sw   ra,  0(sp)
    sw   t0,  4(sp)
    sw   t1,  8(sp)
    sw   t2, 12(sp)
    sw   a0, 16(sp)
    sw   a1, 20(sp)
    sw   a2, 24(sp)
    sw   a3, 28(sp)
    sw   a4, 32(sp)
    sw   a5, 36(sp)
    csrr a0, mcause
    csrr a1, mepc
    call trap_c
    lw   ra,  0(sp)
    lw   t0,  4(sp)
    lw   t1,  8(sp)
    lw   t2, 12(sp)
    lw   a0, 16(sp)
    lw   a1, 20(sp)
    lw   a2, 24(sp)
    lw   a3, 28(sp)
    lw   a4, 32(sp)
    lw   a5, 36(sp)
    addi sp, sp, 48
    mret
