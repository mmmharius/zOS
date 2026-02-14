#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define VGA_ADDR 0xB8000 // 0xB8000 -> VGA TEXT mode video memory address
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define NB_SCREEN 3

typedef struct {
    int     row;
    int     col;
    int     start_row;
    char    buffer[VGA_WIDTH * VGA_HEIGHT];
} screen_t;

extern screen_t screens[NB_SCREEN];
extern int      current_screen;
extern int      mode_split;
extern int      current_split;

void    screen_init();
void    screen_switch(int id);
void    screen_toggle_split();
void    screen_putchar(char c);
void    screen_backspace();
void    screen_refresh();

int     get_width();
int     get_vga_pos(int screen_id, int row, int col);
void    update_cursor();
void    replace_row(int row, int id);
void    scroll(int id);
void    split_refresh(int left_id, int right_id);

#endif