#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "main.h"

void sdl_frame_time(sdl_console_inf *inf)
{
	struct timeval	tv;
	gettimeofday( &tv, NULL );
	inf->elapsed = tv.tv_usec - inf->lastusec + (tv.tv_sec-inf->lastsec)*1000000;
	inf->lastsec = tv.tv_sec;
	inf->lastusec = tv.tv_usec;

	inf->fps = 1000000.0 / inf->elapsed;
}


void sdl_init_timestuff(sdl_console_inf *inf)
{
	struct timeval	tv;

	gettimeofday( &tv, NULL );

	inf->lastsec = tv.tv_sec;
	inf->lastusec = tv.tv_usec;
	inf->elapsed = 0;
	inf->fps = 0;
	inf->frames = 0;
}


