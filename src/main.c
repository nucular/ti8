#include "conf.h"
#include "main.h"

#include <tigcclib.h>
#include <args.h>
#include <estack.h>

#include "cpu.h"
#include "input.h"
#include "screen.h"
#include "emu.h"


void _main(void)
{
  ESI argptr;
  const char *filename;
  FILES *file;
  unsigned short filesize;

  // Get the file name from the argument list
  InitArgPtr(argptr);
  if (ArgCount() != 1)
  {
    fatal("Invalid number of arguments");
    return;
  }
  if (GetArgType(argptr) != STR_TAG) {
    fatal("Invalid argument type: Expected string");
    return;
  }
  filename = GetStrnArg(argptr);

  // Try to open the file (thank god we only have to do this once)
  switch (FAccess(filename, FM_READ, "CHP8"))
  {
    case FS_ERROR:
      fatal("Could not open program: File is locked or not CHP8"); return; break;
    case FS_NOT_FOUND:
      fatal("Could not open program: File not found"); return; break;
    case FS_BAD_NAME:
      fatal("Could not open program: File name is invalid"); return; break;
  }
  file = malloc(sizeof(FILES));
  if (!file)
  {
    fatal("Could not open program: Out of memory");
    return;
  }
  switch (FOpen(filename, file, FM_READ, "CHP8"))
  {
    case FS_ERROR:
      free(file);
      fatal("Could not open program: File locked or not CHP8");
      return; break;
    case FS_BAD_NAME:
      free(file);
      fatal("Could not open program: File name is invalid");
      return; break;
    case FS_MEMORY:
      free(file);
      fatal("Could not open program: Out of memory");
      return; break;
  }

  // Get and check the file size
  filesize = FGetSize(file);
  if (filesize >= EMU_MEMSIZE)
  {
    FClose(file); free(file);
    fatal("Program is invalid: Too large");
    return;
  }

  // We're basically safe now: initiate everything
  main_init();

  // Read the file into memory
  if (FRead(emu_memory + EMU_PROGSTART, filesize, file) == FS_EOF)
  {
    FClose(file); free(file);
    fatal("Program is invalid: Unexpected EOF (this really shouldn't happen!)");
    return;
  }

  FClose(file);
  free(file);
}

void main_init()
{
  screen_init();
  emu_init();
}

void main_exit()
{
  emu_exit();
  screen_exit();
}

void fatal(const char *msg)
{
  main_exit();
  DlgMessage("FATAL ERROR", msg, BT_OK, BT_NONE);
  exit(1);
}
