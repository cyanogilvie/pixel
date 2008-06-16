/*
 * Distilled from the Imlib2 library
 */

#include "2d.h"
#include <stdlib.h>
#include <string.h>

// Macros <<<
#define CLIP(x, y, w, h, xx, yy, ww, hh) \
if (x < (xx)) {w += (x - (xx)); x = (xx);} \
if (y < (yy)) {h += (y - (yy)); y = (yy);} \
if ((x + w) > ((xx) + (ww))) {w = (ww) - (x - xx);} \
if ((y + h) > ((yy) + (hh))) {h = (hh) - (y - yy);}

#define RGBA_COMPOSE(r, g, b, a)  ((b) << 24) | ((g) << 16) | ((r) << 8) | (a)
#define INV_XAP                   (256 - xapoints[x])
#define XAP                       (xapoints[x])
#define INV_YAP                   (256 - yapoints[dyy + y])
#define YAP                       (yapoints[dyy + y])

#define A_VAL(p) ((DATA8 *)(p))[0]
#define R_VAL(p) ((DATA8 *)(p))[1]
#define G_VAL(p) ((DATA8 *)(p))[2]
#define B_VAL(p) ((DATA8 *)(p))[3]

// Macros >>>

# ifndef DATA64
#  define DATA64 unsigned long long
#  define DATA32 unsigned int
#  define DATA16 unsigned short
#  define DATA8  unsigned char
# endif

struct _scale_info {
   int                *xpoints;
   DATA32            **ypoints;
   int                *xapoints, *yapoints;
   int                 xup_yup;
};

typedef struct _scale_info ScaleInfo;

static ScaleInfo *FreeScaleInfo(ScaleInfo * isi) //<<<
{
	if (isi) {
		free(isi->xpoints);
		free(isi->ypoints);
		free(isi->xapoints);
		free(isi->yapoints);
		free(isi);
	}
	return NULL;
}

//>>>
static int *CalcApoints(int s, int d, int b1, int b2, int up) //<<<
{
	int		*p, i, j = 0, rv = 0;

	if (d < 0) {
		rv = 1;
		d = -d;
	}
	p = malloc(d * sizeof(int));
	if (d < (b1 + b2)) {
		if (d < b1) {
			b1 = d;
			b2 = 0;
		} else {
			b2 = d - b1;
		}
	}
	/* scaling up */
	if (up) {
		int		val, inc;

		for (i = 0; i < b1; i++)
			p[j++] = 0;

		if (d > (b1 + b2)) {
			int		ss, dd;

			ss = s - b1 - b2;
			dd = d - b1 - b2;
			val = 0;
			inc = (ss << 16) / dd;
			for (i = 0; i < dd; i++) {
				p[j++] = (val >> 8) - ((val >> 8) & 0xffffff00);
				if (((val >> 16) + b1) >= (s - 1))
					p[j - 1] = 0;
				val += inc;
			}
		}
		for (i = 0; i < b2; i++)
			p[j++] = 0;
	} else {	/* scaling down */
		int		val, inc;

		for (i = 0; i < b1; i++)
			p[j++] = (1 << (16 + 14)) + (1 << 14);
		if (d > (b1 + b2)) {
			int		ss, dd, ap, Cp;

			ss = s - b1 - b2;
			dd = d - b1 - b2;
			val = 0;
			inc = (ss << 16) / dd;
			Cp = ((dd << 14) / ss) + 1;
			for (i = 0; i < dd; i++) {
				ap = ((0x100 - ((val >> 8) & 0xff)) * Cp) >> 8;
				p[j] = ap | (Cp << 16);
				j++;
				val += inc;
			}
		}
		for (i = 0; i < b2; i++)
			p[j++] = (1 << (16 + 14)) + (1 << 14);
	}
	if (rv) {
		for (i = d / 2; --i >= 0;) {
			int		tmp = p[i];

			p[i] = p[d - i - 1];
			p[d - i - 1] = tmp;
		}
	}
	return p;
}

