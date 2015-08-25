#include "conf.h"
#include "main.h"

#include <stdlib.h>
#include <args.h>
#include <asmtypes.h>
#include <compat.h>
#include <dialogs.h>
#include <estack.h>
#include <files.h>
#include <kbd.h>
#include <statline.h>
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
  status = FRead((unsigned char *)(emu_mem + EMU_PROGSTART), filesize, file);
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
  while (emu_running)
  {
    if (!OSdequeue(&key, keyqueue))
    {
      switch (key)
      {
        case KEY_QUIT:
        case KEY_ESC:
          emu_running = FALSE;
          break;

        case KEY_APPS:
          emu_setpaused(!emu_paused);
          break;

        case KEY_MODE:
          if (emu_stepthrough)
          {
            emu_stepthrough = FALSE;
            emu_setpaused(FALSE);
          }
          else
          {
            emu_stepthrough = TRUE;
            emu_setpaused(TRUE);
            emu_printdebug();
          }
          break;

        case KEY_F1: emu_regsel = 0x1; emu_printdebug(); break;
        case KEY_F2: emu_regsel = 0x2; emu_printdebug(); break;
        case KEY_F3: emu_regsel = 0x3; emu_printdebug(); break;
        case KEY_F4: emu_regsel = 0x4; emu_printdebug(); break;
        case KEY_F5: emu_regsel = 0x5; emu_printdebug(); break;
        case KEY_F6: emu_regsel = 0x6; emu_printdebug(); break;
        case KEY_F7: emu_regsel = 0x7; emu_printdebug(); break;
        case KEY_F8: emu_regsel = 0x8; emu_printdebug(); break;

        case KEY_CLEAR:
          if (emu_stepthrough)
          {
            if (emu_regsel < EMU_REGCOUNT)
              emu_reg[emu_regsel] = 0;
            else if (emu_regsel == EMU_REGCOUNT)
              emu_i = 0;
            else
              emu_pc = emu_mem + EMU_PROGSTART;
            emu_printdebug();
          }
          break;
      }

      // Pseudoconstants...
      if (key == KEY_UP && emu_stepthrough)
      {
        if (emu_regsel == 0) emu_regsel = EMU_REGCOUNT + 1;
        else emu_regsel--;
        emu_printdebug();
      }
      else if (key == KEY_DOWN && emu_stepthrough)
      {
        if (emu_regsel == EMU_REGCOUNT + 1) emu_regsel = 0;
        else emu_regsel++;
        emu_printdebug();
      }
      else if (key == KEY_LEFT && emu_stepthrough)
      {
        if (emu_regsel < EMU_REGCOUNT)
          emu_reg[emu_regsel]--;
        else if (emu_regsel == EMU_REGCOUNT)
          emu_i--;
        else
          emu_pc -= 2;
        emu_printdebug();
      }
      else if (key == KEY_RIGHT && emu_stepthrough)
      {
        if (emu_regsel < EMU_REGCOUNT)
          emu_reg[emu_regsel]++;
        else if (emu_regsel == EMU_REGCOUNT)
          emu_i++;
        else
          emu_pc += 2;
        emu_printdebug();
      }
    }

    if (!emu_paused)
    {
      input_update();
      emu_cycle();
    }
    if (screen_dirty)
    {
      screen_update();
      screen_dirty = FALSE;
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
