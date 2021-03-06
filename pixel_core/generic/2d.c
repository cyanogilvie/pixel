#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include "pixel.h"
//#include "2d.h"
//#include "misc.h"

// lookups
uint8	fact[256][256];	// [a][b] = a * (b/255.0)
uint8	scale_lookup_square[256][256];
uint8	scale_lookup_sine[256][256];
uint8	scale_lookup_linear[256][256];
uint8	inv_scale_lookup_square[256][256];
uint8	inv_scale_lookup_sine[256][256];
uint8	inv_scale_lookup_linear[256][256];
float	sRGB_to_linear_floats[256];
#define L2SRGB	4096
uint8_t	linear_floats_to_sRGB[L2SRGB];

uint32	r_mask, g_mask, b_mask;
uint32	r_size = 0;
uint32	g_size = 0;
uint32	b_size = 0;
sint32	r_shift, g_shift, b_shift;
uint32	abs_r_sh, abs_g_sh, abs_b_sh;


#define SWAP_INT(i1, i2) {int hold = i1; i1 = i2; i2 = hold;}

//#define FAST_ASM_BUF_MUNGE
//#define FAST_ASM_BUF_CLR
#define FAST_ASM_MEMCPY

#ifdef FAST_ASM_MEMCPY
void asm_memcpy( _pel *dest_buf, _pel *src_buf, uint32 len );
#endif

//int g_total_pmaps = 0;
gimp_image_t *pmap_new(int x, int y, _pel colour) //{{{1
{
	gimp_image_t	*new;

	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	new->width = x;
	new->height = y;
	new->bytes_per_pixel = 4;
	//out->pixel_data = (_pel*)malloc(sizeof(_pel) * x * y);
	// Allocate memory aligned to 32 bytes (for SIMD)
	if (0 != posix_memalign((void**)&new->pixel_data, 32, sizeof(_pel) * x * y))
		Tcl_Panic("Could not allocate memory for pelf data");
//	asm_pelset(new->pixel_data, colour, x*y);

//	g_total_pmaps++;
//	fprintf(stderr, "allocing pmap: w: %d h: %d  %p %d\n",
//			x, y, new, g_total_pmaps);

	return new;
}


gimp_image_t *pmap_dup(gimp_image_t *src) //{{{1
{
	gimp_image_t	*new;

	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	new->width = src->width;
	new->height = src->height;
	new->bytes_per_pixel = 4;
	new->pixel_data = (_pel *)malloc(src->width*src->height*4);
	memcpy(new->pixel_data, src->pixel_data, src->width*src->height);

	return new;
}


void row_blend(_pel *d, _pel *s, int len, int flags) //{{{1
{
	_pel		*last;

	last = d + len;
//	fprintf(stderr, "len: (%d)\n", len);
	
	if (flags == 0) {
		memcpy(d, s, len*4);
	} else if (flags & MD_BLEND) {
		for (; d<last; d++, s++)
			P_BLEND(d, s);
	} else if (flags & MD_ALPHA) {
		for (; d<last; d++, s++)
			P_ALPHA(d, s);
	} else if (flags & MD_ALPHA_UNDER) {
		for (; d<last; d++, s++)
			P_ALPHA_UNDER(d, s);
	} else if (flags & MD_ADDITIVE) {
		for (; d<last; d++, s++)
			P_ADDITIVE(d, s);
	} else if (flags & MD_BLIT) {
	} else {
		memcpy(d, s, len*4);
	}
}


void row_blend_ref(_pel *d, _pel *s, _pel *r, int len, int flags) //{{{1
{
	_pel		*last;

	last = d + len;
//	fprintf(stderr, "len: (%d)\n", len);
	
	if (flags == 0) {
		memcpy(d, s, len*4);
	} else if (flags & MD_BLEND) {
		for (; d<last; d++, s++, r++)
			P_ALPHA_REF(d, s, r);
	} else if (flags & MD_ALPHA) {
		for (; d<last; d++, s++, r++)
			P_ALPHA_REF(d, s, r);
	} else if (flags & MD_ALPHA_UNDER) {
		for (; d<last; d++, s++, r++)
			P_ALPHA_REF(d, s, r);
	} else if (flags & MD_ADDITIVE) {
		for (; d<last; d++, s++, r++)
			P_ADDITIVE_REF(d, s, r);
	} else if (flags & MD_BLIT) {
	} else {
		memcpy(d, s, len*4);
	}
}


void row_set(_pel *dest, _pel colour, int len, int flags) //{{{1
{
	_pel			*last;
	register _pel	*s = &colour;
	register _pel	*d = dest;

	last = d + len;
	
	if (flags & MD_BLEND) {
		for (; d<last; d++)
			P_BLEND(d, s);
	} else if (flags & MD_ALPHA) {
		for (; d<last; d++)
			P_ALPHA(d, s);
	} else if (flags & MD_ALPHA_UNDER) {
		for (; d<last; d++)
			P_ALPHA_UNDER(d, s)
	} else if (flags & MD_ADDITIVE) {
		for (; d<last; d++)
			P_ADDITIVE(d, s);
	} else if (flags & MD_BLIT) {
	} else {
#ifndef _WINDOWS
		asm_pelset(d, colour, len);
#else
		for (; d<last; d++)
			*d = *s;
#endif
	}
}


