#include <pixelInt.h>

#define PI			3.141592653589793

#define Wt			9
#define W			(Wt*2+1)
#define FIXED_BITS	22		// Max is min(fixedpoint's bits-10, uint64_t's bits/2-1)
#define FIXED_TO_DOUBLE(f) ((f) / (double)(1LL<<FIXED_BITS))

static inline int min(const int i1, const int i2) { return i1 < i2 ? i1 : i2; }

typedef int32_t fixedpoint;

static float w1(const float fc, const float x) //{{{
{
	return x==0 ? 2*fc : sinf(2*PI*fc*x) / (PI*x);
}

//}}}
static float w2(const int wt, const float x) //{{{
{
	if (x < -wt || x > wt) {
		return 0.0;
	} else if (x == 0) {
		return 1.0;
	} else {
		const float X = PI * (x/wt);
		return sinf(X) / X;
	}
}

//}}}
static float h(const int wt, const float fc, const float x) //{{{
{
	return w1(fc, x) * w2(wt, x);
}

//}}}
static void make_kern(fixedpoint* kern, int wt, const float f, const float frac, const int kstart, const int kend) //{{{
{
	int			k;
	fixedpoint	acc = 0;

	// Calculate the kernel weights for this fractional v
	for (k=kstart; k<kend; k++)
		acc += (kern[k] = h(wt, f, -wt+k-frac) * (1LL<<FIXED_BITS));

	// Normalize the kernel to eliminate bias
	for (k=kstart; k<kend; k++)
		kern[k] = ((int64_t)(kern[k])<<FIXED_BITS) / acc;
}

//}}}
static void make_kern_float(float* restrict kern, const int wt, const float f, const float frac, const int kstart, const int kend) //{{{
{
	int			k;
	float		acc = 0;

	// Calculate the kernel weights for this fractional v
	for (k=kstart; k<kend; k++)
		acc += (kern[k] = h(wt, f, -wt+k-frac));

	// Normalize the kernel to eliminate bias
	for (k=kstart; k<kend; k++)
		kern[k] /= acc;
}

//}}}
static void make_kern_float2(float* restrict kern, const int wt, const float f, const float frac, const int kstart, const int kend) //{{{
{
	int			k;
	float		acc = 0;

	// Calculate the kernel weights for this fractional v
	for (k=kstart; k<kend; k++)
		acc += (kern[k] = h(wt, f, -wt+k+frac));

	// Normalize the kernel to eliminate bias
	for (k=kstart; k<kend; k++)
		kern[k] /= acc;
}

