
.set noat
.set noreorder

#include "macros.inc"

.section .text, "ax"

glabel entry_point
.if VERSION_CN == 1
    lui   $t0, %lo(_mainSegmentNoloadStartHi)
    ori   $t0, %lo(_mainSegmentNoloadStartLo)
    lui   $t1, %lo(_mainSegmentNoloadSizeHi)
    ori   $t1, %lo(_mainSegmentNoloadSizeLo)
.L80249010:
    sw    $zero, ($t0)
    sw    $zero, 4($t0)
    addi  $t0, $t0, 8
    addi  $t1, $t1, -8
    bnez  $t1, .L80249010
     nop
    lui   $sp, %lo(gIdleThreadStackHi)
    ori   $sp, %lo(gIdleThreadStackLo)
    lui   $t2, %lo(main_funcHi)
    ori   $t2, %lo(main_funcLo)
    jr    $t2
     nop
.else
    lui   $t0, %hi(_mainSegmentNoloadStart)
    lui   $t1, %lo(_mainSegmentNoloadSizeHi)
    addiu $t0, %lo(_mainSegmentNoloadStart)
    ori   $t1, %lo(_mainSegmentNoloadSizeLo)
.L80246010:
    addi  $t1, $t1, -8
    sw    $zero, ($t0)
    sw    $zero, 4($t0)
    bnez  $t1, .L80246010
     addi  $t0, $t0, 8
    lui   $t2, %hi(main_func)
    lui   $sp, %hi(gIdleThreadStack)
    addiu $t2, %lo(main_func)
    jr    $t2
     addiu $sp, %lo(gIdleThreadStack)
.endif
    nop
    nop
    nop
    nop
    nop
    nop
