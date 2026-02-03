#include "kernel.h"
#include "keyboard.h"
#include "screen.h"
#include "ft_printk.h"

int COL = 0;
int ROW = 0;



void    main() {
    // print_42();
    int c = 12;
    ft_printk("Hello from ft_kprintf!\n");
    ft_printk("Teste : %d", c);
    move_cursor();
    keyboard_loop();
    while(1);
}