//}}}
static int glue_kern_vis(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	out = NULL;
	int				width, height, x, k, yofs, from_x, from_y, to_x, to_y, wt, w;
	fixedpoint*		kern = NULL;
	double			frac, f, kf, k0;
	fixedpoint		min = 256LL<<FIXED_BITS, max = -(256LL<<FIXED_BITS);
	double			minf, maxf, xf, yf;
	_pel			red = {.c = 0xffff0000};
	_pel			green = {.c = 0xff00ff00};
	_pel			lightgreen = {.c = 0xffa0ffa0};
	_pel			black = {.c = 0xffffffff};
	_pel			white = {.c = 0xff000000};
	_pel			grey = {.c = 0xffa0a0a0};
	_pel			yellow = {.c = 0xffffff00};

	enum {A_cmd, A_WIDTH, A_HEIGHT, A_WT, A_F, A_FRAC, A_objc}; CHECK_ARGS("width height wt f frac");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &height));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WT], &wt));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_F], &f));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_FRAC], &frac));

	w = wt*2+1;
	kern = ckalloc(sizeof(fixedpoint) * w);

	out = pmap_new(width, height, (_pel){.c = 0});
	pmap_clr(out, white);
	memset(kern, 0, w);
	make_kern(kern, wt, f, frac, 0, w);

	for (k=0; k<w; k++) {
		if (kern[k] < min) min = kern[k];
		if (kern[k] > max) max = kern[k];
	}

	minf = FIXED_TO_DOUBLE(min);
	maxf = FIXED_TO_DOUBLE(max);

	yf = 0.7 * (height-1) / (maxf-minf);
	xf = (width-1) / (double)w;

	yofs = maxf*yf + height*((1-0.7)/2.0);

	// Y: kern to out space: y_out = yofs - y*yf
	// X: kern to out space: x_out = x*xf

	/*
	0 = -wt + kf - frac
	-kf = -wt -frac
	kf = wt + frac
	w * x/width = wt + frac
	x = (width * (wt + frac)) / w
	*/
	// Zero in kernel space
	k0 = (width * (wt + frac)) / w +.5;
	line(k0, 0, k0, height-1, green, out);
	for (kf = k0+xf; kf < width; kf += xf)
		line(kf, 0, kf, height-1, lightgreen, out);
	for (kf = k0-xf; kf > 0; kf -= xf)
		line(kf, 0, kf, height-1, lightgreen, out);

	// Zero line
	line(0, yofs, width-1, yofs, red, out);

	// Kernel taps
	k = 0;
	from_x = (k * xf)+.5;
	from_y = (yofs - FIXED_TO_DOUBLE(kern[k])*yf)+.5;
	line(from_x, 0, from_x, height-1, yellow, out);
	for (k=1; k<w; k++) {
		to_x = (k * xf)+.5;
		to_y = (yofs - FIXED_TO_DOUBLE(kern[k])*yf)+.5;

		//fprintf(stderr, "(%d, %d) -> (%d, %d)\n", from_x, from_y, to_x, to_y);
		line_aa(from_x, from_y, to_x, to_y, black, out);

		from_x = to_x;
		from_y = to_y;
		line(from_x, 0, from_x, height-1, yellow, out);
	}

	// Kernel
	from_x = 0;
	from_y = (yofs - h(wt, f, -wt+k-frac) * yf)+.5;
	for (x=1; x<width; x++) {
		kf = w * x/(double)width;

		to_x = x;
		to_y = (yofs - h(wt, f, -wt+kf-frac) * yf)+.5;

		if (to_x < 0) to_x = 0;
		if (to_y < 0) to_y = 0;
		if (to_x >= width) to_x = width-1;
		if (to_y >= height) to_y = height-1;

		//fprintf(stderr, "(%d, %d) -> (%d, %d)\n", from_x, from_y, to_x, to_y);
		line_aa(from_x, from_y, to_x, to_y, grey, out);

		from_x = to_x;
		from_y = to_y;
	}

	ckfree(kern);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(out));
	return TCL_OK;
}