//>>>
static DATA32 **CalcYPoints(DATA32 *src, int sw, int sh, int dh, int b1, int b2) //<<<
{
	DATA32	**p;
	int		i, j = 0;
	int		val, inc, rv = 0;

	if (dh < 0) {
		dh = -dh;
		rv = 1;
	}
	p = malloc((dh + 1) * sizeof(DATA32 *));
	if (dh < (b1 + b2)) {
		if (dh < b1) {
			b1 = dh;
			b2 = 0;
		} else {
			b2 = dh - b1;
		}
	}
	val = 0;
	inc = 1 << 16;
	for (i = 0; i < b1; i++) {
		p[j++] = src + ((val >> 16) * sw);
		val += inc;
	}
	if (dh > (b1 + b2)) {
		val = (b1 << 16);
		inc = ((sh - b1 - b2) << 16) / (dh - b1 - b2);
		for (i = 0; i < (dh - b1 - b2); i++) {
			p[j++] = src + ((val >> 16) * sw);
			val += inc;
		}
	}
	val = (sh - b2) << 16;
	inc = 1 << 16;
	for (i = 0; i <= b2; i++) {
		p[j++] = src + ((val >> 16) * sw);
		val += inc;
	}
	if (rv) {
		for (i = dh / 2; --i >= 0;) {
			DATA32	*tmp = p[i];

			p[i] = p[dh - i - 1];
			p[dh - i - 1] = tmp;
		}
	}
	return p;
}

//>>>
static int *CalcXPoints(int sw, int dw, int b1, int b2) //<<<
{
	int                *p, i, j = 0;
	int                 val, inc, rv = 0;

	if (dw < 0) {
		dw = -dw;
		rv = 1;
	}
	p = malloc((dw + 1) * sizeof(int));
	if (dw < (b1 + b2)) {
		if (dw < b1) {
			b1 = dw;
			b2 = 0;
		} else {
			b2 = dw - b1;
		}
	}
	val = 0;
	inc = 1 << 16;
	for (i = 0; i < b1; i++) {
		p[j++] = (val >> 16);
		val += inc;
	}
	if (dw > (b1 + b2)) {
		val = (b1 << 16);
		inc = ((sw - b1 - b2) << 16) / (dw - b1 - b2);
		for (i = 0; i < (dw - b1 - b2); i++) {
			p[j++] = (val >> 16);
			val += inc;
		}
	}
	val = (sw - b2) << 16;
	inc = 1 << 16;
	for (i = 0; i <= b2; i++) {
		p[j++] = (val >> 16);
		val += inc;
	}
	if (rv) {
		for (i = dw / 2; --i >= 0;) {
			int	tmp = p[i];

			p[i] = p[dw - i - 1];
			p[dw - i - 1] = tmp;
		}
	}
	return p;
}

//>>>
static ScaleInfo *CalcScaleInfo(gimp_image_t *im, int sw, int sh, int dw, int dh) //<<<
{
	ScaleInfo     *isi;
	int                 scw, sch;

	scw = dw * im->width / sw;
	sch = dh * im->height / sh;

	isi = malloc(sizeof(ScaleInfo));
	if (!isi)
		return NULL;
	memset(isi, 0, sizeof(ScaleInfo));

	isi->xup_yup = (abs(dw) >= sw) + ((abs(dh) >= sh) << 1);

	isi->xpoints = CalcXPoints(im->width, scw, 0, 0);
	if (!isi->xpoints)
		return FreeScaleInfo(isi);
	isi->ypoints = CalcYPoints((DATA32 *)im->pixel_data, im->width, im->height,
			sch, 0, 0);
	if (!isi->ypoints)
		return FreeScaleInfo(isi);

	isi->xapoints = CalcApoints(im->width, scw, 0, 0, isi->xup_yup & 1);
	if (!isi->xapoints)
		return FreeScaleInfo(isi);

	isi->yapoints = CalcApoints(im->height, sch, 0, 0, isi->xup_yup & 2);
	if (!isi->yapoints)
		return FreeScaleInfo(isi);

	return isi;
}

