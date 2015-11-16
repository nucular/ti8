#include "emu.h"

#include <stdlib.h>
#include <alloc.h>
#include <asmtypes.h>
#include <compat.h>
#include <graph.h>
#include <intr.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "input.h"
#include "screen.h"


void emu_init()
{
  emu_mem = malloc(EMU_MEMSIZE);
  if (!emu_mem) fatal("Could not initialize emulator", "Out of memory");

  emu_pc = emu_mem + EMU_PROGSTART;
  emu_i = 0;

  // Load the font set
  memcpy(emu_mem, emu_font, 80);

  // Set up the timers
  emu_old_int5 = GetIntVec(AUTO_INT_5);
  SetIntVec(AUTO_INT_5, emu_int5);
  PRG_setRate(0);
  emu_delaytimer = 0;
  emu_soundtimer = 0;

  // Clear registers and stack
  unsigned short i;
  for (i = 0; i < EMU_REGCOUNT; i++) emu_reg[i] = 0;
  for (i = 0; i < EMU_STACKSIZE; i++) emu_stack[i] = 0;
  emu_stack_top = 0;
  emu_regsel = 0;

  randomize();
  emu_paused = FALSE;
  emu_stepthrough = FALSE;
  emu_running = TRUE;
}

void emu_exit()
{
  if (emu_mem) free(emu_mem);
  if (emu_old_int5) SetIntVec(AUTO_INT_5, emu_old_int5);
  PRG_setRate(1);
}

void emu_illegal()
{
  FontSetSys(F_6x8);
  printf_xy(0, 127 - 8,
    "[%03X] %04X: ???          ",
    (unsigned short)(emu_pc - emu_mem),
    (*emu_pc << 8) | (*(emu_pc + 1))
  );
  emu_setpaused(TRUE);
}

