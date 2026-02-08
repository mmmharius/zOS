#include <screen.h>
#include <color.h>

void    split_refresh(int left_id, int right_id) {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    screen_t *s_left = &screens[left_id];
    screen_t *s_right = &screens[right_id];
    uint16_t color_left = get_screen_color(left_id);
    uint16_t color_right = get_screen_color(right_id);
    
    for (int row = 0; row < VGA_HEIGHT; row++)
    for (int col = 0; col < VGA_WIDTH/2; col++)
    vga[row*VGA_WIDTH + col] = (uint16_t)s_left->buffer[row*VGA_WIDTH + col] | color_left;
    
    for (int row = 0; row < VGA_HEIGHT; row++)
    for (int col = 0; col < VGA_WIDTH/2; col++)
    vga[row*VGA_WIDTH + col + VGA_WIDTH/2] = (uint16_t)s_right->buffer[row*VGA_WIDTH + col] | color_right;
}

void    screen_refresh() {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    uint16_t color = get_screen_color(current_screen);
    
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = ' ' | color;
    }
    
    if (mode_split == 0) {
        screen_t *s = &screens[current_screen];
        uint16_t color_screen = get_screen_color(current_screen);
        
        for (int row = 0; row < VGA_HEIGHT; row++) {
            for (int col = 0; col < VGA_WIDTH; col++) {
                char c = s->buffer[row * VGA_WIDTH + col];
                vga[row * VGA_WIDTH + col] = (uint16_t)c | color_screen;
            }
        }
    }
    else
    split_refresh(current_screen, -1);
    update_cursor();
}

void    screen_toggle_split() {
    mode_split = (mode_split == 1) ? 0 : 1;
    screen_refresh();
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