void pmap_clr(gimp_image_t *dest, _pel colour) //{{{1
{
	memset(dest->pixel_data, colour.c, dest->width * dest->height * 4);
//	asm_pelset(dest->pixel_data, colour.c, dest->width * dest->height);
}


void pmapf_clr(struct pmapf* dest, pelf colour) //{{{1
{
	int		i = dest->width * dest->height;
	pelf*	d = dest->pixel_data;

	for (; i; i--, d++)
		*d = colour;
}


gimp_image_t *pmap_cut(gimp_image_t *src, int x1, int y1, int x2, int y2) //{{{1
{
	gimp_image_t	*new;
	int				width, height;
	_pel			*d;
	_pel			*s;
	
	if (x2 < x1) SWAP_INT(x1, x2);
	if (y2 < y1) SWAP_INT(y1, y2);
	
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;

	if (x2 > src->width) x2 = src->width-1;
	if (y2 > src->height) y2 = src->height-1;
	
	width = x2 - x1 + 1;
	height = y2 - y1 + 1;

	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	new->width = width;
	new->height = height;
	new->bytes_per_pixel = 4;
	d = new->pixel_data = (_pel *)malloc(width * height * 4);

	s = src->pixel_data + y1*src->width + x1;

	for (; y1 <= y2; y1++) {
		memcpy(d, s, width*4);
		d += new->width;
		s += src->width;
	}

	return new;
}


void pmap_paste(gimp_image_t *dest, gimp_image_t *src, int xofs, int yofs, int flags) //{{{1
{
	int			start_row, start_col, rows, cols, r;
	_pel		*sr;
	_pel		*dr;


	start_row = 0;
	start_col = 0;
	rows = src->height;
	cols = src->width;

	// Case: off top or left
	if (xofs < 0) {
		cols -= abs(xofs);
		start_col += abs(xofs);
		xofs = 0;
	}
	if (yofs < 0) {
		rows -= abs(yofs);
		start_row += abs(yofs);
		yofs = 0;
	}

	// Case: off bottom or right
	if ((int)(xofs + cols) > (int)(dest->width)) {
		cols -= (xofs + cols) - dest->width;
	}
	if ((int)(yofs + rows) > (int)(dest->height)) {
		rows -= (yofs + rows) - dest->height;
	}

	if (cols <= 0) return;
	if (rows <= 0) return;

	sr = src->pixel_data + start_row*src->width + start_col;
	dr = dest->pixel_data + yofs*dest->width + xofs;

	for (r=0; r<rows; r++) {
		row_blend(dr, sr, cols, flags);
		sr += src->width;
		dr += dest->width;
	}
}


void pmap_paste_ref(gimp_image_t *dest, gimp_image_t *src, gimp_image_t *ref, int xofs, int yofs, int flags) //{{{1
{
	int			start_row, start_col, rows, cols, r;
	_pel		*sr;
	_pel		*dr;
	_pel		*rr;


	start_row = 0;
	start_col = 0;
	rows = src->height;
	cols = src->width;

	// Case: off top or left
	if (xofs < 0) {
		cols -= abs(xofs);
		start_col += abs(xofs);
		xofs = 0;
	}
	if (yofs < 0) {
		rows -= abs(yofs);
		start_row += abs(yofs);
		yofs = 0;
	}

	// Case: off bottom or right
	if ((int)(xofs + cols) > (int)(dest->width)) {
		cols -= (xofs + cols) - dest->width;
	}
	if ((int)(yofs + rows) > (int)(dest->height)) {
		rows -= (yofs + rows) - dest->height;
	}

	if (cols <= 0) return;
	if (rows <= 0) return;

	sr = src->pixel_data + start_row*src->width + start_col;
	dr = dest->pixel_data + yofs*dest->width + xofs;
	rr = ref->pixel_data + yofs*ref->width + xofs;

	for (r=0; r<rows; r++) {
		row_blend_ref(dr, sr, rr, cols, flags);
		sr += src->width;
		dr += dest->width;
		rr += ref->width;
	}
}


