#ifndef SCREEN_H
#define SCREEN_H

#include <asmtypes.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_MEMSIZE ((SCREEN_WIDTH * SCREEN_HEIGHT) / 8)

#define SCREEN_OFFSET64(x,y) ((y<<2)+(y<<2)+(x>>3))
#define SCREEN_ADDR64(p,x,y) ((unsigned char*)(p)+SCREEN_OFFSET64(x,y))
#define SCREEN_MASK64(x) ((unsigned char)(0x80 >> ((x)&7)))
#define SCREEN_GET64(p,x,y) (!(!(*SCREEN_ADDR64(p,x,y) & SCREEN_MASK64(x))))
#define SCREEN_SET64(p,x,y) (*SCREEN_ADDR64(p,x,y) |= SCREEN_MASK64(x))
#define SCREEN_CLR64(p,x,y) (*SCREEN_ADDR64(p,x,y) &= ~SCREEN_MASK64(x))
#define SCREEN_XOR64(p,x,y) (*SCREEN_ADDR64(p,x,y) ^= SCREEN_MASK64(x))

#define SCREEN_OFFSET240(x,y) ((y<<5)-(y<<1)+(x>>3))
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

#endif
