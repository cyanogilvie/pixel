typedef void *Imlib_Image;
typedef void *Imlib_Context;

# ifndef DATA64
#  define DATA64 unsigned long long
#  define DATA32 unsigned int
#  define DATA16 unsigned short
#  define DATA8  unsigned char
# endif

#define RGBA_COMPOSE(r, g, b, a)  ((a) << 24) | ((r) << 16) | ((g) << 8) | (b)
#define INV_XAP                   (256 - xapoints[x])
#define XAP                       (xapoints[x])
#define INV_YAP                   (256 - yapoints[dyy + y])
#define YAP                       (yapoints[dyy + y])

#define A_VAL(p) ((DATA8 *)(p))[0]
#define R_VAL(p) ((DATA8 *)(p))[1]
#define G_VAL(p) ((DATA8 *)(p))[2]
#define B_VAL(p) ((DATA8 *)(p))[3]

#define CLIP(x, y, w, h, xx, yy, ww, hh) \
if (x < (xx)) {w += (x - (xx)); x = (xx);} \
if (y < (yy)) {h += (y - (yy)); y = (yy);} \
if ((x + w) > ((xx) + (ww))) {w = (ww) - (x - xx);} \
if ((y + h) > ((yy) + (hh))) {h = (hh) - (y - yy);}
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define CLIP_TO(_x, _y, _w, _h, _cx, _cy, _cw, _ch) \
{ \
if (INTERSECTS(_x, _y, _w, _h, _cx, _cy, _cw, _ch)) \
   { \
         if (_x < _cx) \
	{ \
	           _w += _x - _cx; \
	           _x = _cx; \
	           if (_w < 0) _w = 0; \
	} \
         if ((_x + _w) > (_cx + _cw)) \
	     _w = _cx + _cw - _x; \
         if (_y < _cy) \
	{ \
	           _h += _y - _cy; \
	           _y = _cy; \
	           if (_h < 0) _h = 0; \
	} \
         if ((_y + _h) > (_cy + _ch)) \
	     _h = _cy + _ch - _y; \
   } \
else \
   { \
      _w = 0; _h = 0; \
   } \
}
   
# define IMAGE_HAS_ALPHA(im) ((im)->flags & F_HAS_ALPHA)
# define IMAGE_IS_UNLOADED(im) ((im)->flags & F_UNLOADED)
# define IMAGE_IS_UNCACHEABLE(im) ((im)->flags & F_UNCACHEABLE)
# define IMAGE_ALWAYS_CHECK_DISK(im) ((im)->flags & F_ALWAYS_CHECK_DISK)
# define IMAGE_IS_VALID(im) (!((im)->flags & F_INVALID))
# define IMAGE_FREE_DATA(im) (!((im)->flags & F_DONT_FREE_DATA))

# define SET_FLAG(flags, f) ((flags) |= (f))
# define UNSET_FLAG(flags, f) ((flags) &= (~f))

#define   CAST_IMAGE(im, image) (im) = (ImlibImage *)(image)
#define   CHECK_PARAM_POINTER_RETURN(func, sparam, param, ret) \
if (!(param)) \
{ \
  fprintf(stderr, "***** Imlib2 Developer Warning ***** :\n" \
                  "\tThis program is calling the Imlib call:\n\n" \
                  "\t%s();\n\n" \
                  "\tWith the parameter:\n\n" \
                  "\t%s\n\n" \
                  "\tbeing NULL. Please fix your program.\n", func, sparam); \
  return ret; \
}

#define   CHECK_PARAM_POINTER(func, sparam, param) \
if (!(param)) \
{ \
  fprintf(stderr, "***** Imlib2 Developer Warning ***** :\n" \
                  "\tThis program is calling the Imlib call:\n\n" \
                  "\t%s();\n\n" \
                  "\tWith the parameter:\n\n" \
                  "\t%s\n\n" \
                  "\tbeing NULL. Please fix your program.\n", func, sparam); \
  return; \
}

struct _imlib_scale_info {
   int                *xpoints;
   DATA32            **ypoints;
   int                *xapoints, *yapoints;
   int                 xup_yup;
   DATA32             *pix_assert;
};

typedef struct _imlib_scale_info ImlibScaleInfo;

