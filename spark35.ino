/*
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

*/


//--v----1----v----2----v----3----v----4----v----5----v----6----v----7----v----8
// S Y S T E M

// PINS
#define DATA   1
#define CLOCK  2
#define STROBE 3

// CONSTANTS
#define PRINTCMD 0x40 // Command code for display printing (QYF-TM1638-board)
#define READCMD  0x42 // Command code for key reading(QYF-TM1638-board)

#define DIGITS   8 // Number of digits
#define SEGMENTS 8 // Number of segments per digit
#define KEYS    16 // Number of keys
#define _END  0xff // End key

// KEY VALUES
#define _KF 1     //  key number:   getkey():     symbol:
#define _K7 3     //   1  2  3  4    1  3  5  7   F  7  8  9
#define _K8 5     //   5  6  7  8    9 11 13 15   E  4  5  6
#define _K9 7     //   9 10 11 12   16  2  4  6   N  1  2  3
#define _KE 9     //  13 14 15 16    8 10 12 14   X  0  D  P
#define _K4 11
#define _K5 13
#define _K6 15
#define _KN 16
#define _K1 2
#define _K2 4
#define _K3 6
#define _KX 8
#define _K0 10
#define _KD 12
#define _KP 14

// SIGN
#define _70 0x3f //  --a--    hgfe-dcba 8421-8421
#define _71 0x06 // |     |
#define _72 0x5b // f     b
#define _73 0x4f // |--g--|
#define _74 0x66 // e     c
#define _75 0x6d // |     |
#define _76 0x7d //  --d--  (h)
#define _77 0x07
#define _78 0x7f
#define _79 0x6f
#define _7Q 0x3b
#define _7A 0x77
#define _7b 0x7c
#define _7C 0x39
#define _7c 0x58
#define _7d 0x5e
#define _7E 0x79
#define _7F 0x71
#define _7G 0x3d
#define _7h 0x74
#define _7H 0x76
#define _7i 0x04
#define _7I 0x30
#define _7J 0x0e
#define _7j 0x0d
#define _7K 0x75
#define _7L 0x38
#define _7N 0x37
#define _7n 0x54
#define _7M 0x15
#define _7o 0x5c
#define _7P 0x73
#define _7Q 0x3b
#define _7q 0x67
#define _7R 0x31
#define _7r 0x50
#define _7S 0x6d
#define _7t 0x78
#define _7u 0x1c
#define _7U 0x3e
#define _7V 0x72
#define _7W 0x2a
#define _7Y 0x6e
#define _7ABSOLUTE 0x36
#define _7APOSTROPHELEFT 0x20
#define _7APOSTROPHERIGHT 0x02
#define _7AT 0x5f
#define _7COMMALEFT 0x10
#define _7COMMARIGHT 0x04
#define _7Cabs 0x14
#define _7Cre 0x21 // Imaginary indicators
#define _7Cim 0x02
#define _7Cangle 0x18
#define _7DEGREE 0x63
#define _7DOT 0x80
#define _7DOWN 0x0c
#define _7ENTER 0x42
#define _7EQUAL 0x48
#define _7GAMMA 0x31
#define _7GRAD 0x63
#define _7GREATERTHAN 0x0c
#define _7HIGHEQUALSIGN 0x41
#define _7HIGHLINE 0x01
#define _7LEFTBRACKET 0x39
#define _7LESSTHAN 0x18
#define _7LOWLINE 0x08
#define _7LOWHIGHLINE 0x09
#define _7MATA 0x22
#define _7MATB 0x14
#define _7MINUS 0x40
#define _7QUESTIONMARK 0x53
#define _7QUOTATIONMARK 0x22
#define _7PARALLEL 0x36
#define _7RECORD 0x6b
#define _7REVERSESOLIDUS 0x64
#define _7RIGHTBRACKET 0x0f
#define _7SOLIDUS 0x52
#define _7SPACE 0x00
#define _7SQRT 0x33
#define _7STATISTIC 0x5d
#define _7SWAP 0x1b
#define _7TEN 0x6b
#define _7THREELINES 0x49
#define _7UP 0x03

static const byte digit[] = {_70, _71, _72, _73, _74, _75, _76, _77, _78, _79}; // Digits

// GLOBAL VARIABLES
static byte dbuf[DIGITS]; // Display buffer
static byte brightness = 0x02; // Brightness (0...7)(incremented at start - LIT+)
static byte key = _KX, oldkey = _KX; // Pressed and previous key (start with LIT+)
static byte fg = 2; // Function key indicator (start with LIT+)
static boolean darkscreen = false; // Screensaver

static int memp, count = 0; // Intrisic functions
static boolean isrun = false;
static boolean islock = false;

// MESSAGES
#define MSGERROR 1
#define MSGRUN 2
#define MSGHYP 3
const byte msg[] PROGMEM = { // Messages (6 digits)
  0, _7E, _7r, _7r, _7o, _7r, 0, 0, // Error
  0, _7r, _7u, _7n, 0, 0, 0, 0, // run
  0, _7H, _7Y, _7P, _7DOT, _7QUESTIONMARK, 0, 0 // HYP?
};

