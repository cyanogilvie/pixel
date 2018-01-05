#ifndef _PIXEL_PNG_ALL_H
#define _PIXEL_PNG_ALL_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <png.h>
#include <pixel.h>
#include "tclstuff.h"
#include "pngstuff.h"

#define PNG_SIG_LEN	8

struct png_membuf {
	int			ofs;
	int			len;
	uint8_t*	buf;
};

#endif