void pmap_patch(gimp_image_t *dest, gimp_image_t *src, int sx, int sy, int sw, int sh, int dx, int dy, int flags) //{{{1
{
	int			start_row, start_col, rows, cols, r;
	_pel		*sr;
	_pel		*dr;


	rows = sh;
	cols = sw;
	start_row = sy;
	start_col = sx;

	// Case: off top or left
	if (dx < 0) {
		cols -= abs(dx);
		start_col += abs(dx);
		dx = 0;
	}
	if (dy < 0) {
		rows -= abs(dy);
		start_row += abs(dy);
		dy = 0;
	}

	// Case: off bottom or right
	if ((int)(dx + cols) > (int)(dest->width)) {
		cols -= (dx + cols) - dest->width;
	}
	if ((int)(dy + rows) > (int)(dest->height)) {
		rows -= (dy + rows) - dest->height;
	}

	if (cols <= 0) return;
	if (rows <= 0) return;

	
	/*
	// rows = smaller(sh, dest->height - abs(dy), src->height - abs(sy))
	// cols = smaller(sw, dest->width - abs(dx), src->width - abs(sx))

	start_row = sy;
	rows = sh;
	if (sy < 0) {
		start_row = abs(sy);
		rows -= abs(sy);
	}
	if ((int)(dest->height - abs(dy)) < (int)rows)
		rows = dest->height - abs(dy);
	if ((int)(src->height - abs(sy)) < (int)rows)
		rows = src->height - abs(sy);

	start_col = sx;
	cols = sw;
	if (sx < 0) {
		start_col = abs(sx);
		cols -= abs(sx);
	}
	if ((int)(dest->width - abs(dx)) < (int)rows)
		rows = dest->width - abs(dx);
	if ((int)(src->width - abs(sx)) < (int)rows)
		rows = src->width - abs(sx);
//	if ((int)(sx + sw) > (int)dest->width)
//		cols -= (sx + sw) - dest->width;

//	if ((int)(sx + cols) > (int)src->width)
//		cols -= (sx + cols) - src->width;

	if (cols <= 0) return;
	if (rows <= 0) return;

	if (dx < 0) {
		start_col += abs(dx);
		dx = 0;
	}
	if (dy < 0) {
		start_row += abs(dy);
		dy = 0;
	}
	*/
	
	sr = src->pixel_data + start_row*src->width + start_col;
	dr = dest->pixel_data + dy*dest->width + dx;

	for (r=0; r<rows; r++) {
		row_blend(dr, sr, cols, flags);
		sr += src->width;
		dr += dest->width;
	}
}


gimp_image_t *pmap_compose(gimp_image_t *pmap1, gimp_image_t *pmap2, int xofs, int yofs, int flags) //{{{1
{
	int					width, height;
	int					w1, w2, h1, h2;
	int					x1, x2, y1, y2;
	gimp_image_t		*new;
	_pel				blank;

	blank.c = 0;

	x1 = (xofs < 0 ? abs(xofs) : 0);
	y1 = (yofs < 0 ? abs(yofs) : 0);
	x2 = (xofs > 0 ? xofs : 0);
	y2 = (yofs > 0 ? yofs : 0);
	
	w1 = pmap1->width + x1;
	h1 = pmap1->height + y1;
	w2 = pmap2->width + x2;
	h2 = pmap2->height + y2;
	
	width = (w1 > w2 ? w1 : w2);
	height = (h1 > h2 ? h1 : h2);
	
	new = pmap_new(width, height, blank);
	memset(new->pixel_data, 0, width * height * 4);

	pmap_paste(new, pmap1, x1, y1, 0);
	pmap_paste(new, pmap2, x2, y2, flags);

	return new;
}



void pmap_free(gimp_image_t **pmap) // {{{1
{
//	g_total_pmaps--;
//	fprintf(stderr, "freeing pmap: w: %d h: %d  %p %d\n", (*pmap)->width, (*pmap)->height, *pmap, g_total_pmaps);
	free((*pmap)->pixel_data);
	(*pmap)->pixel_data = NULL;
	free(*pmap);
	*pmap = NULL;
}


