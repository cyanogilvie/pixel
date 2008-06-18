#ifndef _PIXEL_H
#define _PIXEL_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <tcl.h>
#include <tclstuff.h>
#include <sys/time.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

//effects flags
#define MD_BLUR				0x00000001
#define MD_CHANNEL_MASK		0x00000002
// filter flags
#define MD_FILTER_R			0x00000100
#define MD_FILTER_G			0x00000200
#define	MD_FILTER_B			0x00000400
#define MD_FILTER_A			0x00000800
#define MD_FILTER_SMOOTH	0x00001000
#define MD_FILTER_ALPHA		0x00002000
#define MD_FILTER_ALPHA_Q	0x00004000
// blitter flags
#define MD_BLEND			0x00010000
#define MD_BLIT				0x00020000
#define MD_ALPHA			0x00040000
#define MD_ALPHA_UNDER		0x00080000
#define MD_ADDITIVE			0x00100000
// scale lookup table flags
#define MD_SCALE_SQUARE		0x01000000
#define MD_SCALE_SINE		0x02000000
#define	MD_SCALE_LINEAR		0x04000000
// channel masks
#define MD_MASK_ALPHA		0xff000000
#define MD_MASK_RED			0x00ff0000
#define MD_MASK_GREEN		0x0000ff00
#define MD_MASK_BLUE		0x000000ff

/*
typedef long signed int		sint32;
typedef long unsigned int	uint32;
//typedef unsigned int	uint;
//typedef signed int		sint;
typedef signed short	sint16;
typedef unsigned short	uint16;
typedef signed char		sint8;
typedef unsigned char	uint8;
*/
#ifndef sint32
#define sint32	long signed int
#endif
#ifndef uint32
#define uint32	long unsigned int
//typedef long unsigned int	uint32;
#endif
#ifndef uint
//typedef unsigned int	uint;
#endif
#ifndef sint
//typedef signed int		sint;
#endif
#ifndef sint16
#define sint16	signed short
#endif
#ifndef uint16
#define uint16	unsigned short
#endif
#ifndef sint8
#define sint8	signed char
#endif
#ifndef uint8
#define uint8	unsigned char
#endif


typedef union {
	uint32	c;
	struct {
		uint8	b,g,r,a;
	} ch;
} _pel;


typedef struct {
	uint32	width;
	uint32	height;
	uint32	bytes_per_pixel;
	_pel	*pixel_data;
} gimp_image_t;


typedef struct pmap_list {
	gimp_image_t	*pmap;
	void			*clientdata;
	struct pmap_list		*next;
} pmap_list;


EXTERN_C Tcl_ObjType tcl_pmap;


#define RLE_VERSION						0x1

// Success
#define RLE_STATUS_OK					0

// Supplied rle data to short to contain even a full header
#define RLE_STATUS_TOOSHORT				-1

// RLE header does not contain the proper tag ("pixelrle")
#define RLE_STATUS_BAD_TAG				-2

// End-of-line corruption check failed
#define RLE_STATUS_LINECHECK_FAILED		-3

// Unicode corruption check failed
#define RLE_STATUS_UNICODE_FAILED		-4

// Decoder version incompatibilty
#define RLE_STATUS_VERSION_FAILED		-5

// Too little data provided
#define RLE_STATUS_DATA_TRUNCATED		-6

// Got given more bytes than were expected
#define RLE_STATUS_ENOUGH_ALREADY		-7

// Bad chunk type
#define RLE_STATUS_BAD_CHUNK			-8

// Maptable key size is not one of 1, 2
#define RLE_STATUS_INVALID_KEYSIZE		-9

// Maptable key points beyond map
#define RLE_STATUS_KEY_OUT_OF_RANGE		-10


#ifdef __cplusplus
extern "C" {
#endif

#include "pixelDecls.h"
/*
#include <Pixel/2d.h>
#include <Pixel/bezierstuff.h>
#include <Pixel/misc.h>
#include <Pixel/primitives.h>
#include <Pixel/shaders.h>
#include <Pixel/tcl_pmap.h>
#include <Pixel/tcl_utils.h>
#include <Pixel/rle.h>
*/

#ifdef USE_PIXEL_STUBS
EXTERN CONST char *Pixel_InitStubs(Tcl_Interp *interp, char *version, int exact);
#endif

#ifdef __cplusplus
}
#endif

/*
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT
*/

#endif
