#ifndef _2D_H
#define _2D_H

#include <sys/types.h>
#include "2d_types.h"


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

#include "2d_lookups.h"

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif


// library functions
EXTERN_C gimp_image_t *pmap_new(int x, int y, _pel colour);

EXTERN_C gimp_image_t *pmap_dup(gimp_image_t *src);

EXTERN_C void row_blend(_pel *d, _pel *s, int len, int flags);

EXTERN_C void row_set(_pel *dest, _pel colour, int len, int flags);

EXTERN_C void pmap_clr(gimp_image_t *dest, _pel colour);

EXTERN_C gimp_image_t *pmap_cut(gimp_image_t *src, 
		int x1, int y1, int x2, int y2);

EXTERN_C void pmap_paste(gimp_image_t *dest, gimp_image_t *src, 
		int xofs, int yofs, int flags);

EXTERN_C void pmap_paste_ref(gimp_image_t *dest, gimp_image_t *src, 
		gimp_image_t *ref, int xofs, int yofs, int flags);

EXTERN_C void pmap_patch(gimp_image_t *dest, gimp_image_t *src,
		int sx, int sy, int sw, int sh, int dx, int dy, int flags);

EXTERN_C gimp_image_t *pmap_compose(gimp_image_t *pmap1, gimp_image_t *pmap2,
		int xofs, int yofs, int flags);

EXTERN_C void pmap_free(gimp_image_t **pmap);

EXTERN_C void pmap_filter(gimp_image_t *dest, int flags, double factor);

EXTERN_C gimp_image_t *pmap_dropshadow (gimp_image_t *src, int blur);

EXTERN_C gimp_image_t *pmap_rotate(gimp_image_t *src, int quads);

EXTERN_C void digest_region(gimp_image_t *src, int x, int y, int w, int h,
		unsigned int *r, unsigned int *g, unsigned int *b, unsigned int *a);

EXTERN_C void put_pixel(gimp_image_t *dest, int x, int y, _pel col, int flags);
EXTERN_C _pel get_pixel(gimp_image_t *src, int x, int y);

EXTERN_C void rgb2hsv(unsigned char r, unsigned char g, unsigned char b, double *h,  double *s,  double *v); 
EXTERN_C void hsv2rgb(double h,  double s,  double v, unsigned char *r, unsigned char *g, unsigned char *b); 

EXTERN_C void do_dirty_tricks		();
 
EXTERN_C void init_2d				();

// From scale_image.c
EXTERN_C gimp_image_t *scale_pmap(
		gimp_image_t *src,
		int ssx, int ssy, int ssw, int ssh,
		int ddx, int ddy, int ddw, int ddh);


#endif
