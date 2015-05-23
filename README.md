# fpudump
A simple, header only library to dump x87 FPU state in an OllyDbg style format. I wrote this to figure out what was going wrong with a program. Turns out there was a [bug in newlib](http://thread.gmane.org/gmane.comp.lib.newlib/3523/).

Example:

    #include "fpu_dump.h"
    ...
    double x = 243;
    FPUDUMP;
    x = x / 3.14;
    FPUDUMP;

Example output:

    ================== MAGICAL FPU INSPECTOR ==================
    From fpu_dump.c line 20:

    Status: 0x0020
    Invalid: 0, Denorm: 0, Zero Div: 0
    Overflow: 0, Underflow: 0, Precision: 1
    Stack fault: 0, Error summary: 0, TOP: 0
    C3: 0, C2: 0, C1: 0, C0: 0

    Control: 0x037F
    Invalid Mask: 1, Denorm Mask: 1, Zero Div Mask: 1
    Overflow Mask: 1, Underflow Mask: 1, Precision Mask: 1
    Rounding: NEAR, Precision: 64, Infinity: 0

    3 2 1 0      E S P U O Z D I
    FST 0020  Cond 0 0 0 0  Err 0 0 1 0 0 0 0 0  (GT)
    FCW 037F  Prec NEAR,64  Mask    1 1 1 1 1 1

    ST0  EMPTY                        NAN   REG0
    FFFF 00000000 00000000

    ST1  EMPTY                          0   REG1
    0000 00000000 00000000

    ST2  EMPTY                          0   REG2
    0000 00000000 00000000

    ST3  EMPTY                          0   REG3
    0000 00000000 00000000

    ST4  EMPTY                          0   REG4
    0000 00000000 00000000

    ST5  EMPTY                          0   REG5
    0000 00000000 00000000

    ST6  EMPTY       16045690984229361664   REG6
    403E DEADBEEF BAADD800

    ST7  EMPTY       16045690984229355520   REG7
    403E DEADBEEF BAADC0DF

    ========================= GOODBYE =========================
