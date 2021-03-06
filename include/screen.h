#ifndef SCREEN_H
#define SCREEN_H

#include "conf.h"

#include <asmtypes.h>
#include <compat.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_PITCH 8
#define SCREEN_MEMSIZE 256 // (SCREEN_PITCH * SCREEN_HEIGHT)

#ifdef SCREEN_CENTER
#ifdef SCREEN_SCALE2X
#define SCREEN_OFFX ((LCD_WIDTH/2)-SCREEN_WIDTH)
#define SCREEN_OFFY ((LCD_HEIGHT/2)-SCREEN_HEIGHT)
#else
#define SCREEN_OFFX ((LCD_WIDTH/2)-(SCREEN_WIDTH/2))
#define SCREEN_OFFY ((LCD_HEIGHT/2)-(SCREEN_HEIGHT/2))
#endif
#else
#define SCREEN_OFFX 0
#define SCREEN_OFFY 0
#endif

#define SCREEN_OFFSET64(x,y) (((y)<<2)+((y)<<2)+((x)>>3))
#define SCREEN_ADDR64(p,x,y) ((unsigned char*)(p)+SCREEN_OFFSET64(x,y))
#define SCREEN_MASK64(x) ((unsigned char)(0x80 >> ((x)&7)))
#define SCREEN_GET64(p,x,y) (!(!(*SCREEN_ADDR64(p,x,y) & SCREEN_MASK64(x))))
#define SCREEN_SET64(p,x,y) (*SCREEN_ADDR64(p,x,y) |= SCREEN_MASK64(x))
#define SCREEN_CLR64(p,x,y) (*SCREEN_ADDR64(p,x,y) &= ~SCREEN_MASK64(x))
#define SCREEN_XOR64(p,x,y) (*SCREEN_ADDR64(p,x,y) ^= SCREEN_MASK64(x))

#define SCREEN_OFFSET240(x,y) (((y)<<5)-((y)<<1)+((x)>>3))
#define SCREEN_ADDR240(p,x,y) ((unsigned char*)(p)+SCREEN_OFFSET240(x,y))
#define SCREEN_MASK240(x) ((unsigned char)(0x80 >> ((x)&7)))
#define SCREEN_GET240(p,x,y) (!(!(*SCREEN_ADDR240(p,x,y) & SCREEN_MASK240(x))))
#define SCREEN_SET240(p,x,y) (*SCREEN_ADDR240(p,x,y) |= SCREEN_MASK240(x))
#define SCREEN_CLR240(p,x,y) (*SCREEN_ADDR240(p,x,y) &= ~SCREEN_MASK240(x))
#define SCREEN_XOR240(p,x,y) (*SCREEN_ADDR240(p,x,y) ^= SCREEN_MASK240(x))

void screen_init();
void screen_exit();
void screen_clear();
void screen_update();

BOOL screen_dirty;
unsigned char *screen_mem;
#ifdef SCREEN_SCALE2X
unsigned short *screen_lookup;
#endif

#endif
