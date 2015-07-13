#ifndef STRERROR_H
#define STRERROR_H

#include <alloc.h>
#include <files.h>


const char *strerror_fs(enum FileStatusEnum status);

#endif
