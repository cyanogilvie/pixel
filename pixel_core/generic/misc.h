#ifndef _MISC_H
#define _MISC_H

#include "2d.h"

EXTERN_C void asm_pelset(_pel *dest, _pel rgba, uint32 count);
EXTERN_C void asm_buf_draw_box(_pel *dest_buf, int width, uint32 x1, uint32 y1, uint32 x2, uint32 y2, _pel rgba);

#endif
