#include <stdio.h>
#include <SDL/SDL.h>
#include "main.h"

void sdl_frame_time(sdl_console_inf *inf)
{
	unsigned int			ticks, sec, usec;

	ticks = SDL_GetTicks();
	sec = ticks / 1000;
	usec = (ticks % 1000) * 1000;

	inf->elapsed = usec - inf->lastusec + (sec-inf->lastsec)*1000000;
	inf->lastsec = sec;
	inf->lastusec = usec;

	inf->fps = 1000000.0 / inf->elapsed;
}


void sdl_init_timestuff(sdl_console_inf *inf)
{
	unsigned int	ticks, sec, usec;

	ticks = SDL_GetTicks();
	sec = ticks / 1000;
	usec = (ticks % 1000) * 1000;

	inf->lastsec = sec;
	inf->lastusec = usec;
	inf->elapsed = 0;
	inf->fps = 0;
	inf->frames = 0;
}