void pmap_filter(gimp_image_t *dest, int flags, double factor) // {{{1
{
	register uint32	c_mask = MD_MASK_ALPHA;
	register _pel	*e;
	uint32	l;
	
	if (flags & (MD_FILTER_R | MD_FILTER_B | MD_FILTER_G | MD_FILTER_A)) {
		if (flags & MD_FILTER_R) c_mask |= MD_MASK_RED;
		if (flags & MD_FILTER_G) c_mask |= MD_MASK_GREEN;
		if (flags & MD_FILTER_B) c_mask |= MD_MASK_BLUE;
		e = dest->pixel_data;
		for (l = 0; l < dest->width*dest->height; l++)
			e++->c &= c_mask;
	}

	if (flags & MD_FILTER_ALPHA) {
		uint32	na;
		e = dest->pixel_data;
		for (l = 0; l < dest->width*dest->height; l++) {
			na = (uint32)(e->ch.a * factor);
			if (na>255)
				e++->ch.a = 255;
			else
				e++->ch.a = (uint8)na;
		}
	}

	if (flags & MD_FILTER_ALPHA_Q) {
		uint8	a = (uint8)(factor * 255.0);
		e = dest->pixel_data;
		for (l = 0; l < dest->width*dest->height; l++, e++)
			e->ch.a = fact[e->ch.a][a];
	}

	if (flags & MD_FILTER_SMOOTH) {
		uint32	x,y;
		int		r,g,b,a;
		_pel	scanline[3][dest->width];

		// 256 == (W_0) + 4(W_1) + 4(W_2)
#define W_0		85
#define W_1		86 >> 2
#define W_2		85 >> 2
		e = dest->pixel_data;

		//TODO: sub nine-block cases top | left | bot | right
		e += dest->width;
		for (y=1; y<dest->height-1; y++) {
			memcpy( scanline[0], scanline[1], dest->width << 2 );
			memcpy( scanline[1], e, dest->width << 2 ); 
			memcpy( scanline[2], e + dest->width, dest->width << 2 ); 
			e++;
			for (x=1; x<dest->width-1; x++) {
				a  = fact[ scanline[0][x-1].ch.a ][ W_2 ];
				a += fact[   scanline[0][x].ch.a ][ W_1 ];
				a += fact[ scanline[0][x+1].ch.a ][ W_2 ];
				
				a += fact[ scanline[1][x-1].ch.a ][ W_1 ];
				a += fact[   scanline[1][x].ch.a ][ W_0 ];
				a += fact[ scanline[1][x+1].ch.a ][ W_1 ];

				a += fact[ scanline[2][x-1].ch.a ][ W_2 ];
				a += fact[   scanline[2][x].ch.a ][ W_1 ];
				a += fact[ scanline[2][x+1].ch.a ][ W_2 ];

				if (a == 0) {
					// HACK: if we're completely transparent, skip the rest
					e->c = 0;
					e++;
					continue;
				}
				
				b  = fact[ scanline[0][x-1].ch.b ][ W_2 ];
				b += fact[   scanline[0][x].ch.b ][ W_1 ];
				b += fact[ scanline[0][x+1].ch.b ][ W_2 ];
				
				b += fact[ scanline[1][x-1].ch.b ][ W_1 ];
				b += fact[   scanline[1][x].ch.b ][ W_0 ];
				b += fact[ scanline[1][x+1].ch.b ][ W_1 ];

				b += fact[ scanline[2][x-1].ch.b ][ W_2 ];
				b += fact[   scanline[2][x].ch.b ][ W_1 ];
				b += fact[ scanline[2][x+1].ch.b ][ W_2 ];
				
				
				g  = fact[ scanline[0][x-1].ch.g ][ W_2 ];
				g += fact[   scanline[0][x].ch.g ][ W_1 ];
				g += fact[ scanline[0][x+1].ch.g ][ W_2 ];
				
				g += fact[ scanline[1][x-1].ch.g ][ W_1 ];
				g += fact[   scanline[1][x].ch.g ][ W_0 ];
				g += fact[ scanline[1][x+1].ch.g ][ W_1 ];

				g += fact[ scanline[2][x-1].ch.g ][ W_2 ];
				g += fact[   scanline[2][x].ch.g ][ W_1 ];
				g += fact[ scanline[2][x+1].ch.g ][ W_2 ];
				
				
				r  = fact[ scanline[0][x-1].ch.r ][ W_2 ];
				r += fact[   scanline[0][x].ch.r ][ W_1 ];
				r += fact[ scanline[0][x+1].ch.r ][ W_2 ];
				
				r += fact[ scanline[1][x-1].ch.r ][ W_1 ];
				r += fact[   scanline[1][x].ch.r ][ W_0 ];
				r += fact[ scanline[1][x+1].ch.r ][ W_1 ];

				r += fact[ scanline[2][x-1].ch.r ][ W_2 ];
				r += fact[   scanline[2][x].ch.r ][ W_1 ];
				r += fact[ scanline[2][x+1].ch.r ][ W_2 ];
				
				e->ch.b = b;
				e->ch.g = g;
				e->ch.r = r;
				e++->ch.a = a;
			}
			e++;
		}
	}
}


gimp_image_t *pmap_dropshadow(gimp_image_t *src, int blur) // {{{1
{
	gimp_image_t	*new;
	int				i, x, y, size;
	_pel			*s;
	_pel			*d;
	_pel			*sr;
	_pel			*dr;
	
	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));

	new->width = src->width + (blur*2);
	new->height = src->height + (blur*2);
	new->bytes_per_pixel = src->bytes_per_pixel;

	size = new->width * new->height;
	
	new->pixel_data = (_pel *)malloc(size * new->bytes_per_pixel);
	memset(new->pixel_data, 0x0, size * new->bytes_per_pixel);

	sr = src->pixel_data;
	dr = new->pixel_data + blur*new->width + blur;
	
	for (y=0; y<src->height; y++) {
		s = sr;
		d = dr;
		for (x=0; x<src->width; x++, s++, d++) {
			d->c = 0;
			d->ch.a = s->ch.a;
		}
		sr += src->width;
		dr += new->width;
	}

	for (i=0; i<blur; i++)
		pmap_filter(new, MD_FILTER_SMOOTH, 1.0);

	return new;
}


/*
gimp_image_t *pmap_scale(gimp_image_t *src, int new_width, int new_height)
{
	gimp_image_t	*res;
	int				x, y;
	double			sx, sy, bx, by;
	double			dsx, dsy, dbx, dby;
	double			rx, ry;
	int				ofsx, ofsy;
	int				steps_x, steps_y;
	_pel			*s;
	_pel			*d;
	_pel			scratch;
	scratch.c = 0x00000000;
	res = pmap_new(new_width, new_height, scratch);
	
	steps_x = src->width;
	steps_y = src->height;
	if (steps_x < new_width) steps_x = new_width;
	if (steps_y < new_height) steps_y = new_height;
	
	dsx = (double)(src->width) / steps_x;
	dsy = (double)(src->height) / steps_y;
	ddx = (double)(new_width) / steps_x;
	ddy = (double)(new_height) / steps_y;
	
	rx = (double)(ddx / dsx);
	dy = (double)(ddy / dsy);
	
	for (y=0, dy=0.5, sy=0.5; y<steps_y; y++, dy+=ddy, sy+=dsy) {
		for (x=0, dx=0.5, sx=0.5; x<steps_x; x++, dx+=ddx, sx+=dsx) {
			s = src->pixel_data + (int)(sy*src->width + sx);
			d = res->pixel_data + (int)(dy*dest->width + dx);
			
			
		}
	}

	
	return res;
}
*/


