#include <screen.h>
#include <color.h>
#include <io.h>

screen_t screens[NB_SCREEN];
int current_screen = 0;
int mode_split = 0;

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
    screen_refresh();
}

void    screen_switch(int id) {
    if (id < 0 || id >= NB_SCREEN) return;
    
    if (mode_split)
    split_refresh(current_screen, id);
    else {
        current_screen = id;
        screen_refresh();
    }
}