// HP-35 Key codes
#define _CLR 0
#define _EXP 2
#define _LN 3
#define _LOG 4
#define _POW 6
#define _RCL 8
#define _STO 10
#define _ROT 11
#define _SWAP 12
#define _INV 14
#define _6 18
#define _5 19
#define _4 20
#define _ADD 22
#define _3 26
#define _2 27
#define _1 28
#define _MULT 30
#define _PI 34
#define _DOT 35
#define _0 36
#define _DIV 38
#define _TAN 40
#define _COS 42
#define _SIN 43
#define _ARC 44
#define _SQRT 46
#define _9 50
#define _8 51
#define _7 52
#define _SUB 54
#define _CLX 56
#define _EEX 58
#define _CHS 59
#define _ENTER 62

// Key codes for intrinsic functions
#define _HYP 64 // HYP
#define _LIT 65 // LIT+
#define _FR2P _LIT+1 // R2P
#define _FP2R _LIT+2 // P2R
#define _FPV _LIT+3 // PV
#define _FND _LIT+4 // ND
#define _FGAMMA _LIT+5 // GAMMA
#define _FQE _LIT+6 // QE
#define _FSINH _LIT+7 // SINH
#define _FCOSH _LIT+8 // COSH
#define _FTANH _LIT+9 // TANH
#define _FASINH _LIT+10 // ASINH
#define _FACOSH _LIT+11 // ACOSH
#define _FATANH _LIT+12 // ATANH

// Function address in mem
#define _R2P 0
#define _P2R 18
#define _PV 35
#define _ND 53
#define _GAMMA 87
#define _QE 120
#define _SINH 145
#define _COSH 153
#define _TANH 160
#define _ASINH 173
#define _ACOSH 182
#define _ATANH 191
const byte mem[] PROGMEM = { // Intrinsic functions
  _ENTER, _MULT, _SWAP, _ENTER, _ENTER, _ROT, _MULT, _ADD, _SQRT, _ENTER,
  _ROT, _DIV, _ARC, _SIN, _ROT, _ROT, _ROT,
  _END, //0 R->P r=sqrt(x*x+y*y) a=atan(y/x)

  _SWAP, _SIN, _ENTER, _ENTER, _MULT, _CHS, _ENTER, _1, _ADD, _SQRT,
  _ROT, _MULT, _ROT, _MULT, _SWAP, _ROT,
  _END, //18 P->R y=r*sin(a) x=r*cos(a)

  _SWAP, _ENTER, _ENTER, _1, _ADD, _ROT, _ROT, _ROT, _SWAP, _POW,
  _INV, _CHS, _ENTER, _1, _ADD, _SWAP, _DIV,
  _END, //35 PV(i,n)=(1-1/(1+i)^n)/i

  _ENTER, _ENTER, _ENTER, _MULT, _MULT, _DOT, _0, _7, _MULT, _CHS,
  _SWAP, _1, _DOT, _6, _MULT, _SUB,
  _EXP, _1, _ADD, _INV, // CDF~1/(1+exp(-0.07*x^3-1.6*x))
  _SWAP, _ENTER, _MULT, _CHS,   _2, _DIV, _EXP,
  _PI, _2, _MULT, _SQRT, _INV, _MULT,
  _END, //53 ND PDF=exp(-x*x/2)/sqrt(2*PI)

  _ENTER, _1, _ADD, _ENTER, _ENTER, _ENTER, _1, _2, _MULT,
  _SWAP, _1, _0, _MULT, _INV, _SUB, _INV, _ADD, _LN, _1, _SUB, _MULT,
  _SWAP, _LN, _CHS, _PI, _2, _MULT, _LN, _ADD, _2, _DIV, _ADD,
  _END, //87 LN!=(ln(2*PI)-ln(x))/2 + x*(ln(x+1/(12*x-1/10/x))-1)

  _SWAP, _2, _DIV, _CHS, _ENTER, _ENTER, _MULT, _ROT, _ROT, _ROT, _SUB, _SQRT,
  _ENTER, _ENTER, _CLX, _ADD, _CHS, _ROT, _ADD, _ROT, _ADD, _ROT, _ROT, _ROT,
  _END, //120 QE x12=-p/2+-sqrt(p*p/4-q)

  _EXP, _ENTER, _INV, _CHS, _ADD, _2, _DIV,
  _END, //145 SINH=(exp(x)-exp(-x))/2
  _EXP, _ENTER, _INV, _ADD, _2, _DIV,
  _END, //153 COSH=(exp(x)+exp(-x))/2
  _ENTER, _2, _MULT, _EXP, _ENTER, _ENTER, _1, _SUB, _SWAP, _1, _ADD, _DIV,
  _END, //160 TANH=(exp(2*x)-1)/(exp(2*x)+1)

  _ENTER, _ENTER, _MULT, _1, _ADD, _SQRT, _ADD, _LN,
  _END, //173 ASINH=ln(x+sqrt(x*x+1))
  _ENTER, _ENTER, _MULT, _1, _SUB, _SQRT, _ADD, _LN,
  _END, //182 ACOSH=ln(x+sqrt(x*x-1))
  _ENTER, _ENTER, _1, _ADD, _SWAP, _CHS, _ENTER, _1, _ADD, _DIV, _SQRT, _LN,
  _END, //191 ATANH=ln(sqrt((1+x)/(1-x)))
};


