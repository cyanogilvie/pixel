#ifndef _2D_TYPES_H
#define _2D_TYPES_H

#include <stdint.h>

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


#define P_ALPHA_REF(d,s,r) \
	{ \
		uint8   *sa; \
		uint8   *sia; \
		if (s->ch.a == 0) { \
			*d = *r; \
		} else if (r->ch.a == 0 || s->ch.a == 255) { \
			*d = *s; \
		} else { \
			sa = (uint8 *)(fact + s->ch.a); \
			sia = (uint8 *)(fact + 255 - s->ch.a); \
			d->ch.b = sa[s->ch.b] + sia[r->ch.b]; \
			d->ch.g = sa[s->ch.g] + sia[r->ch.g]; \
			d->ch.r = sa[s->ch.r] + sia[r->ch.r]; \
			d->ch.a = s->ch.a + sia[r->ch.a]; \
		} \
	}


// P_BLEND is deprecated, use P_ALPHA instead
#define P_BLEND(d,s) \
	P_ALPHA(d,s);
#define P_BLEND_REF(d,s,r) \
	P_ALPHA(d,s,r);

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

#define P_ALPHA_UNDER_REF(d,s,r) \
	{ \
		uint8   *da; \
		uint8   *dia; \
		if (r->ch.a == 0) { \
			*d = *s; \
		} else if (s->ch.a == 0 || r->ch.a == 255) { \
			*d = *r; \
		} else { \
			da = (uint8 *)(fact + r->ch.a); \
			dia = (uint8 *)(fact + 255 - r->ch.a); \
			d->ch.b = da[r->ch.b] + dia[s->ch.b]; \
			d->ch.g = da[r->ch.g] + dia[s->ch.g]; \
			d->ch.r = da[r->ch.r] + dia[s->ch.r]; \
			d->ch.a = r->ch.a + dia[s->ch.a]; \
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

#define P_ADDITIVE_REF(d,s,r) \
	{ \
		uint32	t; \
		uint32	na; \
		if (s->ch.a > 0) { \
			t = fact[s->ch.b][s->ch.a] + fact[r->ch.b][r->ch.a]; \
			d->ch.b = t<256 ? t : 255; \
			t = fact[s->ch.g][s->ch.a] + fact[r->ch.g][r->ch.a]; \
			d->ch.g = t<256 ? t : 255; \
			t = fact[s->ch.r][s->ch.a] + fact[r->ch.r][r->ch.a]; \
			d->ch.r = t<256 ? t : 255; \
			\
			na = (uint32)(r->ch.a + s->ch.a); \
			/*na = (uint32)((signed char)d->ch.a + ((signed char)(s->ch.a - d->ch.a) >> 1));*/ \
			if (na>255) { \
				d->ch.a = 255; \
			} else { \
				d->ch.a = (uint8)na; \
			} \
		} else { \
			*d = *r; \
		} \
	}

#define P_BLIT(d,s) \
	if (s->ch.a != 0) \
		*d = *s;


/*
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
*/
#ifndef sint32
#define sint32	int32_t
#endif
#ifndef uint32
#define uint32	uint32_t
#endif
#ifndef sint16
#define sint16	int16_t
#endif
#ifndef uint16
#define uint16	uint16_t
#endif
#ifndef sint8
#define sint8	int8_t
#endif
#ifndef uint8
#define uint8	uint8_t
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

#endif
