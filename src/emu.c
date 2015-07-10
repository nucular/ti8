#include "emu.h"

#include <alloc.h>

#include "main.h"


void emu_init()
{
  emu_memory = malloc(EMU_MEMSIZE);
  if (!emu_memory)
  {
    fatal("Could not initialize emulator: Out of memory");
  }
}

void emu_exit()
{
  if (emu_memory)
  {
    free(emu_memory);
  }
}
