#include <screen.h>
#include <color.h>
#include <io.h>

screen_t screens[NB_SCREEN];
int current_screen = 0;
int mode_split = 0;


static inline int get_width() {
    return (mode_split == 1) ? (VGA_WIDTH / 2) : VGA_WIDTH;
}

static inline int get_vga_pos(int screen_id, int row, int col) {
    if (mode_split) {
        return row * VGA_WIDTH + col + (screen_id * VGA_WIDTH / 2);
    }
    return row * VGA_WIDTH + col;
}

static void update_cursor() {
    screen_t *s = &screens[current_screen];
    int pos = get_vga_pos(current_screen, s->row, s->col);
    
    outb(0x3D4, 0x0F); // 0x0F  = cursor low byte
    // 0x3D4 = choose register | 
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E); // 0x0E  = cursor high byte
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void replace_row(int row, int id) {
    screen_t *s = &screens[id];
    int width = get_width();
    for (int col = 0; col < width; col++) {
        s->buffer[row * VGA_WIDTH + col] = ' ';
    }
}

static void scroll(int id) {
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

void screen_init() {
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

void screen_switch(int id) {
    if (id < 0 || id >= NB_SCREEN) return;
        
    current_screen = id;
    screen_refresh();
}

void screen_toggle_split() {
    mode_split = (mode_split == 1) ? 0 : 1;
    screen_refresh();
}

void screen_putchar(char c) {
    screen_t *s = &screens[current_screen];
    int width = get_width();
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    
    if (c == '\n') {
        s->col = 0;
        s->row++;
    } else {
        s->buffer[s->row * VGA_WIDTH + s->col] = c;
        
        int pos = get_vga_pos(current_screen, s->row, s->col);
        vga[pos] = (uint16_t)c | VGA_DEFAULT_COLOR;
        
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

void screen_backspace() {
    screen_t *s = &screens[current_screen];
    int width = get_width();
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    
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
    vga[pos] = ' ' | VGA_DEFAULT_COLOR;
    
    update_cursor();
}

void screen_refresh() {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = ' ' | VGA_DEFAULT_COLOR;
    }
    
    if (mode_split == 0) {
        screen_t *s = &screens[current_screen];
        for (int row = 0; row < VGA_HEIGHT; row++) {
            for (int col = 0; col < VGA_WIDTH; col++) {
                char c = s->buffer[row * VGA_WIDTH + col];
                vga[row * VGA_WIDTH + col] = (uint16_t)c | VGA_DEFAULT_COLOR;
            }
        }
    } 
    else {
        for (int id = 0; id < NB_SCREEN; id++) {
            screen_t *s = &screens[id];
            int offset = id * (VGA_WIDTH / 2);
            
            for (int row = 0; row < VGA_HEIGHT; row++) {
                for (int col = 0; col < VGA_WIDTH / 2; col++) {
                    char c = s->buffer[row * VGA_WIDTH + col];
                    vga[row * VGA_WIDTH + col + offset] = (uint16_t)c | VGA_DEFAULT_COLOR;
                }
            }
        }
    }
    update_cursor();
}