//}}}
/*
static int glue_scale_pmap_lanczos2(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	src = NULL;
	gimp_image_t*	dst = NULL;
	int				new_w, new_h, xi, yi, k;
	float			x, y, xinc, yinc;	// interpolated coordinates in the source for the new sample
	float			xf, yf;
	_pel*			t;
	fixedpoint		kern[W];

	enum {A_cmd, A_PMAP, A_WIDTH, A_HEIGHT, A_objc}; CHECK_ARGS("pmap width height");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &new_h));

	// Intermediate buffer - scaled down on the horizontal axis
	//dst = pmap_new(new_w, src->height, (_pel){.c = 0});
	//dst = pmap_new(src->width, src->height, (_pel){.c = 0});
	dst = pmap_new(new_w, new_h, (_pel){.c = 0});

	//t = (_pel*)malloc(src->width*src->height*sizeof(_pel));
	t = (_pel*)malloc(dst->width*src->height*sizeof(_pel));

	//t = dst->pixel_data;

	xf = (float)new_w / (2.0*src->width);
	xinc = src->width / (float)dst->width;
	for (xi=0, x=0; xi<dst->width; xi++, x+=xinc) {
		_pel*		d = t + xi;		// Points at the top of the column
		fixedpoint	acc=0;

		for (k=0; k<W; k++) {
			float	xk = fmodf(x, 1.0) + k - Wt;
			if (x+xk < src->width && x+xk >= 0)
				acc += (kern[k] = h(xf, xk) * (1LL<<FIXED_BITS));
		}

		// Normalize the kernel to eliminate bias
		for (k=0; k<W; k++)
			kern[k] = ((int64_t)(kern[k])<<FIXED_BITS) / acc;

		// Produce a column (xi) of the intermediate image
		for (yi=0; yi<src->height; yi++, d += dst->width) {
			// +.5 - trick to do cheap rounding.  round() is astonishingly slow
			int			k=0, xr=x+.5, kend=W;
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			if (xr+kend > src->width)
				kend = src->width - xr;	// Adjust the end to the end of the row

			if (xr-Wt < 0) {	// Adjist the start to the start of the row
				k = Wt - xr;
				xr = Wt;
			}

			s = src->pixel_data + yi*src->width + (xr-Wt);

			for (;  k<kend;  k++, s++) {
				if (s->ch.a == 0) continue;

				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
				a += s->ch.a * kern[k];
			}

			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a < 0) a=0;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;

			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
			d->ch.a = a>>FIXED_BITS;
		}
	}

	yf = (float)new_h / (2.0*src->height);
	yinc = src->height / (float)dst->height;
	for (yi=0, y=0; yi<dst->height; yi++, y+=yinc) {
		_pel*		d = dst->pixel_data + yi*dst->width;	// Points at the start of the row
		fixedpoint	acc=0;

		for (k=0; k<W; k++) {
			float yk = fmodf(y, 1.0) + k - Wt;
			if (y+yk < src->height && y+yk >= 0)
				acc += (kern[k] = h(yf, yk) * (1LL<<FIXED_BITS));
		}

		// Normalize the kernel to eliminate bias
		for (k=0; k<W; k++)
			kern[k] = ((int64_t)(kern[k])<<FIXED_BITS)/acc;

		// Produce a row (yi) of the final image
		for (xi=0; xi<dst->width; xi++, d++) {
			int			k=0, kend=W, yr=y+.5;
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			if (yr+kend > src->height)
				kend = src->height - yr; // Adjust the end to the bottom of the buffer

			if (Wt > yr) { // Adjust the start to the top of the buffer
				k = Wt - yr;
				yr = Wt;
			}

			s = t + (yr-Wt)*dst->width + xi;

			for (; k<kend; k++, s+=dst->width) {
				if (s->ch.a == 0) continue;

				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
				a += s->ch.a * kern[k];
			}

			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a < 0) a=0;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;

			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
			d->ch.a = a>>FIXED_BITS;
		}
	}

	free(t); t = NULL;
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dst));

	return TCL_OK;
}

//}}}
*/
void lanczos_half_scale(_pel* in, _pel* out, int orig_dim, int new_dim, int wt, int dsinc, int iiend, int dinc, int sinc, int sif) //{{{
{
	int				w = wt*2+1;
	fixedpoint		kern[w];
	int				oi, ii;
	float			v;
	const float		vinc = orig_dim/(float)new_dim, f = fminf(.45, new_dim/(2.0*orig_dim));
	_pel*			ds = out;

	// Loop across the dimension to be scaled (oi in "out" coords, v in "in" coords)
	// v is the fractional projection of oi into "in" coord space
	for (oi=0, v=0; oi<new_dim; oi++, v+=vinc, ds+=dsinc) {
		_pel*		d=ds;
		//fixedpoint	acc=0;
		int			vi=v, kend=w, k, kstart=0;
		float		frac=v-vi;

		// Adjust the start and end ranges of k to fit within the soure buffer
		if (vi-wt+kend > orig_dim)
			kend = orig_dim - vi + wt;

		if (wt > vi)
			kstart = wt - vi;

		make_kern((fixedpoint*)&kern, wt, f, frac, kstart, kend);

		// Loop across the fixed dimension (ii in "out" coords)
		for (ii=0; ii<iiend; ii++, d+=dinc) {
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			// Point at the first source pixel for the kernel
			s = in + (vi-wt+kstart)*sinc + ii*sif;

			// Accumulate samples from "in", scaled by the kernel weights
			for (k=kstart; k<kend; k++, s+=sinc) {
				if (s->ch.a == 0) continue;		// Prevents auto-vectorization, but a big nett win if many pixels are completely transparent
				a += s->ch.a * kern[k];
				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
			}

			// Clamp output values
			if (a < 0) a=0;
			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;

			// ADC (add with carry) would really help here with the rounding
			/*
			d->ch.a = (a>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.r = (r>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.g = (g>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.b = (b>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			*/
			d->ch.a = a>>FIXED_BITS;
			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
		}
	}
}


//}}}
static inline void _acc_from_in(float* restrict kern, const int kstart, const int kend, pelf* restrict s, float* restrict dp, const int sinc) //{{{
{
	int c, k;

	for (k=kstart; k<kend; k++, s+=sinc) {
		const float kernval = kern[k];
		float* restrict sp = (float*)s;

		for (c=0; c<4; c++)
			dp[c] += sp[c] * kernval;
	}
}

