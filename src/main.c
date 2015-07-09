#include "main.h"

#include <tigcclib.h>
#include <args.h>
#include <estack.h>

void _main(void)
{
  ESI argptr;
  const char *path;

  InitArgPtr(argptr);
  if (GetArgType(argptr) != STR_TAG)
    return;
  path = GetStrnArg(argptr);
}