//>>>
static void ScaleAARGBA(ScaleInfo *isi, DATA32 *dest, int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow) //<<<
{
   DATA32	*sptr, *dptr;
   int		x, y, end;
   DATA32	**ypoints = isi->ypoints;
   int		*xpoints = isi->xpoints;
   int		*xapoints = isi->xapoints;
   int		*yapoints = isi->yapoints;

   end = dxx + dw;

   if (isi->xup_yup == 3) { /* scaling up both ways <<< */
	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];
		   if (YAP > 0) {
			   for (x = dxx; x < end; x++) {
				   int		r, g, b, a;
				   int		rr, gg, bb, aa;
				   DATA32	*pix;

				   if (XAP > 0) {
					   pix = ypoints[dyy + y] + xpoints[x];
					   r = R_VAL(pix) * INV_XAP;
					   g = G_VAL(pix) * INV_XAP;
					   b = B_VAL(pix) * INV_XAP;
					   a = A_VAL(pix) * INV_XAP;
					   pix++;
					   r += R_VAL(pix) * XAP;
					   g += G_VAL(pix) * XAP;
					   b += B_VAL(pix) * XAP;
					   a += A_VAL(pix) * XAP;
					   pix += sow;
					   rr = R_VAL(pix) * XAP;
					   gg = G_VAL(pix) * XAP;
					   bb = B_VAL(pix) * XAP;
					   aa = A_VAL(pix) * XAP;
					   pix--;
					   rr += R_VAL(pix) * INV_XAP;
					   gg += G_VAL(pix) * INV_XAP;
					   bb += B_VAL(pix) * INV_XAP;
					   aa += A_VAL(pix) * INV_XAP;
					   r = ((rr * YAP) + (r * INV_YAP)) >> 16;
					   g = ((gg * YAP) + (g * INV_YAP)) >> 16;
					   b = ((bb * YAP) + (b * INV_YAP)) >> 16;
					   a = ((aa * YAP) + (a * INV_YAP)) >> 16;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   } else {
					   pix = ypoints[dyy + y] + xpoints[x];
					   r = R_VAL(pix) * INV_YAP;
					   g = G_VAL(pix) * INV_YAP;
					   b = B_VAL(pix) * INV_YAP;
					   a = A_VAL(pix) * INV_YAP;
					   pix += sow;
					   r += R_VAL(pix) * YAP;
					   g += G_VAL(pix) * YAP;
					   b += B_VAL(pix) * YAP;
					   a += A_VAL(pix) * YAP;
					   r >>= 8;
					   g >>= 8;
					   b >>= 8;
					   a >>= 8;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   }
			   }
		   } else {
			   for (x = dxx; x < end; x++) {
				   int		r, g, b, a;
				   DATA32	*pix;

				   if (XAP > 0) {
					   pix = ypoints[dyy + y] + xpoints[x];
					   r = R_VAL(pix) * INV_XAP;
					   g = G_VAL(pix) * INV_XAP;
					   b = B_VAL(pix) * INV_XAP;
					   a = A_VAL(pix) * INV_XAP;
					   pix++;
					   r += R_VAL(pix) * XAP;
					   g += G_VAL(pix) * XAP;
					   b += B_VAL(pix) * XAP;
					   a += A_VAL(pix) * XAP;
					   r >>= 8;
					   g >>= 8;
					   b >>= 8;
					   a >>= 8;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   } else {
					   *dptr++ = sptr[xpoints[x]];
				   }
			   }
		   }
	   }
   } /* scaling up both ways >>> */
   else if (isi->xup_yup == 1) { /* if we're scaling down vertically <<< */
	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   int                 yap;

		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];

		   yap = (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
		   if (yap > 1) {
			   for (x = dxx; x < end; x++) {
				   int			r = 0, g = 0, b = 0, a = 0;
				   int			rr = 0, gg = 0, bb = 0, aa = 0;
				   int			j;
				   DATA32		*pix;

				   if (XAP > 0) {
					   pix = sptr + xpoints[x];
					   for (j = 0; j < yap; j++) {
						   r += R_VAL(pix);
						   g += G_VAL(pix);
						   b += B_VAL(pix);
						   a += A_VAL(pix);
						   rr += R_VAL(pix + 1);
						   gg += G_VAL(pix + 1);
						   bb += B_VAL(pix + 1);
						   aa += A_VAL(pix + 1);
						   pix += sow;
					   }
					   r = r * INV_XAP / yap;
					   g = g * INV_XAP / yap;
					   b = b * INV_XAP / yap;
					   a = a * INV_XAP / yap;
					   r = (r + ((rr * XAP) / yap)) >> 8;
					   g = (g + ((gg * XAP) / yap)) >> 8;
					   b = (b + ((bb * XAP) / yap)) >> 8;
					   a = (a + ((aa * XAP) / yap)) >> 8;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   } else {
					   pix = sptr + xpoints[x];
					   for (j = 0; j < yap; j++) {
						   r += R_VAL(pix);
						   g += G_VAL(pix);
						   b += B_VAL(pix);
						   a += A_VAL(pix);
						   pix += sow;
					   }
					   r /= yap;
					   g /= yap;
					   b /= yap;
					   a /= yap;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   }
			   }
		   } else {
			   for (x = dxx; x < end; x++) {
				   int                 r = 0, g = 0, b = 0, a = 0;
				   DATA32             *pix;

				   if (XAP > 0) {
					   pix = ypoints[dyy + y] + xpoints[x];
					   r = R_VAL(pix) * INV_XAP;
					   g = G_VAL(pix) * INV_XAP;
					   b = B_VAL(pix) * INV_XAP;
					   a = A_VAL(pix) * INV_XAP;
					   pix++;
					   r += R_VAL(pix) * XAP;
					   g += G_VAL(pix) * XAP;
					   b += B_VAL(pix) * XAP;
					   a += A_VAL(pix) * XAP;
					   r >>= 8;
					   g >>= 8;
					   b >>= 8;
					   a >>= 8;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   } else {
					   *dptr++ = sptr[xpoints[x]];
				   }
			   }
		   }
	   }
   } /* if we're scaling down vertically >>> */
   else if (isi->xup_yup == 2) { /* if we're scaling down horizontally <<< */
	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];
		   if (YAP > 0) {
			   for (x = dxx; x < end; x++) {
				   int			r = 0, g = 0, b = 0, a = 0;
				   int			rr = 0, gg = 0, bb = 0, aa = 0;
				   int			xap;
				   int			i;
				   DATA32		*pix;

				   xap = xpoints[x + 1] - xpoints[x];
				   if (xap > 1) {
					   pix = ypoints[dyy + y] + xpoints[x];
					   for (i = 0; i < xap; i++) {
						   r += R_VAL(pix + i);
						   g += G_VAL(pix + i);
						   b += B_VAL(pix + i);
						   a += A_VAL(pix + i);
					   }
					   r = r * INV_YAP / xap;
					   g = g * INV_YAP / xap;
					   b = b * INV_YAP / xap;
					   a = a * INV_YAP / xap;
					   pix = ypoints[dyy + y] + xpoints[x] + sow;
					   for (i = 0; i < xap; i++) {
						   rr += R_VAL(pix + i);
						   gg += G_VAL(pix + i);
						   bb += B_VAL(pix + i);
						   aa += A_VAL(pix + i);
					   }
					   r = (r + ((rr * YAP) / xap)) >> 8;
					   g = (g + ((gg * YAP) / xap)) >> 8;
					   b = (b + ((bb * YAP) / xap)) >> 8;
					   a = (a + ((aa * YAP) / xap)) >> 8;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   } else {
					   pix = ypoints[dyy + y] + xpoints[x];
					   r = R_VAL(pix) * INV_YAP;
					   g = G_VAL(pix) * INV_YAP;
					   b = B_VAL(pix) * INV_YAP;
					   a = A_VAL(pix) * INV_YAP;
					   pix += sow;
					   r += R_VAL(pix) * YAP;
					   g += G_VAL(pix) * YAP;
					   b += B_VAL(pix) * YAP;
					   a += A_VAL(pix) * YAP;
					   r >>= 8;
					   g >>= 8;
					   b >>= 8;
					   a >>= 8;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   }
			   }
		   } else {
			   for (x = dxx; x < end; x++) {
				   int		r = 0, g = 0, b = 0, a = 0;
				   int		xap;
				   int		i;
				   DATA32	*pix;

				   xap = xpoints[x + 1] - xpoints[x];
				   if (xap > 1) {
					   pix = ypoints[dyy + y] + xpoints[x];
					   for (i = 0; i < xap; i++) {
						   r += R_VAL(pix + i);
						   g += G_VAL(pix + i);
						   b += B_VAL(pix + i);
						   a += A_VAL(pix + i);
					   }
					   r /= xap;
					   g /= xap;
					   b /= xap;
					   a /= xap;
					   *dptr++ = RGBA_COMPOSE(r, g, b, a);
				   } else {
					   *dptr++ = sptr[xpoints[x]];
				   }
			   }
		   }
	   }
   } /* if we're scaling down horizontally >>> */
   else { /* if we're scaling down horizontally & vertically <<< */
	   int		count;
	   DATA32	*pix;
	   int		a, r, g, b;

	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   int		yap = (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];
		   for (x = dxx; x < end; x++) {
			   int		xap = xpoints[x + 1] - xpoints[x];
			   int		i, j;

			   if ((xap > 1) || (yap > 1)) {
				   r = 0;
				   g = 0;
				   b = 0;
				   a = 0; // Cyan ??
				   pix = ypoints[dyy + y] + xpoints[x];
				   for (j = yap; --j >= 0;) {
					   for (i = xap; --i >= 0;) {
						   r += R_VAL(pix + i);
						   g += G_VAL(pix + i);
						   b += B_VAL(pix + i);
						   a += A_VAL(pix + i);
					   }
					   pix += sow;
				   }
				   count = xap * yap;
				   R_VAL(dptr) = r / count;
				   G_VAL(dptr) = g / count;
				   B_VAL(dptr) = b / count;
				   A_VAL(dptr) = a / count;
				   dptr++;
			   } else {
				   *dptr++ = sptr[xpoints[x]];
			   }
		   }
	   }
   } /* if we're scaling down horizontally & vertically >>> */
}

