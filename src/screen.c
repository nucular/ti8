#include "screen.h"

#include <stdlib.h>
#include <graph.h>

#include "main.h"


void screen_init()
{
  ClrScr();
  screen_ptr = malloc(SCREEN_MEMSIZE);
  if (!screen_ptr)
  {
    fatal("Could not initialize screen", "Out of memory");
  }
}

void screen_exit()
{
  if (screen_ptr)
  {
    free(screen_ptr);
  }
}
