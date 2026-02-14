#include <screen.h>
#include <io.h>
#ifdef DEBUG
    #include <printk.h>
    #include <debug.h>
#endif

int get_width() {
    return (mode_split) ? VGA_WIDTH / 2 : VGA_WIDTH;
}

int get_vga_pos(int screen_id, int row, int col) {
    if (mode_split) {
        int offset = 0;
        if (screen_id == (current_split + 1) % NB_SCREEN)
            offset = VGA_WIDTH / 2;
        return row * VGA_WIDTH + col + offset;
    }
    return row * VGA_WIDTH + col;
}

void update_cursor() {
    screen_t *s = &screens[current_screen];
    int pos = get_vga_pos(current_screen, s->row, s->col);
    
    outb(0x3D4, 0x0F); // 0x0F  = cursor low byte
    // 0x3D4 = choose register | 
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E); // 0x0E  = cursor high byte
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    #ifdef DEBUG
        printk(1, "curr_screen : %d, pos_row : %d, pos_col : %d\n", current_screen, s->row, s->col);
        print_current_screen();
    #endif
}

void replace_row(int row, int id) {
    screen_t *s = &screens[id];
    int width = get_width();
    for (int col = 0; col < width; col++) {
        s->buffer[row * VGA_WIDTH + col] = ' ';
    }
}

void scroll(int id) {
    screen_t *s = &screens[id];
    int width = get_width();
    
    for (int row = s->start_row; row < VGA_HEIGHT - 1; row++) {
        for (int col = 0; col < width; col++) {
            s->buffer[row * VGA_WIDTH + col] = 
            s->buffer[(row + 1) * VGA_WIDTH + col];
        }
    }
    replace_row(VGA_HEIGHT - 1, id);
    s->row = VGA_HEIGHT - 1;
    s->col = 0;
}