enum _iflags
{
   F_NONE              = 0,
   F_HAS_ALPHA         = (1 << 0),
   F_UNLOADED          = (1 << 1),
   F_UNCACHEABLE       = (1 << 2),
   F_ALWAYS_CHECK_DISK = (1 << 3),
   F_INVALID           = (1 << 4),
   F_DONT_FREE_DATA    = (1 << 5),
   F_FORMAT_IRRELEVANT = (1 << 6),
   F_BORDER_IRRELEVANT = (1 << 7),
   F_ALPHA_IRRELEVANT  = (1 << 8)
};
typedef enum   _iflags                  ImlibImageFlags;

struct _imlibcontext;
typedef struct _imlibcontext ImlibContext;
struct _imlibcontext {
   char                anti_alias;
   char                dither;
   char                blend;
   Imlib_Color_Modifier color_modifier;
   Imlib_Operation     operation;
   Imlib_Font          font;
   Imlib_Text_Direction direction;
   double              angle;
   Imlib_Color         color;
   Imlib_Color_Range   color_range;
   Imlib_Image         image;
   Imlib_Progress_Function progress_func;
   char                progress_granularity;
   char                dither_mask;
   int                 mask_alpha_threshold;
   Imlib_Filter        filter;
   Imlib_Rectangle     cliprect;
   Imlib_TTF_Encoding  encoding;

   int                 references;
   char                dirty;
};


struct _imlibcontextitem;
typedef struct _imlibcontextitem ImlibContextItem;
struct _imlibcontextitem {
   ImlibContext       *context;
   ImlibContextItem   *below;
};

EAPI void
imlib_context_push(Imlib_Context context)
{
   ImlibContextItem   *item;

   CHECK_PARAM_POINTER("imlib_context_push", "context", context);
   ctx = (ImlibContext *) context;

   item = malloc(sizeof(ImlibContextItem));
   item->context = ctx;
   item->below = contexts;
   contexts = item;

   ctx->references++;
}

Imlib_Context imlib_context_new(void) {
   ImlibContext       *context = malloc(sizeof(ImlibContext));

   context->anti_alias = 1;
   context->dither = 0;
   context->blend = 1;
   context->color_modifier = NULL;
   context->operation = IMLIB_OP_COPY;
   context->font = NULL;
   context->direction = IMLIB_TEXT_TO_RIGHT;
   context->angle = 0.0;
   context->color.alpha = 255;
   context->color.red = 255;
   context->color.green = 255;
   context->color.blue = 255;
   context->color_range = NULL;
   context->image = NULL;
   context->progress_func = NULL;
   context->progress_granularity = 0;
   context->dither_mask = 0;
   context->mask_alpha_threshold = 128;
   context->filter = NULL;
   context->cliprect.x = 0;
   context->cliprect.y = 0;
   context->cliprect.w = 0;
   context->cliprect.h = 0;
   context->encoding = IMLIB_TTF_ENCODING_ISO_8859_1;

   context->references = 0;
   context->dirty = 0;

   return (Imlib_Context) context;
}

static ImlibContext *
_imlib_context_get(void)
{
   ImlibContext       *context;

   context = imlib_context_new();
   imlib_context_push(context);

   return context;
}

#define CHECK_CONTEXT(_ctx) \
   if (!_ctx) _ctx = _imlib_context_get()

struct _imlibimagetag
{
   char           *key;
   int             val;
   void           *data;
   void          (*destructor)(ImlibImage *im, void *data);
   ImlibImageTag  *next;
};

typedef struct _imlibimagetag           ImlibImageTag;

typedef int (*ImlibProgressFunction)(ImlibImage *im, char percent,
				      int update_x, int update_y,
				      int update_w, int update_h);
struct _imlibloader
{
   char         *file;
   int           num_formats;
   char        **formats;
   void         *handle;
   char        (*load)(ImlibImage *im,
		       ImlibProgressFunction progress,
		       char progress_granularity, char immediate_load);
   char        (*save)(ImlibImage *im,
		       ImlibProgressFunction progress,
		       char progress_granularity);
   ImlibLoader  *next;
};

typedef struct _imlibloader             ImlibLoader;

struct _imlibborder
{
   int left, right, top, bottom;
};

typedef struct _imlibborder             ImlibBorder;

enum _iflags
{
   F_NONE              = 0,
   F_HAS_ALPHA         = (1 << 0),
   F_UNLOADED          = (1 << 1),
   F_UNCACHEABLE       = (1 << 2),
   F_ALWAYS_CHECK_DISK = (1 << 3),
   F_INVALID           = (1 << 4),
   F_DONT_FREE_DATA    = (1 << 5),
   F_FORMAT_IRRELEVANT = (1 << 6),
   F_BORDER_IRRELEVANT = (1 << 7),
   F_ALPHA_IRRELEVANT  = (1 << 8)
};