gimp_image_t *pmap_rotate(gimp_image_t *src, int quads) //{{{1
{
	_pel			*s, *d;
	gimp_image_t	*new;
	//_pel			tmp;
	int				sign;
	int				littlejump, bigjump;
	int				nx, ny;
	
	sign = (quads < 0) ? -1 : 1;
//	fprintf(stderr, "quads1: (%d)\n", quads);
	quads = abs(quads) % 4;
//	fprintf(stderr, "quads2: (%d)\n", quads);

	if (quads == 0)
		return pmap_dup(src);
	
//	fprintf(stderr, "quads2.1: (%d)\n", quads);
	//tmp.c = 0xffff00ff;
	
//	fprintf(stderr, "quads2.2: (%d)\n", quads);
//	new = pmap_new(src->height, src->width, tmp);
	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	new->width = src->height;
	new->height = src->width;
	new->bytes_per_pixel = 4;
	new->pixel_data = (_pel *)malloc(new->width * new->height * 4);

//	fprintf(stderr, "quads2.3: (%d)\n", quads);
	d = new->pixel_data;

//	fprintf(stderr, "quads2.4 (%d)\n", quads);
	if (quads == 3) {
		quads = 1;
		sign = sign * -1;
		fprintf(stderr, "quads2.5: (%d)\n", quads);
	}
//	fprintf(stderr, "quads2: (%d)\n", quads);

//	fprintf(stderr, "quads3: (%d) sign: (%d)\n", quads, sign);
	quads = quads * sign;
//	fprintf(stderr, "quads4: (%d) sign: (%d)\n", quads, sign);

	switch (quads) {
		case 1:
			s = src->pixel_data + (src->width * src->height) - src->width;
			littlejump = -src->width;
			bigjump = (src->width * src->height) - src->width + 1;
			break;

		case -1:
			s = src->pixel_data + (src->width - 1);
			littlejump = src->width;
			bigjump = -((src->width * src->height) - src->width + 1);
			break;

		case -2:
		case 2:
			s = src->pixel_data + (src->width * src->height) - 1;
			littlejump = -1;
			bigjump = -1;
			break;

		default:
			fprintf(stderr, "Bogus quads value in pmap_rotate: (%d)\n", quads);
			s = src->pixel_data;		// Shouldn't ever get here
			littlejump = 0;
			bigjump = 0;
			break;
	}

	for (ny = 0; ny < new->height; ny++, s+=bigjump, s-=littlejump) {
		for (nx = 0; nx < new->width; nx++, d++, s+=littlejump) {
			d->c = s->c;
		}
	}

	return new;
}


void digest_region(src, x, y, w, h, r, g, b, a) //{{{1
	gimp_image_t	*src;
	int				x, y, w, h;
	unsigned int	*r;
	unsigned int	*g;
	unsigned int	*b;
	unsigned int	*a;
{
	unsigned int	lr, lg, lb, la, lx, ly, span, considered;
	_pel			*s;

	lr = lg = lb = la = 0;

	if (w < 0) {
		x += w;
		w = abs(w);
	}
	if (h < 0) {
		y += h;
		h = abs(h);
	}

	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}

	if (x + w > src->width) {
		w = src->width - x;
	}
	if (y + h > src->height) {
		y = src->height - y;
	}

	s = src->pixel_data + y * src->width + x;
	span = src->width - w;
	for (ly = 0; ly < h; ly++) {
		for (lx = 0; lx < w; lx++) {
			lr += s->ch.r;
			lg += s->ch.g;
			lb += s->ch.b;
			la += s->ch.a;
			s++;
		}
		s += span;
	}

	considered = w * h;

	if (considered > 0) {
		*r = lr / considered;
		*g = lg / considered;
		*b = lb / considered;
		*a = la / considered;
	} else {
		*r = 0;
		*g = 0;
		*b = 0;
		*a = 0;
	}
}


void put_pixel(gimp_image_t *dest, int x, int y, _pel col, int flags) //{{{1
{
	_pel		*d;
	_pel		*s = &col;

	d = dest->pixel_data + dest->width * y + x;
//	fprintf(stderr, "len: (%d)\n", len);
	
	if (flags == 0) {
		d->c = col.c;
	} else if (flags & MD_BLEND) {
		P_BLEND(d, s);
	} else if (flags & MD_ALPHA) {
		P_ALPHA(d, s);
	} else if (flags & MD_ALPHA_UNDER) {
		P_ALPHA_UNDER(d, s);
	} else if (flags & MD_ADDITIVE) {
		P_ADDITIVE(d, s);
	} else if (flags & MD_BLIT) {
	} else {
		d->c = col.c;
	}
}


