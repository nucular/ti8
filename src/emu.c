#include "emu.h"

#include <alloc.h>
#include <mem.h>

#include "main.h"


void emu_init()
{
  emu_memory = malloc(EMU_MEMSIZE);
  if (!emu_memory) fatal("Could not initialize emulator", "Out of memory");

  // Load the font set
  memcpy(emu_memory, emu_font, 80);
}

void emu_exit()
{
  if (emu_memory)
  {
    free(emu_memory);
  }
}
