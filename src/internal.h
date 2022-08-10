#ifndef _GUI_INTERNAL_H_
#define _GUI_INTERNAL_H_

#include <gui.h>

//fonts
#include <freetype/freetype.h>

extern struct GuiContext *ctx;
extern struct GuiTexture arrows[4];
extern struct GuiTexture blank;

#define clamp(x, mn, mx) ({\
    __typeof__ (x) _x = (x); \
    __typeof__ (mn) _mn = (mn); \
    __typeof__ (mx) _mx = (mx); \
    max(_mn, min(_mx, _x)); })


typedef int ivec2[2];

typedef union ivec2s {
  ivec2 raw;
  struct {
    int x;
    int y;
  };
} ivec2s;

#endif