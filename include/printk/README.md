# include/printk/

Builds into `printk.a`.

```
includes/printk.h    constantes + déclarations
src/printk.c         printk(), dispatch formats, ft_kputchar, ft_putstr
src/ft_puthex.c      %x / %X
src/ft_putnbr.c      %d / %i
src/ft_putnsigned.c  %u
src/ft_putptr.c      %p
```

---

## Destinations

```c
#define VGA    0   // screen actif (scr.current)
#define SERIAL 1   // COM1 — 0x3F8
#define DBG    2   // screen 2 (DEBUG only)
```

---

## printk

```c
int printk(int output, const char *str, ...)
```

Pas de `va_list`. Les args sont lus directement sur la stack :

```c
uint32_t *args = (uint32_t *)(&str + 1);
```

Fonctionne parce que le kernel est compilé en 32-bit cdecl (`-m32`) :
tous les args sont sur la stack dans l'ordre, `&str + 1` pointe sur le premier.

### Formats supportés

```
%c   ft_kputchar
%s   ft_putstr
%d   ft_putnbr     (signed)
%i   ft_putnbr
%u   ft_putnsigned (unsigned)
%x   ft_puthex     (hex lowercase)
%X   ft_puthex     (hex uppercase)
%p   ft_putptr     (0x + 8 chiffres hex)
%%   '%'
```

---

## ft_kputchar

```c
if (output == SERIAL) {
    while ((inb(COM1 + 5) & 0x20) == 0);  // attendre TX empty
    outb(COM1, c);
}
else if (output == DBG)   screen_putchar(c, DEBUG_SCREEN_ID);
else                      screen_putchar(c, scr.current);
```

`COM1 + 5` = `0x3FD` = Line Status Register. Bit 5 = transmitter empty.

---

## Registres COM1 (base 0x3F8)

```
+0  Data Register
+1  Interrupt Enable Reg
+2  Interrupt ID Reg
+3  Line Control Reg
+4  Modem Control Reg
+5  Line Status Reg    ← bit 5 : TX empty
+6  Modem Status Reg
+7  Scratch Reg
```

---

## Build

```sh
make                        # → printk.a
make EXTRA_CFLAGS=-DDEBUG   # active le path DBG
```

Géré automatiquement par le Makefile principal.
