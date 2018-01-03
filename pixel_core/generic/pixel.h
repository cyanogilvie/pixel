#ifndef _PIXEL_H
#define _PIXEL_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <tcl.h>
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

//#include "tclstuff.h"
#include "2d_types.h"



typedef struct {
	char *		type;
	void *		info;
	//ptr to free_info routine
} sp_info;

EXTERN_C Tcl_ObjType tcl_pmap;


typedef struct {
	char			tag[8];					// "pixelrle"
	unsigned char	line_end_check[3];		// "\x0a\x0d"'a'
	unsigned char	unicode_check[2];		// "\0xf0\x00"
	unsigned char	version;
	unsigned int	width;
	unsigned int	height;
	unsigned short	map_table_length;		// Number of map entries
	unsigned char	map_table_keysize;		// map offset keysize in bytes
	unsigned int	num_chunks;				// number of chunks in data
	unsigned int	chunks_size;			// size in bytes of chunk data
} rle_header;

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
