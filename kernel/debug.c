#include "debug.h"


void    print_screen(int id) {
    printk(1, "SCREEN : %d\n\n", id);
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            printk(1, "%c", screens[id].buffer[row * VGA_WIDTH + col]);
        }
        printk(1, "\n");
    }
}

void    print_all_screen() {
    for (int i = 0; i < NB_SCREEN; i++) {
        print_screen(i);
    }
}

void    print_current_screen() {
    print_screen(current_screen);
}