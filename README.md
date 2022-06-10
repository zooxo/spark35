# SPARK35 - HP-35 Emulator on Arduino

See a video at: https://youtu.be/l01LcFuFtyg

```
  Welcome to SPARK35 - a HP-35 emulator on Arduino
  (Digispark on QYF-TM1638 platform)
  Version 1.0 ... (c) 2022 by deetee/zooxo
  This software is covered by the 3-clause BSD license.
  See also: https://github.com/zooxo/spark35
  ____________________
    PREAMBLE
  ____________________
  In January 1972, Hewlett-Packard introduced the HP-35 the first scientific
  calculator and revolutionized the way to calculate. Peter Monta,
  Jacques Laporte and Pietro de Luca reverse-reengineered the HP-35 to use the
  HP-35-ROM on AVR microcontroller.
  SPARK35 uses this emulator to run the HP-35 on a Digispark (ATTINY85) and the
  QYF-TM1638 development board. Despite the small flash memory (6012 bytes)
  SPARK35 adds some extra funktions like hyperbolics, present value, complex
  conversions, gamma, standard normal distribution and quadratic equation.
  Enjoy!
  deetee
  ____________________
    KEYBOARD LAYOUT
  ____________________
  F(G)[OFF]      7(1/X)[SIN] 8(SQRT)[->P] 9(/)[->R]
  EEX(SWAP)[]    4(X^Y)[COS] 5(LOG)[PV]   6(*)[ND]
  CHS(ROT)[]     1(EXP)[TAN] 2(LN)[LN!]   3(-)[QE]
  CLX(CLR)[LIT+] 0(STO)[ARC] .(RCL)[HYP]  ENTER(+)[PI]
  ____________________
    OPERATIONS
  ____________________
  . 0 1 2 3 4 5 6 7 8 9                  ... Number keys
  ENTER CHS EEX CLX CLR SWAP ROT STO RCL ... Management keys
  - * + / 1/X SQRT X^Y LOG LN EXP PI     ... Basic mathematical operations
  ARC SIN COS TAN ... Trigonometric functions
  HYP             ... Demand hyperbolic function
                      (7|4|1 -> SINH|COSH|TANH, 8|5|2 -> ASINH|ACOSH|ATANH)
  ->P ->R         ... Complex conversion (polar vs. rectangular)
  PV   ... Present value of annuity
           (ie i=0.08 rate and n=5 periods equals a PV=3.99)
  ND   ... Standard normal distribution (PDF/CDF)
  LN!  ... ln(GAMMA)
  QE   ... Quadratic equation
           (enter -5 and 6 to find the roots of y=xx-5x+6 -> 3, 2)
  F G  ... Function keys
  OFF  ... Switch off display
  LIT+ ... Raise brightness of display (7 levels)
  ____________________
    HARDWARE
  ____________________
                               __________________
     VCC=5V                   | 8.8.8.8.8.8.8.8. |
          ---------       GND-|GND               |
     VCC-| VCC  P1 |----------|DIO   *   *   *   |
         |      P2 |----------|CLK   *   *   *   |
     GND-| GND  P3 |----------|STB   *   *   *   |
          ----------      VCC-|VCC   *   *   *   |
          DIGISPARK            ------------------
                                   QYF-TM1638
  ____________________
    HP-35 DESIGN
  ____________________
     _________________________
    |                         |
    |     -1.234567890-12     |    Key codes
    |_________________________|    (default: 0xff)
    |                         |
    | X^Y  log  ln   e^X  CLR |   -  6  -  4  3  2  -  0
    | SQRT arc  sin  cos  tan |   - 46  - 44 43 42  - 40
    | 1/X  X><Y ROT  STO  RCL |   - 14  - 12 11 10  -  8
    | E-N-T-E-R CHS  EEX  CLX |   - 62  -  - 59 58  - 56
    |   -     7     8     9   |   - 54  - 52 51 50  -  -
    |   +     4     5     6   |   - 22  - 20 19 18  -  -
    |   *     1     2     3   |   - 30  - 28 27 26  -  -
    |   /     0     .     PI  |   - 38  - 36 35 34  -  -
    |_________________________|

```
