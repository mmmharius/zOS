#include <screen.h>
#include <color.h>
#include <io.h>
#ifdef DEBUG
    #include <printk.h>
#endif

void    screen_putchar(char c) {
    screen_t *s = &screens[current_screen];
    int width = get_width();
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    uint16_t color = get_screen_color(current_screen);
    
    if (c == '\n') {
        s->col = 0;
        s->row++;
    } else {
        s->buffer[s->row * VGA_WIDTH + s->col] = c;
        
        int pos = get_vga_pos(current_screen, s->row, s->col);
        vga[pos] = (uint16_t)c | color;
        
        s->col++;
        
        if (s->col >= width) {
            s->col = 0;
            s->row++;
        }
    }
    
    if (s->row >= VGA_HEIGHT) {
        scroll(current_screen);
        screen_refresh();
    }
    
    update_cursor();
}

void    screen_backspace() {
    screen_t *s = &screens[current_screen];
    int width = get_width();
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    uint16_t color = get_screen_color(current_screen);
    
    if (s->col > 0) {
        s->col--;
    } else if (s->row > s->start_row) {
        s->row--;
        
        for (s->col = width - 1; s->col > 0; s->col--) {
            if (s->buffer[s->row * VGA_WIDTH + s->col] != ' ') {
                s->col++;
                break;
            }
        }
        if (s->col == 0 && s->buffer[s->row * VGA_WIDTH] == ' ') {
            s->col = 0;
        }
    }
    
    s->buffer[s->row * VGA_WIDTH + s->col] = ' ';
    int pos = get_vga_pos(current_screen, s->row, s->col);
    vga[pos] = ' ' | color;
    
    update_cursor();
}
