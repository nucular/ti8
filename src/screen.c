#include "screen.h"

#include <tigcclib.h>


void screen_init()
{
  ClrScr();
  screen_ptr = malloc(SCREEN_MEMSIZE);
  if (!screen_ptr)
  {
    fatal("Could not initialize screen: Out of memory");
  }
}

void screen_exit()
{
  if (screen_ptr)
  {
    free(screen_ptr);
  }
}
