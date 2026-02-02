#include "kernel.h"
#include "libasm/libasm.h"
#include <sys/io.h>

int COL = 0;
int LIN = 0;

void    print_42() {
    volatile uint16_t* vga = (uint16_t*)0xB8000; // 0xB8000 -> VGA TEXT mode video memory address

    vga[LIN + COL * 80] = (uint16_t)'4' | 0x0F00;  // each vga[i] is 2 bytes = 16 bits
    LIN++;
    vga[LIN + COL * 80] = (uint16_t)'2' | 0x0F00;
    LIN++;
}

unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);
        return inb(KB_DATA);
}

void    print_char(char c) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    if (LIN == 80) {
        COL++;
        LIN = 0;
    }
    vga[LIN + COL * 80] = (uint16_t)c | 0x0F00;
    LIN++;
}


/*
    vga[0] = (uint16_t)'4' | 0x0F00

    (uint16_t)'4' in ASCII  = 0x0034
    attribute (white/black) = 0x0F00
    result                  = 0x0F34

    0x0F34 = 0000 1111 0011 0100
             └┬─┘ └┬─┘ └───┬───┘
              │    │       └─ ASCII character ('4')
              │    └── foreground color of text
              └── color of screen behind text

    Foreground (lower 4 bits of attribute):
    0xF (or 1111) → white text

    Background (upper 4 bits of attribute):
    0x0 (or 0000) → black background

    Memory (little endian):
    [0xB8000] = 0x34  // low byte character 
    [0xB8001] = 0x0F  // high byte attribute
*/

void main() {
    // print_42();
    print_char(read_keyboard());
}
