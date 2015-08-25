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
    fatal("Could not initialize screen", "Out of memory");
  screen_lookup = malloc(0x101);
  if (!screen_lookup)
    fatal("Could not initialize screen", "Out of memory");
  unsigned short i;
  for (i = 0; i < 0x100; i++)
  {
    screen_lookup[i] = ((i >> 0) & 1) << 0
      | ((i >> 0) & 1) << 1
      | ((i >> 1) & 1) << 2
      | ((i >> 1) & 1) << 3
      | ((i >> 2) & 1) << 4
      | ((i >> 2) & 1) << 5
      | ((i >> 3) & 1) << 6
      | ((i >> 3) & 1) << 7
      | ((i >> 4) & 1) << 8
      | ((i >> 4) & 1) << 9
      | ((i >> 5) & 1) << 10
      | ((i >> 5) & 1) << 11
      | ((i >> 6) & 1) << 12
      | ((i >> 6) & 1) << 13
      | ((i >> 7) & 1) << 14
      | ((i >> 7) & 1) << 15;
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
    for (x = 0; x < SCREEN_PITCH; x++)
    {
      unsigned char a = *SCREEN_ADDR64(screen_mem, x<<3, y);
      unsigned short b = screen_lookup[a];
      *SCREEN_ADDR240(LCD_MEM, x<<4, y<<1) = b;
      *SCREEN_ADDR240(LCD_MEM, x<<4, (y<<1)+1) = b;
    }
  }
}

void screen_exit()
{
  if (screen_mem) free(screen_mem);
  if (screen_lookup) free(screen_lookup);
}
