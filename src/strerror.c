#include "strerror.h"

#include <alloc.h>
#include <files.h>


const char *strerror_fs(enum FileStatusEnum status)
{
  switch (status)
  {
    case FS_OK:
      return "OK"; break;
    case FS_EOF:
      return "Unexpected EOF"; break;
    case FS_ERROR:
      return "File may be locked or not of correct data-type"; break;
    case FS_BAD_NAME:
      return "File name is invalid"; break;
    case FS_MEMORY:
      return "Out of memory"; break;
    case FS_NOT_FOUND:
      return "File does not exist"; break;
    default:
      return "Unknown error"; break;
  }
}