//--v----1----v----2----v----3----v----4----v----5----v----6----v----7----v----8
// S U B R O U T I N E S

static void command(byte val) { // Send command to board
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, val);
  digitalWrite(STROBE, HIGH);
}

static unsigned int getbuttons(void) { // Get button code (one at a time) - 16 bits
  uint16_t buttons = 0;
  byte val = 0;
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, READCMD); // Demand key input
  pinMode(DATA, INPUT); // Switch data line to INPUT
  for (byte i = 0; i < 4; i++) {
    val = shiftIn(DATA, CLOCK, LSBFIRST) >> 1; // Get button value (and half it)
    if (val > 2) val >>= 2; // Reduce values 16 and 32 to 4 and 8
    buttons |= val << (4 * i); // Write value-nibble to buttons-nibbles (4,3,2,1)
  }
  pinMode(DATA, OUTPUT); // Switch data line back to output
  digitalWrite(STROBE, HIGH);
  return (buttons);
}

static byte getkey(void) { // Returns key
  unsigned int c = getbuttons();
  byte ans = 0;
  if (c & 0x0001) return (_KN);
  while (c >>= 1) ans++;
  return (ans);
}

static void printbuf(void) { // Write buffer to display
  if (fg == 1 || fg == 2) dbuf[0] = (fg == 1) ? _7HIGHLINE : _7LOWLINE; // F-key indicator
  command(PRINTCMD); // Demand write command
  for (byte j = 0, dtmp = 0; j < SEGMENTS; j++) { // Write segment of all digits
    for (byte i = 0; i < DIGITS; i++)
      dtmp = (dtmp << 1) | ((dbuf[i] & (0x01 << j)) ? 0x01 : 0x00);
    for (byte i = 0; i < DIGITS; i++) { // Print segments for each digit
      digitalWrite(STROBE, LOW);
      shiftOut(DATA, CLOCK, LSBFIRST, 0xc0 | 2 * j); // Address - segment
      shiftOut(DATA, CLOCK, LSBFIRST, 0x00 | dtmp); // Data - digit
      digitalWrite(STROBE, HIGH);
    }
  }
}


//--v----1----v----2----v----3----v----4----v----5----v----6----v----7----v----8
// H P - 3 5 - E N G I N E  (Peter Monta, Jacques Laporte, Pietro de Luca)

#define WSIZE 14 // Display size