//}}}
static inline void _loop_over_fixed_dim(float* restrict kern, const int kstart, const int kend, pelf* restrict ss, pelf* restrict d, const int sif, const int sinc, const int dinc, const int iiend) //{{{
{
	int i;

	// Loop across the fixed dimension (ii in "out" coords)
	for (i=0; i<iiend; i++, d+=dinc, ss+=sif) {
		int			c;
		float* restrict dp = (float*)d;

		for (c=0; c<4; c++)
			dp[c] = 0;

		// Accumulate samples from "in", scaled by the kernel weights
		_acc_from_in(kern, kstart, kend, ss, dp, sinc);

		/*
		// Clamp output values
		for (c=0; c<4; c++) {
			if (d->chan[c] < 0) {
				d->chan[c] = 0;
			} else if (d->chan[c] > 1.0) {
				d->chan[c] = 1.0;
			}
		}
		*/
	}
}

//}}}
static void lanczos_half_scale_float(pelf* restrict in, pelf* restrict out, const int orig_dim, const int new_dim, const int wt, const float f, const int dsinc, const int iiend, const int dinc, const int sinc, const int sif) //{{{
{
	const int				w = wt*2+1;
	float					kern[w];
	int						oi;
	float					v;
	const float				vinc = orig_dim/(float)new_dim;
	pelf* restrict			ds = out;

	//fprintf(stderr, "lanczos_half_scale_float: %d -> %d: f: %.4f\n", orig_dim, new_dim, f);
	// Loop across the dimension to be scaled (oi in "out" coords, v in "in" coords)
	// v is the fractional projection of oi into "in" coord space
	for (oi=0, v=0; oi<new_dim; oi++, v+=vinc, ds+=dsinc) {
		pelf* restrict		ss;
		pelf* restrict		d=ds;
		int					vi=v, kend=w, kstart=0;
		const float			frac=v-vi;

		// Adjust the start and end ranges of k to fit within the soure buffer
		if (vi-wt+kend > orig_dim)
			kend = orig_dim - vi + wt;

		if (wt > vi)
			kstart = wt - vi;

		make_kern_float((float*)&kern, wt, f, frac, kstart, kend);

		// Point at the first source pixel for the kernel
		ss = in + (vi-wt+kstart)*sinc;

		_loop_over_fixed_dim(kern, kstart, kend, ss, d, sif, sinc, dinc, iiend);
	}
}


//}}}
void lanczos_half_scale_f(_pel* in, _pel* out, const int orig_dim, const int new_dim, const int wt, const float f, const int dsinc, const int iiend, const int dinc, const int sinc, const int sif) //{{{
{
	const int		w = wt*2+1;
	fixedpoint		kern[w];
	int				oi, ii;
	float			v;
	const float		vinc = orig_dim/(float)new_dim;
	_pel*			ds = out;

	// Loop across the dimension to be scaled (oi in "out" coords, v in "in" coords)
	// v is the fractional projection of oi into "in" coord space
	for (oi=0, v=0; oi<new_dim; oi++, v+=vinc, ds+=dsinc) {
		_pel*		d=ds;
		//fixedpoint	acc=0;
		int			vi=v, kend=w, k, kstart=0;
		float		frac=v-vi;

		// Adjust the start and end ranges of k to fit within the soure buffer
		if (vi-wt+kend > orig_dim)
			kend = orig_dim - vi + wt;

		if (wt > vi)
			kstart = wt - vi;

		make_kern((fixedpoint*)&kern, wt, f, frac, kstart, kend);

		// Loop across the fixed dimension (ii in "out" coords)
		for (ii=0; ii<iiend; ii++, d+=dinc) {
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			// Point at the first source pixel for the kernel
			s = in + (vi-wt+kstart)*sinc + ii*sif;

			// Accumulate samples from "in", scaled by the kernel weights
			for (k=kstart; k<kend; k++, s+=sinc) {
				if (s->ch.a == 0) continue;		// Prevents auto-vectorization, but a big nett win if many pixels are completely transparent
				a += s->ch.a * kern[k];
				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
			}

			// Clamp output values
			if (a < 0) a=0;
			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;

			// ADC (add with carry) would really help here with the rounding
			/*
			d->ch.a = (a>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.r = (r>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.g = (g>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.b = (b>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			*/
			d->ch.a = a>>FIXED_BITS;
			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
		}
	}
}


