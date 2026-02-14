#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#ifdef DEBUG
    #include <printk.h>
#endif

unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);
    return inb(KB_DATA);
}

char scancode_to_ascii(unsigned char sc) {
    switch(sc) {
        case KEY_1: return '1';
        case KEY_Q: return 'Q';
        case KEY_W: return 'W';
        case KEY_E: return 'E';
        case KEY_R: return 'R';
        case KEY_T: return 'T';
        case KEY_Y: return 'Y';
        case KEY_U: return 'U';
        case KEY_I: return 'I';
        case KEY_O: return 'O';
        case KEY_P: return 'P';
        case KEY_A: return 'A';
        case KEY_S: return 'S';
        case KEY_D: return 'D';
        case KEY_F: return 'F';
        case KEY_G: return 'G';
        case KEY_H: return 'H';
        case KEY_J: return 'J';
        case KEY_K: return 'K';
        case KEY_L: return 'L';
        case KEY_Z: return 'Z';
        case KEY_X: return 'X';
        case KEY_C: return 'C';
        case KEY_V: return 'V';
        case KEY_B: return 'B';
        case KEY_N: return 'N';
        case KEY_M: return 'M';
        case KEY_ENTER: return '\n';
        case KEY_BACKSPACE: return '\b';
        case KEY_SPACE: return ' ';
        default: return 0;
    }
}

void keyboard_loop() {
    while(1) {
        unsigned char sc = read_keyboard();
        unsigned char key = sc & 0x7F;
        
        if (sc & KEY_RELEASE)
            continue;
        
        if (key == KEY_TAB) {
            if (mode_split) {
                int other = (current_split + 1) % NB_SCREEN;
        
                if (current_screen == current_split)
                    current_screen = other;
                else
                    current_screen = current_split;
                // split_refresh(current_split, (current_split + 1) % NB_SCREEN);
                update_cursor();
            }
            else
                screen_switch((current_screen + 1) % NB_SCREEN);
            continue;
        }
        
        if (key == KEY_1) {
            screen_toggle_split();
            continue;
        }
        
        char c = scancode_to_ascii(sc);
        if (c == 0) 
            continue;
        if (c == '\b')
            screen_backspace();
        else
            screen_putchar(c);
    }
}