_pel get_pixel(gimp_image_t *src, int x, int y) //{{{1
{
	_pel		*s;
	_pel		d;

	s = src->pixel_data + src->width * y + x;
//	fprintf(stderr, "len: (%d)\n", len);
	
	d.c = s->c;

	return d;
}


void rgb2hsv(r, g, b, h, s, v) //{{{1
	unsigned char	r, g, b;
	double			*h, *s, *v;
{
#define MAX		256
#define OLD_HSV
#ifdef OLD_HSV
	int		low = MAX;
	int		mid;
	int		high = -1;
	int		base;
	int		rising;

	if (r < low) low = r;
	if (g < low) low = g;
	if (b < low) low = b;
	if (r > high) high = r;
	if (g > high) high = g;
	if (b > high) high = b;

	if (r == high && b == low) {
		rising = 1;
		base = 0;
		mid = g;
	} else if (g == high && b == low) {
		rising = 0;
		base = 60;
		mid = r;
	} else if (g == high && r == low) {
		rising = 1;
		base = 120;
		mid = b;
	} else if (b == high && r == low) {
		rising = 0;
		base = 180;
		mid = g;
	} else if (b == high && g == low) {
		rising = 1;
		base = 240;
		mid = r;
	} else {
		rising = 0;
		base = 300;
		mid = b;
	}

	if (rising) {
		*h = base + ((mid - low) * 60.0) / (high - low + 1);
	} else {
		*h = base + ((high - mid) * 60.0) / (high - low + 1);
	}

	*s = (high - low) * 100.0 / high;
	*v = (high * 100.0) / MAX;
#else
	int		high = -1;
	int		low = MAX;
	double	nr, ng, nb;

	nr = (double)r / MAX;
	ng = (double)g / MAX;
	nb = (double)b / MAX;

	if (r < low) low = r;
	if (g < low) low = g;
	if (b < low) low = b;
	if (r > high) high = r;
	if (g > high) high = g;
	if (b > high) high = b;

	if (high == r) {
		*h = 60.0 * (ng - nb);
		if (*h < 0) *h += 360;
	} else if (high = g) {
		*h = 120 + 60 * (nb - nr);
	} else {
		*h = 240 + 60 * (nr - ng);
	}

	*s = (high - low) * 100.0 / high;
	*v = (high * 100.0) / MAX;
#endif
}


void hsv2rgb(h, s, v, r, g, b) //{{{1
	double			h, s, v;
	unsigned char	*r, *g, *b;
{
#define MAX		256
	int		low = MAX;
	int		high;
	int		rising;
	unsigned char		*d;
	double	hh;

	high = round(v * MAX / 100.0);
	low = round(high - (s * high / 100.0));
	//if (h < 0 || h > 360) h = h % 360;

	if (h <= 60) {
		//fprintf(stderr, "segment: 0\n");
		*r = high;
		*b = low;
		d = g;
		rising = 1;
	} else if (h <= 120) {
		//fprintf(stderr, "segment: 1\n");
		d = r;
		*g = high;
		*b = low;
		rising = 0;
	} else if (h <= 180) {
		//fprintf(stderr, "segment: 2\n");
		*r = low;
		*g = high;
		d = b;
		rising = 1;
	} else if (h <= 240) {
		//fprintf(stderr, "segment: 3\n");
		*r = low;
		d = g;
		*b = high;
		rising = 0;
	} else if (h <= 300) {
		//fprintf(stderr, "segment: 4\n");
		d = r;
		*g = low;
		*b = high;
		rising = 1;
	} else {
		//fprintf(stderr, "segment: 5\n");
		*r = high;
		*g = low;
		d = b;
		rising = 0;
	}

	hh = h;
	//fprintf(stderr, "before: hh: %f\n", hh);
	while (hh > 60.0) hh -= 60.0;
	//fprintf(stderr, "after: hh: %f\n", hh);
	hh /= 60.0;
	//fprintf(stderr, "normalized: hh: %f\n", hh);

	if (rising) {
		*d = round(hh*(high - low) + low);
	} else {
		*d = round((1 - hh)*(high - low) + low);
	}
}

//}}}1

struct pmapf* pmapf_alpha_over(struct pmapf* dest, struct pmapf* src, int xofs, int yofs) //{{{
{
	int		x, y, c, to_x, to_y, yofs_src=0, xofs_src=0;	// dest coord space
	pelf*	d;
	pelf*	s;
	pelf*	o;
	struct pmapf*	out = NULL;

	out = pmapf_new(dest->width, dest->height);
	memcpy(out->pixel_data, dest->pixel_data, dest->width*dest->height*dest->bytes_per_pixel);
	o = out->pixel_data;

	to_x = xofs + src->width;
	to_y = yofs + src->height;

	if (to_x > dest->width)  to_x = dest->width;
	if (to_y > dest->height) to_y = dest->height;

	if (yofs < 0) {
		yofs_src = -yofs;
		yofs = 0;
		//fprintf(stderr, "Adjusting yofs: %d, yofs_src: %d\n", yofs, yofs_src);
	}

