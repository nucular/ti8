#include "conf.h"
#include "main.h"

#include <stdlib.h>
#include <args.h>
#include <asmtypes.h>
#include <dialogs.h>
#include <estack.h>
#include <files.h>
#include <kbd.h>
#include <stdio.h>
#include <system.h>
#include "strerror.h"

#include "input.h"
#include "screen.h"
#include "emu.h"


void _main(void)
{
  ESI argptr;
  const char *filename;
  FILES *file;
  unsigned short filesize;
  enum FileStatusEnum status;

  // Get the file name from the argument list
  InitArgPtr(argptr);
  if (ArgCount() != 1)
  {
    fatal("Invalid arguments", "Expected one argument of type string");
    return;
  }
  if (GetArgType(argptr) != STR_TAG) {
    fatal("Invalid arguments", "Expected one argument of type string");
    return;
  }
  filename = GetStrnArg(argptr);

  // Try to open the file (thank god we only have to do this once)
  status = FAccess(filename, FM_READ, "CH8");
  if (status != FS_OK) fatal("Could not access program", strerror_fs(status));
  file = malloc(sizeof(FILES));
  if (!file) fatal("Could not open program", "Out of memory");
  status = FOpen(filename, file, FM_READ, "CH8");
  if (status != FS_OK)
  {
    free(file);
    fatal("Could not open program", strerror_fs(status));
  }

  // Get and check the file size
  filesize = FGetSize(file);
  if (filesize >= EMU_PROGSIZE)
  {
    FClose(file); free(file);
    fatal("Program is invalid", "Too large for CHIP-8");
    return;
  }

  // We're basically safe now: initiate everything
  main_init();

  // Read the file into memory
  status = FRead((unsigned short *)(emu_memory + EMU_PROGSTART), filesize, file);
  if (status != FS_OK)
  {
    FClose(file); free(file);
    fatal("Program is invalid", strerror_fs(status));
    return;
  }

  FClose(file);
  free(file);

  // Main loop
  void *keyqueue = kbd_queue();
  unsigned short key;
  SCREEN_SET64(screen_mem,10,10);
  SCREEN_SET64(screen_mem,20,10);
  SCREEN_SET64(screen_mem,10,20);
  SCREEN_SET64(screen_mem,20,20);
  while (emu_running)
  {
    emu_cycle();
    screen_update();
    if (!OSdequeue(&key, keyqueue))
    {
      if (key == KEY_QUIT || key == KEY_ESC)
        emu_running = FALSE;
    }
  }

  main_exit();
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

void fatal(const char* title, const char *msg)
{
  main_exit();
  DlgMessage(title, msg, BT_OK, BT_NONE);
  exit(1);
}