const byte rom[] PROGMEM = { // HP35-ROM, 768 bytes
  0, 221, 2, 255, 2, 36, 0, 23, 1, 68, 2, 68, 0, 132, 1, 16, 2, 209, 3, 251, 0,
  95, 0, 195, 1, 168, 3, 103, 2, 238, 3, 226, 0, 46, 0, 144, 3, 234, 3, 234, 3,
  234, 0, 107, 2, 105, 0, 168, 2, 168, 0, 255, 3, 234, 3, 234, 3, 234, 0, 48, 0,
  204, 0, 170, 1, 168, 0, 67, 1, 211, 0, 204, 0, 48, 0, 0, 0, 131, 1, 68, 0, 68,
  0, 187, 2, 68, 0, 159, 2, 132, 3, 11, 0, 46, 0, 144, 3, 40, 3, 111, 3, 234, 3,
  234, 3, 234, 0, 75, 2, 103, 3, 168, 1, 113, 3, 119, 3, 203, 2, 206, 0, 196, 1,
  219, 1, 40, 0, 52, 2, 206, 3, 117, 1, 46, 2, 250, 1, 22, 3, 106, 3, 131, 1,
  186, 3, 155, 3, 54, 3, 76, 3, 155, 0, 28, 1, 234, 0, 2, 1, 51, 2, 196, 2, 214,
  3, 166, 1, 20, 2, 31, 1, 125, 3, 119, 0, 210, 1, 114, 0, 218, 3, 138, 1, 119,
  0, 206, 0, 52, 1, 142, 3, 12, 1, 42, 1, 138, 1, 186, 1, 163, 0, 170, 1, 122,
  1, 95, 1, 76, 3, 170, 1, 20, 1, 11, 3, 42, 0, 42, 3, 221, 1, 10, 2, 206, 3,
  44, 2, 39, 3, 178, 1, 235, 2, 209, 0, 144, 1, 20, 3, 219, 3, 178, 0, 250, 1,
  142, 1, 186, 1, 255, 0, 218, 0, 170, 3, 76, 1, 22, 1, 106, 2, 126, 1, 59, 2,
  118, 2, 3, 0, 202, 3, 221, 2, 214, 1, 158, 3, 44, 2, 79, 0, 142, 1, 238, 0,
  76, 1, 18, 0, 60, 1, 162, 2, 63, 3, 174, 0, 236, 3, 231, 0, 202, 1, 132, 1,
  235, 0, 254, 1, 168, 0, 46, 3, 250, 3, 250, 1, 250, 1, 250, 0, 74, 2, 143, 3,
  174, 3, 166, 1, 166, 2, 159, 3, 174, 2, 38, 0, 74, 2, 251, 2, 142, 3, 234, 0,
  14, 2, 251, 2, 163, 2, 246, 0, 212, 2, 211, 3, 126, 0, 254, 1, 212, 2, 223, 1,
  40, 1, 196, 0, 206, 1, 110, 0, 190, 1, 254, 2, 46, 0, 48, 0, 144, 1, 113, 1,
  68, 3, 119, 2, 206, 1, 158, 2, 36, 3, 63, 1, 250, 2, 4, 1, 84, 3, 55, 1, 234,
  3, 27, 0, 40, 0, 20, 3, 31, 0, 36, 0, 28, 3, 44, 3, 67, 2, 40, 2, 20, 3, 51,
  1, 14, 1, 100, 0, 208, 1, 40, 3, 174, 1, 117, 1, 196, 3, 221, 2, 189, 2, 43,
  2, 214, 0, 28, 0, 172, 1, 23, 3, 12, 2, 238, 2, 246, 3, 226, 3, 226, 0, 140,
  0, 60, 3, 98, 3, 191, 0, 2, 3, 171, 3, 226, 3, 46, 0, 48, 1, 4, 2, 212, 0,
  115, 1, 191, 0, 254, 2, 164, 3, 15, 1, 148, 3, 243, 0, 28, 2, 146, 1, 233, 2,
  168, 3, 111, 3, 207, 3, 46, 0, 161, 1, 168, 0, 161, 1, 168, 2, 84, 0, 39, 3,
  174, 1, 84, 0, 75, 0, 222, 2, 153, 1, 40, 2, 149, 2, 97, 0, 149, 1, 168, 2,
  153, 2, 148, 3, 107, 2, 238, 3, 226, 1, 38, 3, 166, 1, 106, 2, 146, 1, 186, 0,
  103, 2, 210, 1, 234, 0, 119, 2, 206, 2, 142, 1, 40, 2, 46, 1, 7, 2, 46, 1, 12,
  3, 123, 1, 40, 3, 174, 1, 162, 0, 183, 0, 174, 1, 142, 0, 138, 3, 47, 1, 142,
  0, 84, 0, 151, 2, 148, 1, 183, 1, 84, 0, 87, 0, 254, 3, 190, 0, 55, 2, 146, 3,
  126, 0, 235, 1, 254, 3, 50, 1, 210, 3, 46, 1, 46, 3, 82, 0, 239, 1, 168, 2,
  206, 3, 178, 3, 46, 1, 18, 1, 40, 3, 254, 3, 254, 0, 143, 0, 206, 0, 42, 2,
  214, 2, 201, 1, 98, 1, 168, 3, 174, 1, 12, 2, 145, 1, 140, 2, 109, 2, 12, 2,
  109, 0, 140, 2, 24, 2, 140, 2, 109, 2, 57, 2, 109, 3, 49, 1, 14, 2, 109, 0,
  142, 3, 45, 3, 49, 2, 174, 2, 153, 2, 84, 1, 179, 0, 254, 2, 97, 0, 100, 0,
  206, 1, 98, 1, 234, 0, 84, 2, 151, 2, 153, 3, 49, 2, 174, 2, 149, 3, 49, 2,
  174, 2, 174, 2, 85, 2, 174, 3, 173, 3, 49, 2, 140, 2, 113, 2, 57, 2, 12, 2,
  117, 0, 140, 2, 24, 1, 140, 2, 113, 1, 12, 2, 113, 2, 113, 3, 46, 2, 78, 3,
  76, 1, 88, 3, 239, 1, 140, 2, 24, 1, 152, 1, 88, 0, 152, 1, 24, 2, 88, 0, 84,
  3, 107, 0, 48, 2, 238, 3, 226, 0, 16, 1, 16, 1, 14, 2, 150, 2, 46, 2, 135, 1,
  254, 3, 14, 2, 131, 3, 142, 1, 16, 1, 16, 1, 74, 1, 16, 1, 226, 3, 78, 2, 163,
  3, 206, 1, 14, 0, 28, 2, 82, 0, 44, 2, 167, 0, 183, 1, 226, 3, 22, 2, 203, 3,
  150, 1, 22, 0, 28, 0, 44, 2, 207, 0, 183, 0, 28, 3, 150, 3, 111, 0, 16, 1,
  122, 1, 122, 2, 234, 3, 94, 2, 126, 3, 27, 1, 16, 2, 6, 3, 43, 0, 254, 3, 46,
  3, 14, 1, 16, 0, 206, 2, 204, 1, 216, 2, 24, 1, 88, 0, 216, 2, 88, 2, 24, 0,
  88, 1, 152, 0, 216, 1, 88, 3, 12, 0, 48, 0, 16, 3, 138, 3, 123, 1, 98, 1, 254,
  0, 44, 2, 239, 3, 170, 2, 234, 0, 98, 3, 155, 2, 206, 2, 78, 2, 42, 0, 202, 3,
  12, 2, 187, 1, 16, 2, 146, 2, 146, 1, 126, 3, 179, 1, 210, 3, 18, 2, 50, 0,
  142, 3, 126, 3, 187, 3, 178, 1, 168, 0, 30, 0, 7, 1, 14, 3, 178, 1, 40, 2,
  146, 1, 126, 2, 62, 0, 16, 3, 62, 3, 254, 2, 86, 1, 18, 0, 75, 1, 168, 2, 153,
  1, 142, 2, 20, 1, 11, 2, 238, 3, 70, 0, 3, 2, 206, 1, 126, 0, 3, 1, 254, 1,
  46, 2, 89, 3, 98, 0, 71, 3, 50, 3, 158, 0, 7, 1, 204, 1, 181, 2, 12, 2, 117,
  2, 76, 2, 113, 3, 249, 2, 140, 2, 113, 1, 245, 2, 204, 2, 113, 3, 125, 2, 113,
  2, 229, 2, 113, 3, 217, 3, 174, 1, 78, 0, 26, 0, 191, 1, 78, 3, 46, 0, 28, 1,
  14, 0, 108, 0, 195, 3, 174, 1, 190, 0, 227, 0, 230, 1, 234, 2, 204, 3, 21, 2,
  84, 0, 27, 1, 84, 2, 83, 3, 217, 2, 157, 2, 83, 3, 217, 3, 177, 2, 229, 2,
  204, 2, 109, 3, 125, 2, 140, 2, 109, 1, 245, 2, 76, 2, 109, 3, 249, 2, 12, 2,
  109, 2, 109, 2, 109, 1, 140, 2, 242, 3, 76, 2, 46, 3, 174, 1, 152, 2, 59, 0,
  148, 1, 123, 3, 234, 2, 122, 3, 11, 3, 22, 1, 103, 3, 150, 1, 14, 1, 106, 1,
  115, 2, 206, 0, 210, 3, 170, 1, 190, 1, 179, 3, 46, 3, 14, 0, 238, 2, 206, 2,
  46, 0, 206, 1, 102, 0, 148, 1, 219, 1, 24, 1, 230, 1, 231, 1, 152, 0, 108, 1,
  215, 2, 78, 2, 78, 0, 148, 2, 83, 0, 48, 1, 204, 0, 216, 0, 216, 0, 24, 2, 24,
  1, 88, 0, 24, 2, 88, 3, 171, 2, 89, 3, 226, 1, 46, 1, 126, 2, 27, 2, 210, 3,
  174, 1, 22, 3, 174, 3, 126, 2, 35, 3, 46, 3, 226, 3, 49, 0, 144, 2, 210, 3,
  126, 2, 87, 2, 254, 3, 142, 0, 48, 0, 144, 2, 206, 2, 46, 2, 131, 3, 142, 1,
  126, 2, 127, 3, 174, 1, 22, 3, 174, 1, 183, 0, 204, 1, 202, 1, 94, 2, 175, 0,
  190, 3, 38, 2, 238, 3, 44, 3, 23, 0, 102, 2, 219, 0, 84, 0, 3, 0, 146, 3, 102,
  1, 250, 2, 50, 3, 166, 0, 144, 2, 36, 1, 152, 2, 88, 0, 216, 0, 88, 1, 24, 1,
  216, 0, 88, 3, 155, 3, 230, 1, 147, 3, 142, 1, 98, 3, 19, 2, 206, 0, 60, 3,
  108, 3, 23, 1, 234, 2, 254, 3, 12, 0, 46, 2, 98, 3, 91, 1, 14, 1, 106, 2, 110,
  3, 63, 0, 206, 1, 42, 3, 142, 2, 126, 3, 31, 3, 166, 1, 142, 0, 46, 3, 12, 1,
  235, 2, 76, 0, 216, 0, 88, 0, 24, 0, 88, 1, 216, 2, 88, 2, 24, 0, 24, 1, 88,
  1, 88, 0, 216, 3, 119, 3, 174, 1, 46, 1, 134, 2, 186, 1, 123, 1, 250, 2, 206,
  1, 234, 3, 203, 1, 159, 0, 206, 3, 12, 0, 152, 0, 216, 0, 24, 0, 152, 1, 88,
  2, 7, 1, 76, 1, 251
};