void emu_cycle()
{
  unsigned char b1 = *emu_pc;
  unsigned char b2 = *(emu_pc + 1);
  unsigned char n1 = b1 >> 4;
  unsigned char n2 = b1 & 0xF;
  unsigned char n3 = b2 >> 4;
  unsigned char n4 = b2 & 0xF;

  BOOL advance = TRUE; // advance PC?

  unsigned short i;
  unsigned char l, x, y, lx, ly;
  BOOL hit;

  switch (n1)
  {
    case 0x0:
      switch (b2)
      {
        case 0xE0:
          if (n2 == 0x0) // 0x00E0: Clear screen
            screen_clear();
          else emu_illegal();
          break;

        case 0xEE:
          if (n2 == 0x0 && emu_stack_top > 0) // 0x00EE: Return
          {
            emu_pc = emu_stack[--emu_stack_top];
            advance = FALSE;
          }
          else emu_illegal();
          break;

        default:
          emu_illegal();
          break;
      }
      break;

    case 0x1: // 0x1NNN: Jump to $NNN
      emu_pc = emu_mem + ((n2 << 8) | b2);
      advance = FALSE; break;

    case 0x2: // 0x2NNN: Call $NNN
      emu_stack[emu_stack_top++] = emu_pc + 2;
      emu_pc = emu_mem + ((n2 << 8) | b2);
      advance = FALSE; break;

    case 0x3: // 0x3XNN: Skip if %X == $NN
      if (emu_reg[n2] == b2)
        emu_pc += 4;
      else
        emu_pc += 2;
      advance = FALSE; break;

    case 0x4: // 0x4XNN: Skip if %X != $NN
      if (emu_reg[n2] != b2)
        emu_pc += 4;
      else
        emu_pc += 2;
      advance = FALSE; break;

    case 0x5:
      if (n4 == 0) // 0x5XY0: Skip if %X == %Y
      {
        if (emu_reg[n2] == emu_reg[n3])
          emu_pc += 4;
        else
          emu_pc += 2;
        advance = FALSE;
      }
      else emu_illegal();
      break;

    case 0x6: // 0x6XNN: %X = $NN
      emu_reg[n2] = b2;
      break;

    case 0x7: // 0x7XNN: %X += $NN
#ifndef EMU_REALISTIC
      emu_reg[0xF] = (emu_reg[n2] > (0xFF - b2)); // carry
#endif
      emu_reg[n2] += b2;
      break;

    case 0x8:
      switch (n4)
      {
        case 0x0: // 0x8XY0: %X = %Y
          emu_reg[n2] = emu_reg[n3];
          break;

        case 0x1: // 0x8XY1: %X |= %Y
          emu_reg[n2] |= emu_reg[n3];
          break;

        case 0x2: // 0x8XY2: %X &= %Y
          emu_reg[n2] &= emu_reg[n3];
          break;

        case 0x3: // 0x8XY3: %X ^= %Y
          emu_reg[n2] ^= emu_reg[n3];
          break;

        case 0x4: // 0x8XY4: %X += %Y
          emu_reg[0xF] = (emu_reg[n2] > (0xFF - emu_reg[n3])); // carry
          emu_reg[n2] += emu_reg[n3];
          break;

        case 0x5: // 0x8XY5: %X -= %Y
          emu_reg[0xF] = (emu_reg[n2] < emu_reg[n3]); // borrow
          emu_reg[n2] -= emu_reg[n3];
          break;

        case 0x6: // 0x8XY6: %X >>= 1
#ifdef EMU_REALISTIC
          emu_reg[0xF] = emu_reg[n3] & 0x1;
          emu_reg[n2] = emu_reg[n3] >> 1;
#else
          emu_reg[0xF] = emu_reg[n2] & 0x1;
          emu_reg[n2] >>= 1;
#endif
          break;

        case 0x7: // 0x8XY7: %X = %Y - %X
          emu_reg[0xF] = (emu_reg[n3] < emu_reg[n2]); // borrow
          emu_reg[n2] = emu_reg[n3] - emu_reg[n2];
          break;

        case 0xE: // 0x8XYE: %X <<= 1
#ifdef EMU_REALISTIC
          emu_reg[0xF] = emu_reg[n3] >> 0x7;
          emu_reg[n2] = emu_reg[n3] >> 1;
#else
          emu_reg[0xF] = emu_reg[n2] >> 0x7;
          emu_reg[n2] <<= 1;
#endif
          break;

        default:
          emu_illegal();
          break;
      }
      break;

    case 0x9:
      if (n4 == 0) // 0x9XY0: Skip if %X != %Y
      {
        if (emu_reg[n2] != emu_reg[n3])
          emu_pc += 4;
        else
          emu_pc += 2;
        advance = FALSE;
      }
      else emu_illegal();
      break;

    case 0xA: // 0xANNN: %I = $NNN
      emu_i = (n2 << 8) | b2;
      break;

    case 0xB: // 0xBNNN: Jump to $NNN + %0
      emu_pc = emu_mem + ((n2 << 8) | b2) + emu_reg[0];
      advance = FALSE; break;

    case 0xC: // 0xCXNN: %X = rand() & $NN
      emu_reg[n2] = random(0xFF) & b2;
      break;

    case 0xD: // 0xDXYN: Draw sprite from %I at %X,%Y with a height of $N
      hit = FALSE;
      for (ly = 0; ly < n4; ly++)
      {
        l = emu_mem[emu_i + ly];
        y = emu_reg[n3] + ly;
        for (lx = 0; lx < 8; lx++)
        {
          x = emu_reg[n2] + lx;
          if (l & (0x80 >> lx))
          {
            if (SCREEN_GET64(screen_mem, x, y))
              hit = TRUE;
            SCREEN_XOR64(screen_mem, x, y);
          }
        }
      }
      screen_dirty = TRUE;
      emu_reg[0xF] = hit;
      break;

    case 0xE:
      switch (b2)
      {
        case 0x9E: // 0xEX9E: Skip if key in %VX is pressed
          if (input_keys[emu_reg[b2]])
            emu_pc += 4;
          else
            emu_pc += 2;
          advance = FALSE; break;

        case 0xA1: // 0xEXA1: Skip if key in %VX is not pressed
          if (input_keys[emu_reg[b2]])
            emu_pc += 2;
          else
            emu_pc += 4;
          advance = FALSE; break;

        default:
          emu_illegal(); break;
      }
      break;

    case 0xF:
      switch (b2)
      {
        case 0x07: // 0xFX07: %VX = %delay
          emu_reg[n2] = emu_delaytimer;
          break;

        case 0x0A: // 0xFX0A: Wait for key, set %VX to it
          advance = FALSE;
          for (i = 0; i < 0xF; i++) {
            if (input_keys[i]) {
              emu_reg[n2] = i;
              advance = TRUE;
            }
          }
          break;

        case 0x15: // 0xFX15: %delay = %VX
          emu_delaytimer = emu_reg[n2];
          break;

        case 0x18: // 0xFX15: %sound = %VX
          emu_soundtimer = emu_reg[n2];
          break;

        case 0x1E: // 0xFX1E: %I += %X
#ifdef EMU_REALISTIC
          emu_reg[0xF] = (emu_i > (EMU_MEMSIZE - emu_reg[n2]));
#endif
          emu_i = (emu_i + emu_reg[n2]) % EMU_MEMSIZE;
          break;

        case 0x29: // 0xFX29: %I = address of char for %X
          emu_i = emu_reg[n2] * 0x5;
          break;

        case 0x33: // 0xFX33: Store binary of %X in %I to %I+2
          emu_mem[emu_i] = emu_reg[n2] / 100;
          emu_mem[emu_i + 1] = (emu_reg[n2] / 10) % 10;
          emu_mem[emu_i + 2] = (emu_reg[n2] % 100) % 10;
          break;

        case 0x55: // 0xFX55: Store %0 to %X in memory starting at %I
          for (i = 0; i <= n2; i++)
            emu_mem[emu_i + i] = emu_reg[i];
#ifdef EMU_REALISTIC
          emu_i += n2 + 1;
#endif
          break;

        case 0x65: // 0xFX65: Fill %0 to %X with the memory starting at %I
          for (i = 0; i <= n2; i++)
            emu_reg[i] = emu_mem[emu_i + i];
#ifdef EMU_REALISTIC
          emu_i += n2 + 1;
#endif
          break;

        default:
          emu_illegal();
          break;
      }
      break;

    default:
      emu_illegal();
      break;
  }

  if (advance)
    emu_pc += 2;
  if (emu_pc >= (emu_mem + EMU_MEMSIZE))
    emu_running = FALSE;

  if (emu_stepthrough)
  {
    emu_printdebug();
    emu_setpaused(TRUE);
  }
}

