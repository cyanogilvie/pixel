#ifndef _2D_H
#define _2D_H

#include <sys/types.h>

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

 
// pixel mixer
#define P_CPY(d,s) \
	*d = *s;

/*
#define P_BLEND(d,s) \
	{ \
		uint32	na; \
		if (s->ch.a > 0) { \
			if (s->ch.a < 255) { \
				d->ch.b = fact[s->ch.b][s->ch.a] + fact[d->ch.b][255-s->ch.a]; \
				d->ch.g = fact[s->ch.g][s->ch.a] + fact[d->ch.g][255-s->ch.a]; \
				d->ch.r = fact[s->ch.r][s->ch.a] + fact[d->ch.r][255-s->ch.a]; \
				na = (uint32)(d->ch.a + s->ch.a); \
				if (na>255) { \
					d->ch.a = 255; \
				} else { \
					d->ch.a = (uint8)na; \
				} \
			} else { \
				*d = *s; \
			} \
		} \
	}
*/

/*
#define P_BLEND(d,s) \
	{ \
		uint8   *sa  = (uint8 *)(fact + s->ch.a); \
		uint8   *sia = (uint8 *)(fact + (255 - s->ch.a)); \
		uint8   *da  = (uint8 *)(fact + d->ch.a); \
		if (s->ch.a > 0) { \
			if (s->ch.a < 255) { \
				d->ch.b = sia[d->ch.b] + sa[s->ch.b]; \
				d->ch.g = sia[d->ch.g] + sa[s->ch.g]; \
				d->ch.r = sia[d->ch.r] + sa[s->ch.r]; \
				d->ch.a = da[255 - d->ch.a] + s->ch.a; \
			} else { \
				*d = *s; \
			} \
		} \
	}
*/

/*
#define P_ALPHA(d,s) \
	{ \
		uint8   *sa; \
		uint8   *sia; \
		uint8   *da; \
		sa = (uint8 *)(fact + s->ch.a); \
		sia = (uint8 *)(fact + (255 - s->ch.a)); \
		da = (uint8 *)(fact + d->ch.a); \
		if (s->ch.a > 0) { \
			if (s->ch.a < 255) { \
				d->ch.b = sia[da[d->ch.b]] + sa[s->ch.b]; \
				d->ch.g = sia[da[d->ch.g]] + sa[s->ch.g]; \
				d->ch.r = sia[da[d->ch.r]] + sa[s->ch.r]; \
				d->ch.a = da[255 - s->ch.a] + s->ch.a; \
			} else { \
				*d = *s; \
			} \
		} \
	}
*/
#define P_ALPHA(d,s) \
	{ \
		uint8   *sa; \
		uint8   *sia; \
		if (s->ch.a == 0) { \
		} else if (d->ch.a == 0 || s->ch.a == 255) { \
			*d = *s; \
		} else { \
			sa = (uint8 *)(fact + s->ch.a); \
			sia = (uint8 *)(fact + 255 - s->ch.a); \
			d->ch.b = sa[s->ch.b] + sia[d->ch.b]; \
			d->ch.g = sa[s->ch.g] + sia[d->ch.g]; \
			d->ch.r = sa[s->ch.r] + sia[d->ch.r]; \
			d->ch.a = s->ch.a + sia[d->ch.a]; \
		} \
	}


// P_BLEND is deprecated, use P_ALPHA instead
#define P_BLEND(d,s) \
	P_ALPHA(d,s);

/*
#define P_ALPHA_UNDER(d,s) \
	{ \
		uint8   *sa; \
		uint8   *dia; \
		uint8   *da; \
		sa = (uint8 *)(fact + s->ch.a); \
		dia = (uint8 *)(fact + (255 - d->ch.a)); \
		da = (uint8 *)(fact + d->ch.a); \
		if (d->ch.a < 255) { \
			if (d->ch.a > 0) { \
				d->ch.b = dia[sa[s->ch.b]] + da[d->ch.b]; \
				d->ch.g = dia[sa[s->ch.g]] + da[d->ch.g]; \
				d->ch.r = dia[sa[s->ch.r]] + da[d->ch.r]; \
				d->ch.a = sa[255 - d->ch.a] + d->ch.a; \
			} else { \
				*d = *s; \
			} \
		} \
	}
*/
#define P_ALPHA_UNDER(d,s) \
	{ \
		uint8   *da; \
		uint8   *dia; \
		if (d->ch.a == 0) { \
			*d = *s; \
		} else if (s->ch.a == 0 || d->ch.a == 255) { \
		} else { \
			da = (uint8 *)(fact + d->ch.a); \
			dia = (uint8 *)(fact + 255 - d->ch.a); \
			d->ch.b = da[d->ch.b] + dia[s->ch.b]; \
			d->ch.g = da[d->ch.g] + dia[s->ch.g]; \
			d->ch.r = da[d->ch.r] + dia[s->ch.r]; \
			d->ch.a = d->ch.a + dia[s->ch.a]; \
		} \
	}

#define P_ADDITIVE(d,s) \
	{ \
		uint32	t; \
		uint32	na; \
		if (s->ch.a > 0) { \
			t = fact[s->ch.b][s->ch.a] + fact[d->ch.b][d->ch.a]; \
			d->ch.b = t<256 ? t : 255; \
			t = fact[s->ch.g][s->ch.a] + fact[d->ch.g][d->ch.a]; \
			d->ch.g = t<256 ? t : 255; \
			t = fact[s->ch.r][s->ch.a] + fact[d->ch.r][d->ch.a]; \
			d->ch.r = t<256 ? t : 255; \
			\
			na = (uint32)(d->ch.a + s->ch.a); \
			/*na = (uint32)((signed char)d->ch.a + ((signed char)(s->ch.a - d->ch.a) >> 1));*/ \
			if (na>255) { \
				d->ch.a = 255; \
			} else { \
				d->ch.a = (uint8)na; \
			} \
		} \
	}

#define P_BLIT(d,s) \
	if (s->ch.a != 0) \
		*d = *s;

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif


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

// lookups
EXTERN_C uint8	fact[256][256];	// [a][b] = a * (b/255.0)
EXTERN_C uint8	scale_lookup_square[256][256];
EXTERN_C uint8	scale_lookup_sine[256][256];
EXTERN_C uint8	scale_lookup_linear[256][256];
EXTERN_C uint8	inv_scale_lookup_square[256][256];
EXTERN_C uint8	inv_scale_lookup_sine[256][256];
EXTERN_C uint8	inv_scale_lookup_linear[256][256];


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

EXTERN_C void pmap_patch(gimp_image_t *dest, gimp_image_t *src,
		int sx, int sy, int sw, int sh, int dx, int dy, int flags);

EXTERN_C gimp_image_t *pmap_compose(gimp_image_t *pmap1, gimp_image_t *pmap2,
		int xofs, int yofs, int flags);

EXTERN_C void pmap_free(gimp_image_t **pmap);

EXTERN_C void pmap_filter	(gimp_image_t *dest, int flags, double factor);

EXTERN_C gimp_image_t *pmap_dropshadow (gimp_image_t *src, int blur);

EXTERN_C gimp_image_t *pmap_rotate(gimp_image_t *src, int quads);

EXTERN_C void do_dirty_tricks		();
 
EXTERN_C void init_2d				();


#endif
