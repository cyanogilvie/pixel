#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include "2d.h"
#include "misc.h"

// lookups
uint8	fact[256][256];	// [a][b] = a * (b/255.0)
uint8	scale_lookup_square[256][256];
uint8	scale_lookup_sine[256][256];
uint8	scale_lookup_linear[256][256];
uint8	inv_scale_lookup_square[256][256];
uint8	inv_scale_lookup_sine[256][256];
uint8	inv_scale_lookup_linear[256][256];

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

gimp_image_t *pmap_new(int x, int y, _pel colour) //{{{1
{
	gimp_image_t	*new;

	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	new->width = x;
	new->height = y;
	new->bytes_per_pixel = 4;
	new->pixel_data = (_pel *)malloc(x*y*4);
//	asm_pelset(new->pixel_data, colour, x*y);

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
		asm_pelset(d, colour, len);
	}
}


void pmap_clr(gimp_image_t *dest, _pel colour) //{{{1
{
	memset(dest->pixel_data, colour.c, dest->width * dest->height * 4);
//	asm_pelset(dest->pixel_data, colour.c, dest->width * dest->height);
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

	if (x2 > src->width) x2 = src->width;
	if (y2 > src->height) y2 = src->height;
	
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
	gimp_image_t		*new;
	_pel				blank;

	blank.c = 0;
	
	width = pmap1->width + xofs + pmap2->width;
	height = pmap1->height + yofs + pmap2->height;
	
	new = pmap_new(width, height, blank);

	pmap_paste(new, pmap1, 0, 0, 0);
	pmap_paste(new, pmap2, xofs, yofs, flags);

	return new;
}



void pmap_free(gimp_image_t **pmap) // {{{1
{
	free((*pmap)->pixel_data);
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
		for (l = 0; l < dest->width*dest->height; l++)
			e++->ch.a = fact[e->ch.a][a];
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
				b = 0;
				g = 0;
				r = 0;
				a = 0;
				
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
				
				
				a  = fact[ scanline[0][x-1].ch.a ][ W_2 ];
				a += fact[   scanline[0][x].ch.a ][ W_1 ];
				a += fact[ scanline[0][x+1].ch.a ][ W_2 ];
				
				a += fact[ scanline[1][x-1].ch.a ][ W_1 ];
				a += fact[   scanline[1][x].ch.a ][ W_0 ];
				a += fact[ scanline[1][x+1].ch.a ][ W_1 ];

				a += fact[ scanline[2][x-1].ch.a ][ W_2 ];
				a += fact[   scanline[2][x].ch.a ][ W_1 ];
				a += fact[ scanline[2][x+1].ch.a ][ W_2 ];
				
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
	_pel			tmp;
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
	tmp.c = 0xffff00ff;
	
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


void do_dirty_tricks() // {{{1
{
	double		foo;
	uint32		a,b,bar;

	/*
	uint32		i;

	printf( "Building 32-16 buffer munging values: " );
	fflush( stdout );
	
	r_mask = visualinfo.red_mask;
	g_mask = visualinfo.green_mask;
	b_mask = visualinfo.blue_mask;
	for( i = 0; i < bpp; i++ ) {
		r_size += (r_mask >> i) & 0x1;
		g_size += (g_mask >> i) & 0x1;
		b_size += (b_mask >> i) & 0x1;
	}
	r_shift = ffs( visualinfo.red_mask   ) - 1 - (8-r_size); 
	g_shift = ffs( visualinfo.green_mask ) - 1 - (8-g_size); 
	b_shift = ffs( visualinfo.blue_mask  ) - 1 - (8-b_size); 

	abs_r_sh = abs(r_shift);
	abs_g_sh = abs(g_shift);
	abs_b_sh = abs(b_shift);
	printf( "done\n" );
	*/

	printf( "Generating lookups: " );
	fflush( stdout );

	for( a=0; a<=255; a++ )
		for( b=0; b<=255; b++ ) {
			foo = a * (b/255.0);
			bar = (uint8)foo;
			if( foo - (uint8)(foo) > 0.5 ) bar++;
			fact[a][b] = bar;
		}
	printf( "fact " );
	fflush( stdout );

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
	printf( "scale" );

	printf( "\n" );
}


void init_2d () // {{{1
{
	fprintf(stderr, "experimental version\n");
	do_dirty_tricks();
}


