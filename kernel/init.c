#include <keyboard.h>
#include <screen.h>
#include <printk.h>

int SCREEN = 1;
int HALF_SCREEN = 0;

t_screen screens[NB_SCREEN + 1];
t_screen *current;

t_screen s_creen = {
    .row = 0,
    .col = 0,
    .scroll = 0
};

void    init() {
    init_screen();
}
void    init_screen() {
    for (int i = 0; i <= (NB_SCREEN - 1); i++) {
        printk(1, "%d, %d", i, (NB_SCREEN - 1));
        screens[i].row = 0;
        screens[i].col = 0;
        screens[i].scroll = 0;
        screens[i].row_max = VGA_HEIGHT;
        screens[i].col_max = VGA_WIDTH;
        screens[i].row_start = 0;
        screens[i].col_start = 0;
        for (int j = START_PRINT; j < VGA_WIDTH * (VGA_HEIGHT * SCREEN_LENGHT); j++)
            screens[i].buffer[j] = (uint16_t)' ' | VGA_DEFAULT_COLOR;
    }
    current = &screens[0];
}