typedef enum   _iflags                  ImlibImageFlags;

struct _imlibimage
{
   char             *file;
   int               w, h;
   DATA32           *data;
   ImlibImageFlags   flags;
   time_t            moddate;
   ImlibBorder       border;
   int               references;
   ImlibLoader      *loader;
   char             *format;
   ImlibImage       *next;
   ImlibImageTag    *tags;
   char             *real_file;
   char             *key;
};

typedef struct _imlibimage              ImlibImage;

/* create an image data struct and fill it in */
ImlibImage *__imlib_ProduceImage(void)
{
   ImlibImage         *im;

   im = malloc(sizeof(ImlibImage));
   memset(im, 0, sizeof(ImlibImage));
   im->data = NULL;
   im->file = NULL;
   im->real_file = NULL;
   im->key = NULL;
   im->flags = F_FORMAT_IRRELEVANT | F_BORDER_IRRELEVANT | F_ALPHA_IRRELEVANT;
   im->loader = NULL;
   im->next = NULL;
   im->tags = NULL;
   return im;
}

/* create a new image struct from data passed that is wize w x h then return */
/* a pointer to that image struct */
ImlibImage *__imlib_CreateImage(int w, int h, DATA32 * data)
{
   ImlibImage         *im;

   im = __imlib_ProduceImage();
   im->w = w;
   im->h = h;
   im->data = data;
   im->references = 1;
   SET_FLAG(im->flags, F_UNCACHEABLE);
   return im;
}

/* free an image struct */
void __imlib_ConsumeImage(ImlibImage * im) {

   __imlib_FreeAllTags(im);
   if (im->file)
      free(im->file);
   if (im->real_file)
      free(im->real_file);
   if (im->key)
      free(im->key);
   if ((IMAGE_FREE_DATA(im)) && (im->data))
      free(im->data);
   if (im->format)
      free(im->format);
   free(im);
}


ImlibScaleInfo *__imlib_CalcScaleInfo(
		gimp_image_t *im,
		int sw, int sh, int dw, int dh)
{
	ImlibScaleInfo     *isi;
	int                 scw, sch;

	scw = dw * im->width / sw;
	sch = dh * im->height / sh;

	isi = malloc(sizeof(ImlibScaleInfo));
	if (!isi)
		return NULL;
	memset(isi, 0, sizeof(ImlibScaleInfo));

	isi->pix_assert = im->pixel_data + im->width * im->height;

	isi->xup_yup = (abs(dw) >= sw) + ((abs(dh) >= sh) << 1);

	isi->xpoints = __imlib_CalcXPoints(im->width, scw, 0, 0);
	if (!isi->xpoints)
		return __imlib_FreeScaleInfo(isi);
	isi->ypoints = __imlib_CalcYPoints(im->pixel_data, im->width, im->height,
			sch, 0, 0);
	if (!isi->ypoints)
		return __imlib_FreeScaleInfo(isi);

	isi->xapoints = __imlib_CalcApoints(im->width, scw, 0, 0, isi->xup_yup & 1);
	if (!isi->xapoints)
		return __imlib_FreeScaleInfo(isi);

	isi->yapoints = __imlib_CalcApoints(im->height, sch, 0, 0, isi->xup_yup & 2);
	if (!isi->yapoints)
		return __imlib_FreeScaleInfo(isi);

	return isi;
}

ImlibScaleInfo *__imlib_FreeScaleInfo(ImlibScaleInfo * isi)
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