//}}}
static int glue_scale_pmap_lanczos3(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	src = NULL;
	gimp_image_t*	dst = NULL;
	gimp_image_t*	t = NULL;
	int				new_w, new_h, wt;

	enum {A_cmd, A_PMAPF, A_WIDTH, A_HEIGHT, A_args, A_WT=A_args, A_objc};
	CHECK_RANGE_ARGS("pmapf width height ?wt?");


	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &new_h));
	if (objc >= 5) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WT], &wt));
	} else {
		const int src_w = (int)src->width;

		if (new_w > src_w) {
			wt = 3;
		} else if (new_w * 8 < src_w) {
			wt = 15;
		} else if (new_w * 5 < src_w) {
			wt = 13;
		} else if (new_w * 3 < src_w) {
			wt = 10;
		} else {
			wt = 9;
		}
	}

	// Intermediate buffer - scaled down on the horizontal axis
	//dst = pmap_new(new_w, src->height, (_pel){.c = 0});
	//dst = pmap_new(src->width, src->height, (_pel){.c = 0});
	dst = pmap_new(new_w, new_h, (_pel){.c = 0});

	//t = pmap_new(src->width, src->height, (_pel){.c = 0});

	/*
	t = pmap_new(dst->width, src->height, (_pel){.c = 0});
	// first: scale width
	lanczos_half_scale(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			1,					// dsinc
			src->height,		// iiend
			dst->width,			// dinc
			1,					// sinc
			src->width			// sif
			);

	// second: scale height
	lanczos_half_scale(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			dst->width,			// dsinc
			dst->width,			// iiend
			1,					// dinc
			dst->width,			// sinc
			1					// sif
			);
	*/

	t = pmap_new(src->width, dst->height, (_pel){.c = 0});
	// first: scale height
	lanczos_half_scale(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	//Tcl_SetObjResult(interp, Tcl_NewPMAPObj(t));
	//return TCL_OK;

	// second: scale width
	lanczos_half_scale(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);


	pmap_free(&t);
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dst));

	return TCL_OK;
}

//}}}
static int glue_lowpass_pmap_lanczos3(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	src = NULL;
	gimp_image_t*	dst = NULL;
	gimp_image_t*	t = NULL;
	double			f;

	enum {A_cmd, A_PMAP, A_F, A_objc}; CHECK_ARGS("pmap f");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &src));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_F], &f));

	dst = pmap_new(src->width, src->height, (_pel){.c = 0});
	t = pmap_new(src->width, dst->height, (_pel){.c = 0});

	// first: scale height
	lanczos_half_scale_f(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			Wt,
			f,					// f
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	//Tcl_SetObjResult(interp, Tcl_NewPMAPObj(t));
	//return TCL_OK;

	// second: scale width
	lanczos_half_scale_f(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			Wt,
			f,					// f
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);


	pmap_free(&t);
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dst));

	return TCL_OK;
}

//}}}
static int glue_scale_pmapf_lanczos3(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	struct pmapf*	t = NULL;
	int				new_w, new_h, wt;

	enum {A_cmd, A_PMAPF, A_WIDTH, A_HEIGHT, A_args, A_WT=A_args, A_objc};
	CHECK_RANGE_ARGS("pmapf width height ?wt?");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &new_h));
	if (objc >= 5) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WT], &wt));
	} else {
		const int src_w = (int)src->width;

		if (new_w > src_w) {
			wt = 4;
		} else if (new_w * 8 < src_w) {
			wt = 15;
		} else if (new_w * 5 < src_w) {
			wt = 13;
		} else if (new_w * 3 < src_w) {
			wt = 10;
		} else {
			wt = 9;
		}
	}




