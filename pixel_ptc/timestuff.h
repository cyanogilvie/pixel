#ifndef _TIMESTUFF_H
#define _TIMESTUFF_H

#include "main.h"

extern double	fps;
extern long		elapsed;
extern long long	frames;

extern void init_timestuff(console_inf *inf);
extern void frame_time(console_inf *inf);

#endif
