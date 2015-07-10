#ifndef EMU_H
#define EMU_H

#define EMU_MEMSIZE 0xFFF
#define EMU_PROGSTART 0x200

void emu_init();
void emu_exit();

char *emu_memory;

#endif
