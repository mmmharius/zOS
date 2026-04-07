# boot/

Un seul fichier : `boot.asm`.

---

## Multiboot header

GRUB scanne les 8192 premiers octets du binaire pour trouver `0x1BADB002`.
Les trois valeurs doivent être présentes et leur somme doit valoir 0 mod 2^32.

```asm
MBOOT_MAGIC     equ 0x1BADB002
MBOOT_FLAGS     equ 0x00000003
MBOOT_CHECKSUM  equ -(MBOOT_MAGIC + MBOOT_FLAGS)
```

```
FLAGS = 0x00000003

  0000 0000 0000 0000 0000 0000 0000 0011
                                        ││
                                        │└─ bit 0 : aligne les modules sur 4 KB
                                        └── bit 1 : demande la memory map à GRUB
```

```
CHECKSUM = -(0x1BADB002 + 0x00000003) = 0xE4524FFB

0x1BADB005 + 0xE4524FFB = 0x100000000 → lower 32 bits = 0x00000000
```

La section `.multiboot` est séparée de `.text` pour que le linker la place en tête du binaire, dans la fenêtre des 8192 octets scannés par GRUB.

---

## _start

```asm
_start:
    cli        ; pas d'IDT → on désactive les interruptions
    call main
    hlt        ; si main() retourne
    jmp _start
```

---

## Linker script

Le kernel se charge à 1 MB (`0x100000`). Tout ce qui est en dessous est réservé (BIOS, VGA, ROM).

```
Physical memory :

  0x00000000 – 0x000003FF   IVT (real mode)
  0x00000400 – 0x000004FF   BIOS data area
  0x00007C00 – 0x00007DFF   MBR
  0x000A0000 – 0x000BFFFF   VGA
  0x000C0000 – 0x000FFFFF   BIOS ROM — inutilisable

  0x00100000  ← kernel
```

Ordre des sections à 0x100000 :

```
.multiboot   magic, flags, checksum
.text        code
.rodata      string literals
.data        globals initialisés
.bss         globals à zéro (screen_mgr_t scr)
```

`.bss` n'est pas stocké dans le binaire ELF, juste sa taille. GRUB le met à zéro au chargement.