#if 0
	dst = pmapf_new(new_w, new_h);
	t = pmapf_new(src->width, dst->height);

	// first: scale height
	lanczos_half_scale_float(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			fminf(.5, dst->height/(2.0*src->height)),	// f
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	// second: scale width
	lanczos_half_scale_float(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			fminf(.5, dst->width/(2.0*t->width)),	// f
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);

	pmapf_free(&t);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;

#else



	// Scale to the smallest output dimension first (reduces the peak memory requirements)
	if (new_w > new_h) {
		t = pmapf_new(src->width, new_h);

		// first: scale height
		lanczos_half_scale_float(
				src->pixel_data,	// in
				t->pixel_data,		// out
				src->height,		// orig_dim
				new_h,				// new_dim
				wt,
				fminf(.5, new_h/(2.0*src->height)),	// f
				t->width,			// dsinc
				t->width,			// iiend
				1,					// dinc
				src->width,			// sinc
				1					// sif
				);

		dst = pmapf_new(new_w, new_h);

		// second: scale width
		lanczos_half_scale_float(
				t->pixel_data,		// in
				dst->pixel_data,	// out
				t->width,			// orig_dim
				dst->width,			// new_dim
				wt,
				fminf(.5, dst->width/(2.0*t->width)),	// f
				1,					// dsinc
				t->height,			// iiend
				dst->width,			// dinc
				1,					// sinc
				t->width			// sif
				);
	} else {
		t = pmapf_new(new_w, src->height);

		// first: scale width
		lanczos_half_scale_float(
				src->pixel_data,	// in
				t->pixel_data,		// out
				src->width,			// orig_dim
				new_w,				// new_dim
				wt,
				fminf(.5, new_w/(2.0*src->width)),	// f
				1,					// dsinc
				t->height,			// iiend
				new_w,				// dinc
				1,					// sinc
				src->width			// sif
				);

		dst = pmapf_new(new_w, new_h);

		// second: scale height
		lanczos_half_scale_float(
				t->pixel_data,		// in
				dst->pixel_data,	// out
				t->height,			// orig_dim
				dst->height,		// new_dim
				wt,
				fminf(.5, dst->height/(2.0*t->height)),	// f
				dst->width,			// dsinc
				t->width,			// iiend
				1,					// dinc
				t->width,			// sinc
				1					// sif
				);
	}

	pmapf_free(&t);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
#endif
}

//}}}
static int glue_shear_pmapf_lanczos_x(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	double			shear;
	const int		wt=4;
	const int		w=wt*2+1;
	float			kern_norm[w];
	int				xs, xd, y, skip_start=0, skip_end=0;

	enum {A_cmd, A_PMAPF, A_SHEAR, A_args, A_SKIP_START=A_args, A_SKIP_END, A_objc};
	CHECK_RANGE_ARGS("pmapf shear ?skip_start? ?skip_end?");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_SHEAR], &shear));
	if (objc > 3) TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SKIP_START], &skip_start));
	if (objc > 4) TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SKIP_END], &skip_end));

	dst = pmapf_new(src->width + (int)(ceil(fabs(shear*src->height))) - skip_start - skip_end, src->height);

	{
		const int	src_w = src->width;
		const int	src_h = src->height;
		const int	dst_w = dst->width;

		for (y=0; y<src_h; y++) {
			const float xf = shear < 0 ? (src_h-y-1)*shear*-1 : y*shear;
			//const int xdstart = (int)floor(xf);				// [xdstart, xdend] - range of dst pixels that take input from src
			//const int xdend   = src_w + (int)ceil(xf);
			pelf* restrict	d = dst->pixel_data + y*dst_w - skip_start;
			pelf* restrict	s/* = src->pixel_data + y*src_w*/;
			int				c;
			const int		xi = xf;
			const float		frac = (xf-xi);

			make_kern_float2(kern_norm, wt, .5, frac, 0, w);

			// empty area (might actually have to process samples that are within wt of the src edge) {{{
			xd = -skip_start;
			xs = -xi;
			if (xs<-wt && xd<0) {
				const int adj = min(-wt-xs, -xd);
				xd += adj;
				xs += adj;
				d += adj;
			}
			for (; xs<-wt && xd<dst_w; xd++, xs++, d++) {
				float* restrict dp = d->chan;
				for (c=0; c<4; c++)
					dp[c] = 0.0;
			}
			//}}}

			// first section: kern out of bounds on left in src (also maybe right if src->width < w) {{{
			{
				int			kstart;

				s = src->pixel_data + y*src_w + xs;

				kstart = wt - xs;
				if (xs<wt && xd<0) {
					const int adj = min(wt-xs, -xd);
					xd += adj;
					xs += adj;
					d += adj;
					s += adj;
					kstart -= adj;
				}
				for (
						;
						xs<wt && xd<dst_w;
						xd++, xs++, d++, s++, kstart--
				) {
					const int kend = min(w, (src_w-xs)+wt);		// +wt - account for the ofs kend is relative to: xs-wt
					float* restrict dp = d->chan;
					pelf* restrict	ss = s-wt+kstart;

					for (c=0; c<4; c++)
						dp[c] = 0;

					// Accumulate samples from "in", scaled by the kernel weights
					_acc_from_in(kern_norm, kstart, kend, ss, dp, 1);
				}
			}
			//}}}

			// inner section: full coverage in src for kern {{{
			{
				const int	kstart=0;
				const int	kend=w;

				if (xs<src_w-wt-1 && xd<0) {
					const int adj = min((src_w-wt-1) - xs, -xd);
					xd += adj;
					xs += adj;
					s += adj;
					d += adj;
				}
				for (; xs<src_w-wt-1 && xd<dst_w; xd++, xs++, s++, d++) {
					float* restrict	dp = d->chan;
					pelf* restrict	ss = s-wt;

					for (c=0; c<4; c++)
						dp[c] = 0;

					// Accumulate samples from "in", scaled by the kernel weights
					_acc_from_in(kern_norm, kstart, kend, ss, dp, 1);
				}
			}
			//}}}

			// third section: kern out of bounds on right in src (also maybe left if src->width < w) {{{
			{
				int			kend = w;

				if (xs<src_w+wt && xd<0) {
					const int adj = min (src_w+wt-xs, -xd);
					xd += adj;
					xs += adj;
					s += adj;
					d += adj;
					kend -= adj;
				}
				for (
						;
						xs<src_w+wt && xd<dst_w;
						xd++, xs++, s++, d++, kend--
				) {
					const int kstart = -min(0, xs-wt);
					float* restrict dp = d->chan;
					pelf* restrict	ss = s-(wt-kstart);

					for (c=0; c<4; c++)
						dp[c] = 0;

					// Accumulate samples from "in", scaled by the kernel weights
					_acc_from_in(kern_norm, kstart, kend, ss, dp, 1);
				}
			}
			//}}}

			// empty area {{{
			if (xd < 0) {
				const int adj = -xd;
				xd += adj;
				d += adj;
			}
			for (; xd<dst_w; xd++, d++) {
				float* restrict dp = d->chan;
				for (c=0; c<4; c++)
					dp[c] = 0.0;
			}
			//}}}
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));
	return TCL_OK;
}

