#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#include <printk.h>
#include <color.h>
#include <init.h>
#ifdef DEBUG
    #include "debug.h"
#endif

void replace_entire_row(int row, char c)
{
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;

    for (int col = 0; col < VGA_WIDTH; col++) {
        vga[row * VGA_WIDTH + col] =
            (uint16_t)c | VGA_DEFAULT_COLOR;
    }
}

void    scroll() {
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;

    for (int row = START_PRINT + 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga[(row - 1) * VGA_WIDTH + col] = vga[row * VGA_WIDTH + col];
        }
    }
    current->row = VGA_HEIGHT - 1;
    current->col = 0;
    replace_entire_row(current->row, ' ');
}

void    check_col() {
    if (current->col >= VGA_WIDTH) {
        current->col = 0;
        current->row++;
    }
    if (current->row >= VGA_HEIGHT)
        scroll();
}

void    print_char(char c) {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR; // 0xB8000 -> VGA TEXT mode video memory address
    check_col();
    vga[current->col + current->row * VGA_WIDTH] = (uint16_t)c | VGA_DEFAULT_COLOR;
    current->buffer[current->row * VGA_WIDTH + current->col] = c;
    current->col++;
    move_cursor();
}

void    print_42() {
    print_char('4');
    print_char('2');
}

void cursor_for_half() {
    if (current == &screens[0]) {
        int linear_pos = current->row * VGA_WIDTH + current->col;

        current->row = linear_pos / (VGA_WIDTH / 2);
        current->col = linear_pos % (VGA_WIDTH / 2);
    }
    else if (current == &screens[1]) {
        int linear_pos = current->row * VGA_WIDTH + current->col;

        current->row = linear_pos / (VGA_WIDTH / 2);
        current->col = linear_pos % (VGA_WIDTH / 2);
    }
}


void    switch_screen(int id) {
#ifdef DEBUG
    printk(1, "switching to screen %d\n", id);
#endif
    if (id < 0 || id >= NB_SCREEN)
        return;
    current = &screens[id];
    load_screen();
    move_cursor();
}

void    half_screen() {
    screens[0].col_max = VGA_WIDTH / 2;
    screens[1].col_start = VGA_WIDTH / 2;
    cursor_for_half();
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    HALF_SCREEN = 1;
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH / 2; col++) {
            vga[row * VGA_WIDTH + col] = screens[0].buffer[row * VGA_WIDTH + col] | VGA_DEFAULT_COLOR;
        }
        for (int col = 0; col < VGA_WIDTH / 2; col++) {
            vga[row * VGA_WIDTH + (col + VGA_WIDTH / 2)] = screens[1].buffer[row * VGA_WIDTH + col] | VGA_DEFAULT_COLOR;
        }
    }
    current = &screens[NB_SCREEN];
    load_half();
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