	if (xofs < 0) {
		xofs_src = -xofs;
		xofs = 0;
		//fprintf(stderr, "Adjusting xofs: %d, xofs_src: %d\n", xofs, xofs_src);
	}

	//fprintf(stderr, "yofs: %d, yofs_src: %d, src->height: %d, to_y: %d\n", yofs, yofs_src, src->height, to_y);
	if (yofs_src >= src->height) return out;
	if (xofs_src >= src->width)  return out;

	for (y=yofs; y<to_y; y++) {
		d = dest->pixel_data + y*dest->width + xofs;
		o = out->pixel_data + y*out->width + xofs;
		s = src->pixel_data + (y-yofs+yofs_src)*src->width + xofs_src;
		for (x=xofs; x<to_x; x++, d++, s++, o++) {
			for (c=0; c<3; c++)
				o->chan[c] = s->ch.a*s->chan[c] + (1-s->ch.a)*d->chan[c];
			o->ch.a = s->ch.a + (1-s->ch.a) * d->ch.a;
		}
	}

	return out;
}

//}}}
struct pmapf* pmapf_rotate_90(struct pmapf* restrict src, int quads) //{{{1
{
	pelf*			s;
	pelf*			d;
	struct pmapf*	new = NULL;
	int				sign;
	int				littlejump, bigjump;
	int				nx, ny;
	
	sign = (quads < 0) ? -1 : 1;
	quads = abs(quads) % 4;

	if (quads == 3) {
		quads = 1;
		sign = sign * -1;
	}

	switch (quads) {
		case 1:
		case -1:
			new = pmapf_new(src->height, src->width);
			break;

		case 0:
		case 2:
			new = pmapf_new(src->width, src->height);
			break;
	}

	d = new->pixel_data;

	quads = quads * sign;

	switch (quads) {
		case 0:
			s = src->pixel_data;
			{
				const int		len = new->width * new->height * 4;
				float* restrict	sp = s->chan;
				float* restrict	dp = d->chan;
				int				i;

				for (i=0; i<len; i++)
					*dp++ = *sp++;

				return new;
			}

		case 1:
			s = src->pixel_data + (src->width * src->height) - src->width;
			littlejump = -src->width;
			bigjump = (src->width * src->height) - src->width + 1;
			break;

		case -1:
			s = src->pixel_data + (src->width - 1);
			littlejump = src->width;
			bigjump = -((src->width * src->height) - src->width + 1);
			break;

		case -2:
		case 2:
			s = src->pixel_data;
			d = new->pixel_data + new->height*new->width -1;
			{
				const int		len = new->width * new->height;
				int				i;

				for (i=0; i<len; i++) {
					float* restrict dp = d->chan;
					float* restrict sp = s->chan;
					int c;

					for (c=0; c<4; c++)
						dp[c] = sp[c];

					d--;
					s++;
				}

				return new;
			}

		default:
			fprintf(stderr, "Bogus quads value in pmap_rotate: (%d)\n", quads);
			s = src->pixel_data;		// Shouldn't ever get here
			littlejump = 0;
			bigjump = 0;
			break;
	}

	{
		const int	big = bigjump;
		const int	little = littlejump;
		int			c;

		for (ny=0; ny<new->height; ny++, s+=big, s-=little) {
			for (nx=0; nx<new->width; nx++, d++, s+=little) {
				float* restrict sp = s->chan;
				float* restrict dp = d->chan;
				for (c=0; c<4; c++)
					dp[c] = sp[c];
			}
		}
	}

	return new;
}


struct pmapf* pmapf_cut(struct pmapf* restrict src, int x1, int y1, int x2, int y2) //{{{1
{
	struct pmapf* restrict	dst = NULL;
	
	if (x2 < x1) SWAP_INT(x1, x2);
	if (y2 < y1) SWAP_INT(y1, y2);
	
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;

	if (x2 > src->width)  x2 = src->width-1;
	if (y2 > src->height) y2 = src->height-1;
	
	{
		const int				src_width  = src->width;
		const int				dst_width  = x2 - x1 + 1;
		const int				dst_height = y2 - y1 + 1;
		const int				end_y = y2;
		int						y;
		pelf* restrict			s = src->pixel_data + y1*src->width + x1;
		pelf* restrict			d = NULL;

		dst = pmapf_new(dst_width, dst_height);
		d = dst->pixel_data;

		for (y=y1; y<=end_y; y++) {
			memcpy(d, s, dst_width*16);
			d += dst_width;
			s += src_width;
		}
	}

	return dst;
}