//}}}
static int glue_shear_pmapf_lanczos_y(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	double			shear;
	const int		wt=4;
	const int		w=wt*2+1;
	float			kern_norm[w];
	int				ys, yd, x, skip_start=0, skip_end=0;

	enum {A_cmd, A_PMAPF, A_SHEAR, A_args, A_SKIP_START=A_args, A_SKIP_END, A_objc};
	CHECK_RANGE_ARGS("pmapf shear ?skip_start? ?skip_end?");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_SHEAR], &shear));
	if (objc > 3) TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SKIP_START], &skip_start));
	if (objc > 4) TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SKIP_END], &skip_end));

	dst = pmapf_new(src->width, src->height + (int)(ceil(fabs(shear*src->width))) - skip_start - skip_end);

	{
		const int	src_w = src->width;
		const int	src_h = src->height;
		const int	dst_w = dst->width;
		const int	dst_h = dst->height;

		for (x=0; x<src_w; x++) {
			const float yf = shear < 0 ? (src_w-x-1)*shear*-1 : x*shear;
			//const int ydstart = (int)floor(yf);				// [ydstart, ydend] - range of dst pixels that take input from src
			//const int ydend   = src_h + (int)ceil(yf);
			pelf* restrict	d = dst->pixel_data + x - skip_start*dst_w;
			pelf* restrict	s/* = src->pixel_data + x*/;
			int				c;
			const int		yi = yf;
			const float		frac = (yf-yi);

			make_kern_float2(kern_norm, wt, .5, frac, 0, w);

			// empty area (might actually have to process samples that are within wt of the src edge) {{{
			yd = -skip_start;
			ys = -yi;
			if (yd < 0) {
				yd += skip_start;
				ys += skip_start;
				d += dst_w * skip_start;
			}
			for (; ys<-wt && yd<dst_h; yd++, ys++, d+=dst_w) {
				float* restrict dp = d->chan;
				for (c=0; c<4; c++)
					dp[c] = 0.0;
			}
			//}}}

			// first section: kern out of bounds on left in src (also maybe right if src->width < w) {{{
			{
				int			kstart;

				s = src->pixel_data + ys*src_w + x;

				kstart = wt - ys;
				if (yd < 0 && ys<wt) {
					const int adj = min(-yd, wt-ys);
					yd += adj;
					ys += adj;
					d += dst_w*adj;
					s += src_w*adj;
					kstart -= adj;
				}
				for (
						;
						ys<wt && yd<dst_h;
						yd++, ys++, d+=dst_w, s+=src_w, kstart--
				) {
					const int kend = min(w, (src_h-ys)+wt);		// +wt - account for the ofs kend is relative to: ys-wt
					float* restrict dp = d->chan;
					pelf* restrict	ss = s+((-wt+kstart)*src_w);

					for (c=0; c<4; c++)
						dp[c] = 0;

					// Accumulate samples from "in", scaled by the kernel weights
					_acc_from_in(kern_norm, kstart, kend, ss, dp, src_w);
				}
			}
			//}}}

			// inner section: full coverage in src for kern {{{
			{
				const int	kstart=0;
				const int	kend=w;

				if (yd < 0 && ys < src_h-wt-1) {
					const int adj = min(-yd, (src_h-wt-1) - ys);
					yd += adj;
					ys += adj;
					s += src_w*adj;
					d += dst_w*adj;
				}
				for (; ys<src_h-wt-1 && yd<dst_h; yd++, ys++, s+=src_w, d+=dst_w) {
					float* restrict	dp = d->chan;
					pelf* restrict	ss = s-(wt*src_w);

					for (c=0; c<4; c++)
						dp[c] = 0;

					// Accumulate samples from "in", scaled by the kernel weights
					_acc_from_in(kern_norm, kstart, kend, ss, dp, src_w);
				}
			}
			//}}}

			// third section: kern out of bounds on right in src (also maybe left if src->width < w) {{{
			{
				int			kend = w;

				if (yd < 0 && ys<src_h+wt) {
					const int adj = min(-yd, src_h+wt - ys);
					yd += adj;
					ys += adj;
					s += src_w*adj;
					d += dst_w*adj;
					kend -= adj;
				}
				for (
						;
						ys<src_h+wt && yd<dst_h;
						yd++, ys++, s+=src_w, d+=dst_w, kend--
				) {
					const int kstart = -min(0, ys-wt);
					float* restrict dp = d->chan;
					pelf* restrict	ss = s-((wt-kstart)*src_w);

					for (c=0; c<4; c++)
						dp[c] = 0;

					// Accumulate samples from "in", scaled by the kernel weights
					_acc_from_in(kern_norm, kstart, kend, ss, dp, src_w);
				}
			}
			//}}}

			// empty area {{{
			if (yd<0) {
				const int adj = -yd;
				yd += adj;
				d += adj*dst_w;
			}
			for (; yd<dst_h; yd++, d+=dst_w) {
				float* restrict dp = d->chan;
				for (c=0; c<4; c++)
					dp[c] = 0.0;
			}
			//}}}
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));
	return TCL_OK;
}

