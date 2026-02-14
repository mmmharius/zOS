#include <screen.h>
#include <color.h>
#include <io.h>
#ifdef DEBUG
    #include <printk.h>
#endif

screen_t screens[NB_SCREEN];
int current_screen = 0;
int mode_split = 0;
int current_split = 0;

void    screen_init() {
    for (int i = 0; i < NB_SCREEN; i++) {
        screens[i].row = 0;
        screens[i].col = 0;
        screens[i].start_row = (i == 0) ? 5 : 0;
        
        for (int j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++) {
            screens[i].buffer[j] = ' ';
        }
    }
    
    current_screen = 0;
    mode_split = 0;
    current_split = 0;
    screen_refresh();
}

void    screen_switch(int id) {
    if (id < 0 || id >= NB_SCREEN) {
        #ifdef DEBUG
            printk(1, "erreur screen switch\n");
        #endif
        return ;
    }
    if (mode_split) {
        int left = current_split;
        int right = (current_split + 1) % NB_SCREEN;
        if (current_screen == right)
            split_refresh(right, left);
        else
            split_refresh(left, right);
        #ifdef DEBUG
            printk(1, "curr screen : %d, other screen : %d\n", current_split, current_split + 1);
        #endif
    }
    else {
        current_screen = id;
        screen_refresh();
    }
}