void emu_setpaused(BOOL paused)
{
  if (paused)
  {
    FontSetSys(F_4x6);
    printf_xy(LCD_WIDTH - 9, LCD_HEIGHT - 7, "||");
    emu_paused = TRUE;
  }
  else
  {
    clrscr();
    screen_update();
    emu_paused = FALSE;
  }
}

void emu_printdebug()
{
  // Program counter and next instruction with mnemonic
  char buf[255] = "\x00";
  FontSetSys(F_6x8);
  printf_xy(12, 127 - 17, emu_regsel == EMU_REGCOUNT + 1 ? "v" : "  ");
  printf_xy(0, 127 - 8,
    "[%03X] %04X: %s          ",
    (unsigned short)(emu_pc - emu_mem),
    (*emu_pc << 8) | (*(emu_pc + 1)),
    emu_findmne(emu_pc, buf)
  );

  // List all registers including I
  FontSetSys(F_4x6);
  unsigned char i;
  for (i = 0; i < EMU_REGCOUNT; i++)
  {
    printf_xy(LCD_WIDTH - 24, i * 7 + 1, i == emu_regsel ? ">" : "  ");
    printf_xy(LCD_WIDTH - 20, i * 7 + 1,
      "V%01X %02X",
      i,
      emu_reg[i]
    );
  }
  printf_xy(LCD_WIDTH - 24, i * 7 + 1, i == emu_regsel ? ">" : "  ");
  printf_xy(LCD_WIDTH - 20, i * 7 + 1,
    "I %03X",
    emu_i
  );
}