//}}}
static int glue_lowpass_pmapf_lanczos3(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	struct pmapf*	t = NULL;
	double			f;
	int				wt;

	enum {A_cmd, A_PMAPF, A_WT, A_F, A_objc}; CHECK_ARGS("pmapf wt f");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WT], &wt));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_F], &f));

	dst = pmapf_new(src->width, src->height);
	t = pmapf_new(src->width, dst->height);

	// first: scale height
	lanczos_half_scale_float(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			f,					// f
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	// second: scale width
	lanczos_half_scale_float(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			f,					// f
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);


	pmapf_free(&t);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}

int lanczos_init(Tcl_Interp *interp) // {{{1
{
	NEW_CMD("pixel::scale_pmapf_lanczos", glue_scale_pmapf_lanczos3);
	NEW_CMD("pixel::lowpass_pmapf_lanczos", glue_lowpass_pmapf_lanczos3);
	NEW_CMD("pixel::shear_pmapf_lanczos_x", glue_shear_pmapf_lanczos_x);
	NEW_CMD("pixel::shear_pmapf_lanczos_y", glue_shear_pmapf_lanczos_y);

	NEW_CMD("pixel::scale_pmap_lanczos", glue_scale_pmap_lanczos3);
	NEW_CMD("pixel::lowpass_pmap_lanczos", glue_lowpass_pmap_lanczos3);
	NEW_CMD("pixel::kern_vis", glue_kern_vis);

	return TCL_OK;
}

