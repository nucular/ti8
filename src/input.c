#include "input.h"

#include <compat.h>
#include <kbd.h>

void input_update()
{
#ifdef INPUT_OPTIMIZED
  BEGIN_KEYTEST
#endif
  input_keys[0x0] = INPUT_KEYTEST(RR_DOT);
  input_keys[0x1] = INPUT_KEYTEST(RR_7);
  input_keys[0x2] = INPUT_KEYTEST(RR_8);
  input_keys[0x3] = INPUT_KEYTEST(RR_9);
  input_keys[0x4] = INPUT_KEYTEST(RR_4);
  input_keys[0x5] = INPUT_KEYTEST(RR_5);
  input_keys[0x6] = INPUT_KEYTEST(RR_6);
  input_keys[0x7] = INPUT_KEYTEST(RR_1);
  input_keys[0x8] = INPUT_KEYTEST(RR_2);
  input_keys[0x9] = INPUT_KEYTEST(RR_3);
  input_keys[0xA] = INPUT_KEYTEST(RR_0);
  input_keys[0xB] = INPUT_KEYTEST(RR_NEGATE);
  input_keys[0xC] = INPUT_KEYTEST(RR_MULTIPLY);
  input_keys[0xD] = INPUT_KEYTEST(RR_MINUS);
  input_keys[0xE] = INPUT_KEYTEST(RR_PLUS);
  input_keys[0xF] = INPUT_KEYTEST(RR_ENTER);
#ifdef INPUT_OPTIMIZED
  END_KEYTEST
#endif
}
