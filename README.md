# zOS

Kernel x86 32-bit bare-metal. C + NASM. Boot GRUB, VGA text mode, clavier PS/2.
Pas de libc, pas de syscalls, pas de filesystem.

---

## Setup

```sh
git clone git@github.com:mmmharius/zOs.git
cd zOs

# si include/libasm_zOs/ est vide après le clone :
git clone git@github.com:mmmharius/libasm_zOs.git include/libasm_zOs
```

### Dépendances

```sh
sudo apt install nasm gcc grub-pc-bin grub-common xorriso qemu-system-x86

# si gcc -m32 fail :
sudo apt install gcc-multilib
```

---

## Build

```sh
make          # kernel.bin
make run      # kernel.bin + ISO + QEMU
make debug    # -DDEBUG → active Ctrl+G debug panel
make corr     # -DCORR  → affiche juste "42"
make iso      # ISO seulement, pas de QEMU
make clean
make fclean
make re
```

QEMU est lancé avec `-serial stdio` → `printk(SERIAL, ...)` sort dans le terminal.

---

## Contrôles

| Touche      | Action                                              |
|-------------|-----------------------------------------------------|
| `Tab`       | Screen suivant (mode normal)                        |
| `Tab`       | Changer de panel actif (split)                      |
| `1`         | Toggle split view                                   |
| `Ctrl+G`    | Toggle debug panel (`make debug` requis)            |
| `Backspace` | Supprime le dernier caractère                       |
| `Enter`     | Nouvelle ligne                                      |

---

## Structure

```
zOs/
├── boot/
│   ├── boot.asm             multiboot header + _start
│   └── README.md
├── kernel/
│   ├── kernel.c             main()
│   ├── keyboard.c           polling PS/2, keyboard_loop()
│   ├── debug.c              debug state (DEBUG only)
│   ├── README.md
│   └── screen/
│       ├── screen_core.c    screen_init, screen_switch
│       ├── screen_display.c vga_draw_screen, split refresh
│       ├── screen_io.c      screen_putchar, screen_backspace
│       ├── screen_utils.c   get_vga_pos, update_cursor, scroll
│       └── README.md
├── include/
│   ├── color.h              VGA_COLOR(), palette, couleurs par screen
│   ├── debug.h              DBG_HISTORY_SIZE
│   ├── io.h                 inb() / outb()
│   ├── keyboard.h           scancodes PS/2
│   ├── screen.h             screen_t, screen_mgr_t, SCR_* flags
│   ├── stdint.h             uint8_t, uint16_t, uint32_t…
│   ├── printk/              → printk.a
│   │   └── README.md
│   └── libasm_zOs/          → libasm_zOs.a  (submodule)
│       └── README.md
├── linker.ld
└── Makefile
```

---

## Ports I/O

```
0x60   PS/2 data              keyboard.c
0x64   PS/2 status (bit 0)    keyboard.c
0x3D4  VGA CRTC index         screen_utils.c — update_cursor()
0x3D5  VGA CRTC data          screen_utils.c — update_cursor()
         reg 0x0E = cursor high byte
         reg 0x0F = cursor low  byte
0x3F8  COM1 serial data       printk.c
0x3FD  COM1 line status       printk.c — bit 5 : TX empty
```
