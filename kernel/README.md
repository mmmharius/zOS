# kernel/

- `kernel.c` — `main()`, entry point depuis boot.asm
- `keyboard.c` — lecture PS/2, scancodes, keyboard_loop()
- `debug.c` — affichage état kernel sur screen 2 (DEBUG only)
- `screen/` — voir `screen/README.md`

---

## kernel.c

```c
int main() {
    screen_init();
    printk(VGA, logo);
    keyboard_loop();
    while (1);
}
```

Logo selon le build :

```
make       → ASCII art "ZOS"
make debug → même art + "DEBUG VERSION" + dump serial de tous les screens
make corr  → "42"
```

---

## keyboard.c

### Ports PS/2

```
0x60  KB_DATA    → lire le scancode
0x64  KB_STATUS  → bit 0 = donnée disponible
```

Pas d'interruptions (pas d'IDT). Polling pur :

```c
unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);
    return inb(KB_DATA);
}
```

### Scancodes

Les scancodes ne sont pas de l'ASCII. Chaque touche envoie deux events :

```
press   → bit 7 = 0   ex: 'A' = 0x1E
release → bit 7 = 1   ex: 'A' release = 0x9E

sc & 0x7F  → strip bit 7 pour obtenir l'id de la touche
sc & 0x80  → test release
```

### Table des scancodes

```
0x02='1'
0x0F=TAB   0x10=Q  0x11=W  0x12=E  0x13=R  0x14=T
0x15=Y     0x16=U  0x17=I  0x18=O  0x19=P
0x1C=ENTER 0x1D=CTRL
0x1E=A  0x1F=S  0x20=D  0x21=F  0x22=G
0x23=H  0x24=J  0x25=K  0x26=L
0x2C=Z  0x2D=X  0x2E=C  0x2F=V  0x30=B  0x31=N  0x32=M
0x0E=BACKSPACE  0x39=SPACE  0x80=KEY_RELEASE (flag bit)
```

Les gaps entre les rangées correspondent aux touches Caps Lock, Shift, etc.

### Ctrl

Ctrl est le seul key dont on track press **et** release :

```c
if (key == KEY_CTRL) {
    ctrl_pressed = !(sc & KEY_RELEASE);
    continue;
}
```

Tous les autres keys ignorent le release (`sc & KEY_RELEASE → continue`).

### Flow complet

```
read_keyboard() → sc

key = sc & 0x7F

key == CTRL          → ctrl_pressed = !(sc & 0x80) ; continue
sc & KEY_RELEASE     → continue
ctrl && key == KEY_G → toggle_debug_screen() (DEBUG) ; continue
key == KEY_TAB       → switch screen / focus split ; continue
key == KEY_1         → screen_toggle_split() ; continue

c = scancode_to_ascii(sc)
debug_print_state(sc)  (DEBUG)

c == 0   → skip
c == '\b'→ screen_backspace()
else     → screen_putchar(c, scr.current)
```

---

## debug.c (DEBUG only)

`debug_print_state(sc)` : réécrit les 4 premières lignes de screen 2 à chaque keypress.

```
row 0: [KEY PRESS]| key=A sc=0x1e
row 1: [SCREN]    | cur=0 mode=1 sp-L=0 sp-R=2
row 2: [CURRENT]  | row=7 col=3
row 3: [FLAG]     | scr_0=0x0B scr_1=0x01 scr_2=0x12
row 4: [HISTORY]  | A B C D E F G H I J
```

History = circular buffer de `DBG_HISTORY_SIZE` (10) keypresses.

`print_all_screen()` : dump des 4 screen buffers sur le port série au démarrage DEBUG.
