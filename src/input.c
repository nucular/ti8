#include "input.h"

#include <compat.h>
#include <kbd.h>

void input_update()
{
  BEGIN_KEYTEST
  input_keys[0x0] = _keytest_optimized(RR_2);
  input_keys[0x1] = _keytest_optimized(RR_PAREN_OPEN);
  input_keys[0x2] = _keytest_optimized(RR_PAREN_CLOSE);
  input_keys[0x3] = _keytest_optimized(RR_COMMA);
  input_keys[0x4] = _keytest_optimized(RR_7);
  input_keys[0x5] = _keytest_optimized(RR_8);
  input_keys[0x6] = _keytest_optimized(RR_9);
  input_keys[0x7] = _keytest_optimized(RR_4);
  input_keys[0x8] = _keytest_optimized(RR_4);
  input_keys[0x9] = _keytest_optimized(RR_6);
  input_keys[0xA] = _keytest_optimized(RR_1);
  input_keys[0xB] = _keytest_optimized(RR_3);
  input_keys[0xC] = _keytest_optimized(RR_DIVIDE);
  input_keys[0xD] = _keytest_optimized(RR_MULTIPLY);
  input_keys[0xE] = _keytest_optimized(RR_MINUS);
  input_keys[0xF] = _keytest_optimized(RR_PLUS);
  END_KEYTEST
}
