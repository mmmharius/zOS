#ifndef DEBUG_H
#define DEBUG_H

#include <color.h>
#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#include <printk.h>

void    print_all_screen();
void    print_current_screen();
void    print_screen(int id);

#endif