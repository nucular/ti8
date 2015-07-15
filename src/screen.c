#include "screen.h"

#include <stdlib.h>
#include <asmtypes.h>
#include <graph.h>
#include <mem.h>

#include "main.h"


void screen_init()
{
  ClrScr();
  MoveTo(0, 0); // for printf
  screen_mem = malloc(SCREEN_MEMSIZE);
  if (!screen_mem)
  {
    fatal("Could not initialize screen", "Out of memory");
  }
  screen_clear();
}

void screen_clear()
{
  memset(screen_mem, 0, SCREEN_MEMSIZE);
  screen_dirty = TRUE;
}

void screen_update()
{
  unsigned short x, y;
  for (y = 0; y < SCREEN_HEIGHT; y++)
  {
    for (x = 0; x < SCREEN_WIDTH; x++)
    {
      *SCREEN_ADDR240(LCD_MEM, x, y) = *SCREEN_ADDR64(screen_mem, x, y);
    }
  }
}

void screen_exit()
{
  if (screen_mem) free(screen_mem);
}
