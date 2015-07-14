#include "emu.h"

#include <alloc.h>
#include <asmtypes.h>
#include <intr.h>
#include <mem.h>

#include "main.h"


void emu_init()
{
  emu_memory = malloc(EMU_MEMSIZE);
  if (!emu_memory) fatal("Could not initialize emulator", "Out of memory");
  emu_pc = emu_memory + EMU_PROGSTART;
  emu_stack_top = 0;
  emu_running = TRUE;

  // Load the font set
  memcpy(emu_memory, emu_font, 80);

  // Set up the timers
  emu_old_int5 = GetIntVec(AUTO_INT_5);
  SetIntVec(AUTO_INT_5, emu_int5);
  PRG_setRate(0);
  emu_delaytimer = 0;
  emu_soundtimer = 0;
}

void emu_exit()
{
  if (emu_memory) free(emu_memory);
  if (emu_old_int5) SetIntVec(AUTO_INT_5, emu_old_int5);
  PRG_setRate(1);
}

void emu_cycle()
{
  unsigned short op = *emu_pc;
  
  unsigned char b1 = op >> 8;
  unsigned char b2 = op & 0xFF;
  unsigned char n1 = b1 >> 4;
  unsigned char n2 = b1 & 0xF;
  unsigned char n3 = b2 >> 4;
  unsigned char n4 = b2 & 0xF;

  //emu_pc++;
  if (emu_pc >= (emu_memory + EMU_MEMSIZE))
    emu_running = FALSE;
}

DEFINE_INT_HANDLER(emu_int5)
{
  if (emu_delaytimer >= 2)
    emu_delaytimer -= 2;
  if (emu_soundtimer >= 2)
    emu_soundtimer -= 2;
}