void do_dirty_tricks() // {{{1
{
	double			foo;
	uint32_t		a,b,bar;
	float			v, g;

	for( a=0; a<=255; a++ )
		for( b=0; b<=255; b++ ) {
			foo = a * (b/255.0);
			bar = (uint8)foo;
			if( foo - (uint8)(foo) > 0.5 ) bar++;
			fact[a][b] = bar;
		}

	for( a=0; a<=255; a++ )
		for( b=0; b<=255; b++ ) {
			foo = (((128-abs(a-128)) + 127) / (255.0)) * b;
			scale_lookup_linear[a][b] = (uint8)foo;
			if( foo - scale_lookup_linear[a][b] > 0.5 ) scale_lookup_linear[a][b]++;
			inv_scale_lookup_linear[a][b] = b - scale_lookup_linear[a][b];
			
			foo = (((127 * sin( ( (a/255.0)*180 )*0.01745329252 ))+128) /255.0) * b;
			scale_lookup_sine[a][b] = (uint8)foo;
			if( foo - scale_lookup_sine[a][b] > 0.5 ) scale_lookup_sine[a][b]++;
			inv_scale_lookup_sine[a][b] = b - scale_lookup_sine[a][b];

			foo = b;
			scale_lookup_square[a][b] = (uint8)foo;
			if( foo - scale_lookup_square[a][b] > 0.5 ) scale_lookup_square[a][b]++;
			inv_scale_lookup_square[a][b] = b - scale_lookup_square[a][b];
		}

	// Precompute the lookup from sRGB 8bit samples to linear floats
	for (a=0; a<256; a++) {
		sRGB_to_linear_floats[a] = a < 10 ?
			a / (255*12.92)  :
			powf((a/255.0 + 0.055)/(1 + 0.055), 2.4);
		//fprintf(stderr, "a: %d -> %.4f (%.2f)\n", a, sRGB_to_linear_floats[a], sRGB_to_linear_floats[a] * 255);
	}

	// Precompute the lookup from linear floats * (L2SRGB-1) to sRGB 8bit samples
	for (a=0; a<L2SRGB; a++) {
		v = a/(float)(L2SRGB-1);
		g = v < 0.0031308 ?
			0xff*(v*12.92)  :
			0xff*((1+0.055)*powf(v, 1/2.4)-0.055);
		linear_floats_to_sRGB[a] = .5 + g;
		//fprintf(stderr, "a: %d (%.2f), g: %.2f, v: %.4f -> %d\n", a, a * 255.0/L2SRGB, g, v, linear_floats_to_sRGB[a]);
	}
}


//}}}1

struct pmapf* pmapf_new(int width, int height) //{{{
{
	struct pmapf*	out = NULL;

	out = (struct pmapf*)malloc(sizeof(struct pmapf));
	out->width = width;
	out->height = height;
	out->bytes_per_pixel = sizeof(pelf);	// Used to signal floating point pixel data
	//out->pixel_data = (pelf*)malloc(sizeof(pelf) * out->width * out->height);
	// Allocate memory aligned to 32 bytes (for SIMD)
	if (0 != posix_memalign((void**)&out->pixel_data, 32, sizeof(pelf) * out->width * out->height))
		Tcl_Panic("Could not allocate memory for pelf data");

	return out;
}

//}}}
void pmapf_free(struct pmapf** pmapf) //{{{
{
	if ((*pmapf)->pixel_data) {
		free((*pmapf)->pixel_data);
		(*pmapf)->pixel_data = NULL;
	}
	(*pmapf)->pixel_data = NULL;
	free(*pmapf);
	*pmapf = NULL;
}

//}}}
inline pelf clamp_pelf(pelf in) //{{{
{
	pelf	out;
	int		c;

	for (c=0; c<4; c++)
		out.chan[c] = in.chan[c] < 0.0 ? 0.0 :
			in.chan[c] > 1.0 ? 1.0 : in.chan[c];

	return out;
}

//}}}
static inline float clamp_chan(float in) //{{{
{
	return in < 0.0 ? 0.0 :
		in > 1.0 ? 1.0 :
		in;
}

//}}}
inline int clamp_int(int in, int max) //{{{
{
	return in < 0.0 ? 0.0 :
		in > max ? max :
		in;
}

//}}}
struct pmapf* pmap_to_pmapf(gimp_image_t* in) //{{{
{
	int				x, y, c;
	struct pmapf*	out;
	_pel*			s;
	pelf*			d;

	out = pmapf_new(in->width, in->height);

	s = (_pel*)(in->pixel_data);
	d = out->pixel_data;

	for (y=0; y<in->height; y++) {
		for (x=0; x<in->width; x++, s++, d++) {
			for (c=0; c<3; c++)
				d->chan[c] = sRGB_to_linear_floats[s->chan[c]];

			d->chan[CHAN_A] = s->chan[CHAN_A] / 255.0;
		}
	}

	return out;
}

//}}}
gimp_image_t* pmapf_to_pmap(struct pmapf* in) //{{{
{
	int				x, y, c;
	gimp_image_t*	out;
	_pel*			d;
	pelf*			s;

	out = pmap_new(in->width, in->height, (_pel)(uint32_t)0);

	s = in->pixel_data;
	d = (_pel*)(out->pixel_data);

	for (y=0; y<in->height; y++) {
		for (x=0; x<in->width; x++, s++, d++) {
			for (c=0; c<3; c++)
				d->chan[c] = linear_floats_to_sRGB[(int)(clamp_chan(s->chan[c]) * (L2SRGB-1))];

			d->chan[CHAN_A] = 0xff * clamp_chan(s->chan[CHAN_A] + .5/256);
		}
	}

	return out;
}

//}}}

void init_2d () // {{{1
{
	do_dirty_tricks();
}


