    .global __set_thread_area
    .type   __set_thread_area, %function
    .align 1
__set_thread_area:
.Ltmp1:
    C.BSTART.AUX
    addi    a0, 0, ->t
    ssrset  t#1, 0
.Ltmp2:
    C.BSTART RET
    addi    zero, 0, ->t
    addi    t#1, 0, ->a0
    addi    ra, 0, ->t
    setc.tgt t#1
    C.BSTOP