//>>>

//#define LINESIZE 16
#define LINESIZE 16
gimp_image_t *scale_pmap( //<<<
		gimp_image_t *src,
		int ssx, int ssy, int ssw, int ssh,
		int ddx, int ddy, int ddw, int ddh)
{
	ScaleInfo		*scaleinfo = NULL;
	DATA32			*buf = NULL;
	int				sx, sy, sw, sh, dx, dy, dw, dh, dxx, dyy, y2, x2;
	int				psx, psy, psw, psh;
	int				y, h, hh;
	gimp_image_t	*dst;
	_pel			init;

	init.c = 0;
	dst = pmap_new(abs(ddw), abs(ddh), init);
	// We will be overwriting all the dst pixels, so don't bother to blank

	sx = ssx;
	sy = ssy;
	sw = ssw;
	sh = ssh;
	dx = ddx;
	dy = ddy;
	dw = abs(ddw);
	dh = abs(ddh);
	/* don't do anything if we have a 0 width or height image to render */
	/* if the input rect size < 0 don't render either */
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
		return NULL;
	/* clip the source rect to be within the actual image */
	psx = sx;
	psy = sy;
	psw = sw;
	psh = sh;
	CLIP(sx, sy, sw, sh, 0, 0, src->width, src->height);
	if (psx != sx)
		dx += ((sx - psx) * abs(ddw)) / ssw;
	if (psy != sy)
		dy += ((sy - psy) * abs(ddh)) / ssh;
	if (psw != sw)
		dw = (dw * sw) / psw;
	if (psh != sh)
		dh = (dh * sh) / psh;
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
		return NULL;
	/* clip output coords to clipped input coords */
	psx = dx;
	psy = dy;
	psw = dw;
	psh = dh;
	x2 = sx;
	y2 = sy;
	CLIP(dx, dy, dw, dh, 0, 0, dst->width, dst->height);
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
		return NULL;
	if (psx != dx)
		sx += ((dx - psx) * ssw) / abs(ddw);
	if (psy != dy)
		sy += ((dy - psy) * ssh) / abs(ddh);
	if (psw != dw)
		sw = (sw * dw) / psw;
	if (psh != dh)
		sh = (sh * dh) / psh;
	dxx = dx - psx;
	dyy = dy - psy;
	dxx += (x2 * abs(ddw)) / ssw;
	dyy += (y2 * abs(ddh)) / ssh;

	if ((dw > 0) && (sw == 0))
		sw = 1;
	if ((dh > 0) && (sh == 0))
		sh = 1;
	/* do a second check to see if we now have invalid coords */
	/* don't do anything if we have a 0 width or height image to render */
	/* if the input rect size < 0 don't render either */
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
		return NULL;
	scaleinfo = CalcScaleInfo(src, ssw, ssh, ddw, ddh);
	if (!scaleinfo)
		return NULL;
	/* if we are scaling the image at all make a scaling buffer */
	/* allocate a buffer to render scaled RGBA data into */
	buf = malloc(dw * LINESIZE * sizeof(DATA32));
	if (!buf) {
		FreeScaleInfo(scaleinfo);
		return NULL;
	}
	/* setup h */
	h = dh;
	/* scale in LINESIZE Y chunks and convert to depth */
	for (y = 0; y < dh; y += LINESIZE) {
		hh = LINESIZE;
		if (h < LINESIZE)
			hh = h;
		/* scale the imagedata for this LINESIZE lines chunk of image */
		ScaleAARGBA(scaleinfo, (DATA32 *)dst->pixel_data, dxx, dyy + y,
				0, y, dw, hh, dw, src->width);

		h -= LINESIZE;
	}
	/* free up our buffers and point tables */
	free(buf);
	FreeScaleInfo(scaleinfo);

   return dst;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