// VARIABLES
static byte a[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // A register
static byte b[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // B register
static byte c[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // C register
static byte d[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // D register
static byte e[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // E register
static byte f[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // F register
static byte m[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // M scratchpad
static byte t[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Temporary
static byte s[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ; // Status
static byte p = 0, pc = 0, ret = 0; // Pointer
static byte offset = 0; // ROM offset
static byte first, last; // Register loop boundaries
static byte carry = 0, prevCarry = 0; // Carry bits
static byte key_code = _END, key_rom = 0; // Key variables
static boolean display_enable = true, display_update = true; // Display control
static byte msgnr = 0; // Message number

// SUBROUTINES
static byte do_add(byte x, byte y) { // Add 2 bytes
  int res = x + y + carry;
  if (res > 9) {
    res -= 10; carry = 1;
  }
  else carry = 0;
  return (res);
}
static byte do_sub(byte x, byte y) { // Substract 2 bytes
  int res = x - y - carry;
  if (res < 0) {
    res += 10; carry = 1;
  }
  else carry = 0;
  return (res);
}

static void process_rom(void) { // Process HP35-engine
  byte fetch_h, fetch_l, op_code = 0; // ROM fetch and operation code
  if ((pc == 191) & (offset == 0)) msgnr = MSGERROR; // Error handling

  prevCarry = carry; carry = 0; // Fetch ROM
  fetch_h = pgm_read_byte_near(rom + offset * 256 * 2 + pc * 2);
  fetch_l = pgm_read_byte_near(rom + offset * 256 * 2 + pc * 2 + 1);
  pc++; pc %= 256;

  if (key_code < _END) { // Process received key
    key_rom = key_code; key_code = _END; s[0] = 1;
  }
  if ((fetch_l & 0x03) == 0x01) { // Jump subroutine
    ret = pc;
    pc = (((fetch_l >> 2) & 0x3f) | ((fetch_h << 6) & 0x0c0));
  }
  if ((fetch_l & 0x7F) == 0x30) pc = ret; // Return from subroutine
  if ((fetch_l & 0x7F) == 0x10) offset = (((fetch_h << 1) & 06) | ((fetch_l >> 7) & 01)); // ROM
  if (fetch_l == 0xD0) { // Jump to pc + rom if key is available
    pc = key_rom; // Reset pointer
    s[0] = 0;
  }
  byte l3f = fetch_l & 0x3f; // Temporary variable
  if (l3f == 0x14) // Set carry due to status
    carry = s[((fetch_h & 0x03) << 2) | ((fetch_l & 0xc0) >> 6)];
  if (l3f == 0x04) s[((fetch_h & 0x03) << 2) | ((fetch_l & 0xc0) >> 6)] = 1; // Set s
  if (l3f == 0x24) s[((fetch_h & 0x03) << 2) | ((fetch_l & 0xc0) >> 6)] = 0; // Clear s
  if (l3f == 0x34) memset(s, 0, 12); // Clear stati
  if (l3f == 0x2C) { // Set carry
    carry = 0;
    if (p == (((fetch_h & 0x03) << 2) | ((fetch_l & 0xc0) >> 6))) carry = 1;
  }
  if (l3f == 0x0c) p = (((fetch_h & 0x03) << 2) | ((fetch_l & 0xc0) >> 6)); // Set p
  if (l3f == 0x3c) p = ((p + 1) & 0x0f);
  if (l3f == 0x1c) p = ((p - 1) & 0x0f);
  if (l3f == 0x18) { // Load constant
    c[p] = ((fetch_l >> 6) | (fetch_h << 2));
    p = ((p - 1) & 0x0f);
  }
  byte h3 = fetch_h & 0x03; // Temporary variable
  if ((h3 == 0x00) && ((fetch_l & 0xef) == 0xa8)) { // c<->m
    byte tmp[14];
    memcpy(tmp, c, WSIZE); memcpy(c, m, WSIZE); memcpy(m, tmp, WSIZE);
  }
  if ((h3 == 0x01) && ((fetch_l & 0xef) == 0x28)) { // c to stack
    memcpy(f, e, WSIZE); memcpy(e, d, WSIZE); memcpy(d, c, WSIZE);
  }
  if ((h3 == 0x01) && ((fetch_l & 0xef) == 0xa8)) { // Stack to a
    memcpy(a, d, WSIZE); memcpy(d, e, WSIZE); memcpy(e, f, WSIZE);
  }
  if ((h3 == 0x02) && ((fetch_l & 0xef) == 0xa8)) memcpy(c, m, WSIZE); // m to c
  if ((h3 == 0x03) && ((fetch_l & 0xef) == 0x28)) { // Rotate down
    byte tmp[14];
    memcpy(tmp, c, WSIZE); memcpy(c, d, WSIZE); memcpy(d, e, WSIZE);
    memcpy(e, f, WSIZE); memcpy(f, tmp, WSIZE);
  }
  if ((h3 == 0x03) && ((fetch_l & 0xef) == 0xa8)) // Clear all register
    for (byte i = 0; i < WSIZE; i++) a[i] = b[i] = c[i] = d[i] = e[i] = f[i] = m[i] = 0;
  if ((h3 == 0x00) && ((fetch_l & 0xef) == 0x28)) // No display
    display_enable = display_update = false;
  if ((h3 == 0x02) && ((fetch_l & 0xef) == 0x28)) display_enable = !display_enable; // Display
  if ((fetch_l & 0x03) == 0x03 && (prevCarry != 1)) // Conditional branch
    pc = ((fetch_l & 0xfc) >> 2) | ((fetch_h & 0x03) << 6);
  if ((fetch_l & 0x03) == 0x02) { // A&R calculations due to opcode
    op_code = ((fetch_l >> 5) & 0x07);
    op_code = op_code | ((fetch_h << 3) & 0x18);
    switch ((fetch_l >> 2) & 0x07) { // Get register boundaries first/last
      case 0: first = last = p; break;
      case 1: first = 3; last = 12; break;
      case 2: first = 0; last = 2; break;
      case 3: first = 0; last = 13; break;
      case 4: first = 0; last = p; break;
      case 5: first = 3; last = 13; break;
      case 6: first = last = 2; break;
      case 7: first = last = 13; break;
    }
    carry = 0;
    switch (op_code) { // Process opcode
      case 0x0: // 0+B
        for (byte i = first; i <= last; i++) if (b[i]) carry = 1; break;
      case 0x01: // 0->B
        for (byte i = first; i <= last; i++) b[i] = 0; break;
      case 0x02: // A-C
        for (byte i = first; i <= last; i++) t[i] = do_sub(a[i], c[i]); break;
      case 0x03: // C-1
        carry = 1;
        for (byte i = first; i <= last; i++) if (c[i] != 0) carry = 0; break;
      case 0x04: // B->C
        for (byte i = first; i <= last; i++) c[i] = b[i]; break;
      case 0x05: // 0-C->C
        for (byte i = first; i <= last; i++) c[i] = do_sub(0, c[i]); break;
      case 0x06: // 0->C
        for (byte i = first; i <= last; i++) c[i] = 0; break;
      case 0x07: // 0-C-1->C
        carry = 1;
        for (byte i = first; i <= last; i++) c[i] = do_sub(0, c[i]); break;
      case 0x08: // Sh A Left
        for (int8_t i = last; i > first; i--) a[i] = a[i - 1];
        a[first] = 0; break;
      case 0x09: // A->B
        for (byte i = first; i <= last; i++) b[i] = a[i]; break;
      case 0x0a: // A-C->C
        for (byte i = first; i <= last; i++) c[i] = do_sub(a[i], c[i]); break;
      case 0x0b: // C-1->C
        carry = 1;
        for (byte i = first; i <= last; i++) c[i] = do_sub(c[i], 0); break;
      case 0x0c: // C->A
        for (byte i = first; i <= last; i++) a[i] = c[i]; break;
      case 0x0d: // 0-C
        for (byte i = first; i <= last; i++) if (c[i] != 0) carry = 1; break;
      case 0x0e: // A+C->C
        for (byte i = first; i <= last; i++) c[i] = do_add(a[i], c[i]); break;
      case 0x0f: // C+1->C
        carry = 1;
        for (byte i = first; i <= last; i++) c[i] = do_add(c[i], 0); break;
      case 0x10: // A-B
        for (byte i = first; i <= last; i++) t[i] = do_sub(a[i], b[i]); break;
      case 0x11: // B<->C
        for (byte i = first; i <= last; i++) {
          byte tmp = b[i]; b[i] = c[i]; c[i] = tmp;
        } break;
      case 0x12: // Sh C Right
        for (byte i = first; i < last; i++) c[i] = c[i + 1];
        c[last] = 0; break;
      case 0x13: // A-1
        carry = 1;
        for (byte i = first; i <= last; i++) if (a[i] != 0) carry = 0; break;
      case 0x14: // Sh B Right
        for (byte i = first; i < last; i++) b[i] = b[i + 1];
        b[last] = 0; break;
      case 0x15: // C+C->A
        for (byte i = first; i <= last; i++) c[i] = do_add(c[i], c[i]); break;
      case 0x16: // Sh A Right
        for (byte i = first; i < last; i++) a[i] = a[i + 1];
        a[last] = 0; break;
      case 0x17: // 0->A
        for (byte i = first; i <= last; i++) a[i] = 0; break;
      case 0x18: // A-B->A
        for (byte i = first; i <= last; i++) a[i] = do_sub(a[i], b[i]); break;
      case 0x19: // A<->B
        for (byte i = first; i <= last; i++) {
          byte tmp = a[i]; a[i] = b[i]; b[i] = tmp;
        } break;
      case 0x1a: // A-C->A
        for (byte i = first; i <= last; i++) a[i] = do_sub(a[i], c[i]); break;
      case 0x1b: // A-1->A
        carry = 1;
        for (byte i = first; i <= last; i++) a[i] = do_sub(a[i], 0); break;
      case 0x1c: // A+B->A
        for (byte i = first; i <= last; i++) a[i] = do_add(a[i], b[i]); break;
      case 0x1d: // C<->A
        for (byte i = first; i <= last; i++) {
          byte tmp = a[i]; a[i] = c[i]; c[i] = tmp;
        } break;
      case 0x1e: // A+C->A
        for (byte i = first; i <= last; i++) a[i] = do_add(a[i], c[i]); break;
      case 0x1f: // A+1->A
        carry = 1;
        for (byte i = first; i <= last; i++) a[i] = do_add(a[i], 0); break;
    }
  }

  if (display_enable) display_update = true; // Display
  else if (display_update) {
    printscreen();
    islock = false; count = 0;
  }
}


//--v----1----v----2----v----3----v----4----v----5----v----6----v----7----v----8
// P R I N T

static void message(byte n) { // Message n to display buffer
  for (byte i = 0; i < DIGITS; i++) {
    byte pos = DIGITS * (n - 1) + i; dbuf[i] = pgm_read_byte(&msg[pos]);
  }
}

static void printscreen(void) { // Print screen
  if (msgnr) message(msgnr); // ### Message
  else if (darkscreen) memset(dbuf, 0, DIGITS); // ### Screensaver
  else { // ### Stack
    byte pos = 0;
    for (int8_t i = WSIZE - 1; i > 5; i--) {
      dbuf[pos] = 0;
      if (b[i] >= 8) dbuf[pos++] = 0; // Trailing blanks
      else if (i == 13) { // Sign
        if (a[i] >= 8) dbuf[pos++] = _7MINUS; else pos++;
      }
      else dbuf[pos++] = digit[a[i]]; // Number
      if (b[i] == 2) dbuf[pos - 1] |= _7DOT; // Dot
    }
    if ((b[0] | b[1] | b[2]) < 8) { // EEX
      dbuf[5] = (a[2] < 8) ? 0 : _7MINUS;
      dbuf[6] = digit[a[1]]; dbuf[7] = digit[a[0]];
    }
  }

  printbuf(); display_update = false;
}


//--v----1----v----2----v----3----v----4----v----5----v----6----v----7----v----8
// S E T U P   a n d   L O O P

void setup() { // SETUP
  pinMode(STROBE, OUTPUT); pinMode(CLOCK, OUTPUT); pinMode(DATA, OUTPUT);
}

// DISPATCH ARRAYS
const byte k2c[] PROGMEM = { // Key to key_code
  _END, _1, _7, _2, _8, _3, _9, _CLX, _EEX, _0, _4, _DOT, _5, _ENTER, _6, _CHS,
  _END, _EXP, _INV, _LN, _SQRT, _SUB, _DIV, _CLR, _SWAP, _STO, _POW, _RCL, _LOG, _ADD, _MULT, _ROT,
  _END, _TAN, _SIN, _FGAMMA, _FR2P, _FQE, _FP2R, _LIT, _END, _ARC, _COS, _HYP, _FPV, _PI, _FND, _END,
  _END, _FTANH, _FSINH, _FATANH, _FASINH, _END, _END, _END, _END, _END, _FCOSH, _END, _FACOSH, _END, _END, _END
};
const byte c2m[] PROGMEM = { // Key-code to mem-pointer
  _R2P, _P2R, _PV, _ND, _GAMMA, _QE, _SINH, _COSH, _TANH, _ASINH, _ACOSH, _ATANH
};

void loop() { // MAIN LOOP

  if (key && key < _END) { // Execute key
    display_update = true; msgnr = 0;
    darkscreen = isrun = false; // Display on and Break
    if (key == _KF) { // F-key pressed
      if (++fg > 2) { // FFF
        darkscreen = true; fg = 0;
      }
    }
    else { // Dispatch key
      byte pos = fg * KEYS + key - 1;
      key_code = pgm_read_byte(&k2c[pos]); // Get key_code
      fg = 0;
      if (key_code == _HYP) { // Demand hyperbolic function
        fg = 3; msgnr = MSGHYP;
      }
      else if (key_code == _LIT) { // LIT+
        command(0x88 | ++brightness);
        key_code = _END;
      }
      else if (key_code > _LIT) { // Run
        memp = pgm_read_byte(&c2m[key_code - _LIT - 1]); // Get function pointer
        isrun = true; msgnr = MSGRUN;
      }
    }
  }

  process_rom(); // Process HP35-ROM-Engine

  if (isrun && !islock && count++ > 5) {
    byte cmd = pgm_read_byte(mem + memp++);
    if (cmd == _END) { // End run
      isrun = false; display_update = true; msgnr = 0;
    }
    else key_code = cmd; // Assign key code
    islock = true;
  }

  key = getkey(); if (key == oldkey) key = _END; else oldkey = key; // Get key
}