static int *__imlib_CalcApoints(int s, int d, int b1, int b2, int up)
{
	int                *p, i, j = 0, rv = 0;

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

static DATA32 **__imlib_CalcYPoints(DATA32 * src, int sw, int sh, int dh, int b1, int b2)
{
	DATA32            **p;
	int                 i, j = 0;
	int                 val, inc, rv = 0;

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

static int *__imlib_CalcXPoints(int sw, int dw, int b1, int b2)
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

void __imlib_ScaleAARGBA(ImlibScaleInfo * isi, DATA32 * dest, int dxx, int dyy,
                    int dx, int dy, int dw, int dh, int dow, int sow)
{
   DATA32             *sptr, *dptr;
   int                 x, y, end;
   DATA32            **ypoints = isi->ypoints;
   int                *xpoints = isi->xpoints;
   int                *xapoints = isi->xapoints;
   int                *yapoints = isi->yapoints;

   end = dxx + dw;
   /* scaling up both ways */
   if (isi->xup_yup == 3) {
	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];
		   if (YAP > 0) {
			   for (x = dxx; x < end; x++) {
				   int                 r, g, b, a;
				   int                 rr, gg, bb, aa;
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
				   int                 r, g, b, a;
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
   }
   /* if we're scaling down vertically */
   else if (isi->xup_yup == 1) {
	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   int                 yap;

		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];

		   yap = (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
		   if (yap > 1) {
			   for (x = dxx; x < end; x++) {
				   int                 r = 0, g = 0, b = 0, a = 0;
				   int                 rr = 0, gg = 0, bb = 0, aa = 0;
				   DATA32             *pix;

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
				   int                 count;
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
   }
   /* if we're scaling down horizontally */
   else if (isi->xup_yup == 2) {
	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];
		   if (YAP > 0) {
			   for (x = dxx; x < end; x++) {
				   int                 r = 0, g = 0, b = 0, a = 0;
				   int                 rr = 0, gg = 0, bb = 0, aa = 0;
				   int                 xap;
				   DATA32             *pix;

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
				   int                 r = 0, g = 0, b = 0, a = 0;
				   int                 xap;
				   DATA32             *pix;

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
   }
   /* if we're scaling down horizontally & vertically */
   else {
	   int                 count;
	   DATA32             *pix;
	   int                 a, r, g, b;

	   /* go through every scanline in the output buffer */
	   for (y = 0; y < dh; y++) {
		   int                 yap =
			   (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
		   /* calculate the source line we'll scan from */
		   dptr = dest + dx + ((y + dy) * dow);
		   sptr = ypoints[dyy + y];
		   for (x = dxx; x < end; x++) {
			   int                 xap = xpoints[x + 1] - xpoints[x];

			   if ((xap > 1) || (yap > 1)) {
				   r = 0;
				   g = 0;
				   b = 0;
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
   }
}

#define LINESIZE 16

gimp_image_t *scale_image(
		gimp_image_t *src,
		int ssx, int ssy, int ssw, int ssh,
		int ddx, int ddy, int ddw, int ddh,
		int clx, int cly, int clw, int clh)
{
	char                rgb_src = 0;

	ImlibScaleInfo     *scaleinfo = NULL;
	DATA32             *buf = NULL;
	int                 sx, sy, sw, sh, dx, dy, dw, dh, dxx, dyy, y2, x2;
	int                 psx, psy, psw, psh;
	int                 y, h, hh;
	gimp_image_t       *dst;
	_pel				init;

	init.c = 0;
	dst = pmap_new(abs(destination_width), abs(destination_height), init);
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
		return;
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
		return;
	/* clip output coords to clipped input coords */
	psx = dx;
	psy = dy;
	psw = dw;
	psh = dh;
	x2 = sx;
	y2 = sy;
	CLIP(dx, dy, dw, dh, 0, 0, dst->w, dst->h);
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
		return;
	if (clw) {
		CLIP_TO(dx, dy, dw, dh, clx, cly, clw, clh);
		if ((dw < 1) || (dh < 1))
			return;
	}
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
		return;
	scaleinfo = __imlib_CalcScaleInfo(src, ssw, ssh, ddw, ddh);
	if (!scaleinfo)
		return;
	/* if we are scaling the image at all make a scaling buffer */
	/* allocate a buffer to render scaled RGBA data into */
	buf = malloc(dw * LINESIZE * sizeof(DATA32));
	if (!buf) {
		__imlib_FreeScaleInfo(scaleinfo);
		return;
	}
	/* setup h */
	h = dh;
	/* scale in LINESIZE Y chunks and convert to depth */
	for (y = 0; y < dh; y += LINESIZE) {
		hh = LINESIZE;
		if (h < LINESIZE)
			hh = h;
		/* scale the imagedata for this LINESIZE lines chunk of image */
		__imlib_ScaleAARGBA(scaleinfo, dst->pixel_data, dxx, dyy + y,
				0, 0, dw, hh, dw, src->width);

		h -= LINESIZE;
	}
	/* free up our buffers and point tables */
	free(buf);
	__imlib_FreeScaleInfo(scaleinfo);

   return dst;
}

