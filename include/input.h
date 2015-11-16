#ifndef INPUT_H
#define INPUT_H

#include "conf.h"

#include <asmtypes.h>

#ifdef INPUT_OPTIMIZED
#define INPUT_KEYTEST(K) _keytest_optimized(K)
#else
#define INPUT_KEYTEST(K) _keytest(K)
#endif

void input_update();

BOOL input_keys[0x10];

#endif
