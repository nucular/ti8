#ifndef SCREEN_H
#define SCREEN_H

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_MEMSIZE ((SCREEN_WIDTH * SCREEN_HEIGHT) / 8)

#define SCREEN_OFFSET64(x,y) ((y<<2)+(y<<2)+(x>>3))
#define SCREEN_ADDR64(x,y) (screen_ptr+SCREEN_OFFSET64(x,y))
#define SCREEN_MASK64(x) ((unsigned char)(0x80 >> ((x)&7)))
#define SCREEN_GET64(x,y) (!(!(*SCREEN_ADDR64(x,y) & SCREEN_MASK64(x))))
#define SCREEN_SET64(x,y) (*SCREEN_ADDR64(x,y) |= SCREEN_MASK64(x))
#define SCREEN_CLR64(x,y) (*SCREEN_ADDR64(x,y) &= ~SCREEN_MASK64(x))
#define SCREEN_XOR64(x,y) (*SCREEN_ADDR64(x,y) ^= SCREEN_MASK64(x))

void screen_init();
void screen_exit();

unsigned char *screen_ptr;

#endif