char* emu_findmne(unsigned char* ptr, char* buf)
{
  unsigned char b1 = *ptr;
  unsigned char b2 = *(ptr + 1);
  unsigned char n1 = b1 >> 4;
  unsigned char n2 = b1 & 0xF;
  unsigned char n3 = b2 >> 4;
  unsigned char n4 = b2 & 0xF;

  // Parse the bytes again to find the mnemonic
  switch (n1)
  {
    case 0x0:
      switch (b2)
      {
        case 0xE0:
          if (n2 == 0x0) // 0x00E0: Clear screen
            strcpy(buf, "cls");
          else strcpy(buf, "???");
          break;
        case 0xEE:
          if (n2 == 0) // 0x00EE: Return
            strcpy(buf, "rts");
          else strcpy(buf, "???");
          break;
        default:
          strcpy(buf, "???"); break;
      }
      break;
    case 0x1: // 0x1NNN: Jump to $NNN
      sprintf(buf, "jmp %03X", (n2 << 8) | b2); break;
    case 0x2: // 0x2NNN: Call $NNN
      sprintf(buf, "jsr %03X", (n2 << 8) | b2); break;
    case 0x3: // 0x3XNN: Skip if %X == $NN
      sprintf(buf, "skeq V%01X %02X", n2, b2); break;
    case 0x4: // 0x4XNN: Skip if %X != $NN
      sprintf(buf, "skne V%01X %02X", n2, b2); break;
    case 0x5:
      if (n4 == 0) // 0x5XY0: Skip if %X == %Y
      {
        sprintf(buf, "skeq V%01X V%01X", n2, n3); break;
      }
      else strcpy(buf, "???");
      break;
    case 0x6: // 0x6XNN: %X = $NN
      sprintf(buf, "mov V%01X %02X", n2, b2); break;
    case 0x7: // 0x7XNN: %X += $NN
      sprintf(buf, "add V%01X %02X", n2, b2); break;
    case 0x8:
      switch (n4)
      {
        case 0x0: // 0x8XY0: %X = %Y
          sprintf(buf, "mov V%01X V%01X", n2, n3); break;
        case 0x1: // 0x8XY1: %X |= %Y
          sprintf(buf, "or V%01X V%01X", n2, n3); break;
        case 0x2: // 0x8XY2: %X &= %Y
          sprintf(buf, "and V%01X V%01X", n2, n3); break;
        case 0x3: // 0x8XY3: %X ^= %Y
          sprintf(buf, "xor V%01X V%01X", n2, n3); break;
        case 0x4: // 0x8XY4: %X += %Y
          sprintf(buf, "add V%01X V%01X", n2, n3); break;
        case 0x5: // 0x8XY5: %X -= %Y
          sprintf(buf, "sub V%01X V%01X", n2, n3); break;
        case 0x6: // 0x8XY6: %X >>= 1
#ifdef EMU_REALISTIC
          sprintf(buf, "shr V%01X V%01X", n3, n2); break;
#else
          sprintf(buf, "shr V%01X", n2); break;
#endif
        case 0x7: // 0x8XY7: %X = %Y - %X
          sprintf(buf, "rsb V%01X V%01X", n2, n3); break;
        case 0xE: // 0x8XYE: %X <<= 1
#ifdef EMU_REALISTIC
          sprintf(buf, "shl V%01X V%01X", n3, n2); break;
#else
          sprintf(buf, "shl V%01X", n2); break;
#endif
        default:
          strcpy(buf, "???"); break;
      }
      break;
    case 0x9:
      sprintf(buf, "skne V%01X V%01X", n2, n3); break;
    case 0xA: // 0xANNN: %I = $NNN
      sprintf(buf, "mvi %03X", (n2 << 8) | b2); break;
    case 0xB: // 0xBNNN: Jump to $NNN + %0
      sprintf(buf, "jmi %03X V0", (n2 << 8) | b2); break;
    case 0xC: // 0xCXNN: %X = rand() & $NN
      sprintf(buf, "rand V%01X %02X", n2, b2); break;
    case 0xD: // 0xDXYN: Draw sprite from %I at $X,$Y with a height of $N
      sprintf(buf, "sprite V%01X V%01X %01X", n2, n3, n4); break;
    case 0xE:
      switch (b2)
      {
        case 0x9E: // 0xEX9E: Skip if key in %VX is pressed
          sprintf(buf, "skpr V%01X", n2); break;
        case 0xA1: // 0xEXA1: Skip if key in %VX is not pressed
          sprintf(buf, "skup V%01X", n2); break;
        default:
          strcpy(buf, "???"); break;
      }
    case 0xF:
      switch (b2)
      {
        case 0x07: // 0xFX07: %VX = %delay
          sprintf(buf, "gdel V%01X", n2); break;
        case 0x0A: // 0xFX0A: Wait for key, set %VX to it
          sprintf(buf, "key V%01X", n2); break;
        case 0x15: // 0xFX15: %delay = %VX
          sprintf(buf, "sdel V%01X", n2); break;
        case 0x18: // 0xFX15: %sound = %VX
          sprintf(buf, "ssnd V%01X", n2); break;
        case 0x1E: // 0xFX1E: %I += %X
          sprintf(buf, "adi V%01X", n2); break;
        case 0x29: // 0xFX29: %I = address of char for %X
          sprintf(buf, "font V%01X", n2); break;
        case 0x33: // 0xFX33: Store binary of %X in %I to %I+2
          sprintf(buf, "bcd V%01X", n2); break;
        case 0x55: // 0xFX55: Store %0 to %X in memory starting at %I
          sprintf(buf, "str V0 V%01X", n2); break;
        case 0x65: // 0xFX65: Fill %0 to %X with the memory starting at %I
          sprintf(buf, "ldr V0 V%01X", n2); break;
        default:
          strcpy(buf, "???"); break;
      }
      break;
    default:
      strcpy(buf, "???"); break;
  }

  return buf;
}

DEFINE_INT_HANDLER(emu_int5)
{
  if (emu_delaytimer > 0)
    emu_delaytimer -= 1;
  if (emu_soundtimer > 0)
    emu_soundtimer -= 1;
}
