// Separable resampling of pmapf images (linear light), as two passes over
// precomputed kernel tables: one entry per output pixel, giving the first input
// pixel it draws on and the weights of the run of input pixels from there.
//
// Pixel coordinates are centre-aligned: pixel p covers [p, p+1) and its sample
// is the light at p+0.5, so output pixel o of a dimension scaled from orig to
// new (ratio r = orig/new) is centred at input coordinate (o+0.5)*r, which is
// input index (o+0.5)*r - 0.5.  Output and input cover the same extent, so a
// mirror symmetric image stays symmetric.
//
// The kernel is the windowed sinc of lanczos.c: a sinc cut off at the output's
// Nyquist frequency, under a sinc window wt input pixels wide each side,
// truncated at the image edges and renormalized.
//
// (A linear-light box reduction ahead of the lanczos passes for large ratios was
// tried, and was slower at every ratio once these passes were cache-blocked and
// vectorized: their cost hardly grows with the ratio.)
//
// The passes are written for NEON on aarch64 (a pelf is one float32x4_t), with
// a portable C version that compilers vectorize for other targets.

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <pixelInt.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "scale.h"

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#	include <arm_neon.h>
#	define SCALE_NEON	1
#else
#	define SCALE_NEON	0
#endif

#define PI_D	3.14159265358979323846

struct taps {
	int		n;			// outputs
	int		stride;		// weights stored per output (the most taps of any)
	int*	start;		// first input index of each output
	int*	count;		// taps of each output
	float*	w;			// n*stride weights, each output's run starting at w + o*stride
};

static void* xmalloc(size_t bytes) //{{{
{
	void*	p = malloc(bytes ? bytes : 1);
	if (p == NULL) Tcl_Panic("Could not allocate %zu bytes for a scale kernel", bytes);
	return p;
}

//}}}
static struct taps* taps_new(int n, int stride) //{{{
{
	struct taps*	t = xmalloc(sizeof *t);
	t->n		= n;
	t->stride	= stride;
	t->start	= xmalloc(sizeof(int) * n);
	t->count	= xmalloc(sizeof(int) * n);
	t->w		= xmalloc(sizeof(float) * (size_t)n * stride);
	return t;
}

//}}}
static void taps_free(struct taps* t) //{{{
{
	if (t == NULL) return;
	free(t->start);
	free(t->count);
	free(t->w);
	free(t);
}

//}}}
static double lanczos_h(int wt, double fc, double x) //{{{
{
	// The kernel of lanczos.c's h(): sinc low pass at fc (cycles per input
	// pixel), times a sinc window reaching wt input pixels each side
	if (x < -wt || x > wt) return 0.0;
	const double	lp = x == 0.0 ? 2.0*fc : sin(2.0*PI_D*fc*x) / (PI_D*x);
	const double	X = PI_D * (x/wt);
	const double	win = x == 0.0 ? 1.0 : sin(X) / X;
	return lp * win;
}

//}}}
static struct taps* taps_lanczos(int orig, int new, int wt) //{{{
{
	const double	r = (double)orig / new;
	const double	fc = fmin(0.5, new / (2.0*orig));	// Cut off at the output's Nyquist frequency (and no higher than the input's)
	struct taps*	t = taps_new(new, 2*wt+1);

	for (int o=0; o<new; o++) {
		const double	v = (o + 0.5) * r - 0.5;		// Centre of output o, as an input index
		const int		vi = (int)floor(v);
		int				lo = vi - wt, hi = vi + wt;
		double			acc = 0.0;
		float*			w = t->w + (size_t)o*t->stride;

		if (lo < 0) lo = 0;
		if (hi > orig-1) hi = orig-1;

		for (int j=lo; j<=hi; j++)
			acc += (w[j-lo] = (float)lanczos_h(wt, fc, j - v));

		for (int j=lo; j<=hi; j++)
			w[j-lo] = (float)(w[j-lo] / acc);	// Normalize: no bias, at the edges too

		t->start[o] = lo;
		t->count[o] = hi - lo + 1;
	}

	return t;
}

//}}}
#ifndef SCALE_V_STRIP
#	define SCALE_V_STRIP	64		// Pixels: a strip of all the rows a few outputs' taps reach stays in L1
#endif
#ifndef SCALE_V_AHEAD
#	define SCALE_V_AHEAD	2		// Outputs: prefetch the strip of the rows this far ahead first needs
#endif

// Vertical pass: out row o = sum over its taps k of w[k] * in row (start+k).
// Rows are width pixels long, in_stride and out_stride pixels apart.  Done in
// strips of SCALE_V_STRIP columns, so the source rows the taps of successive
// outputs share are read from memory once, not once per output row (which
// needs in_stride not to alias them all to the same cache sets: see
// scratch_stride).  In a strip, eight pixels at a time are accumulated in
// registers while stepping down the taps, so each output pixel is written once.
static void apply_v(const pelf* restrict in, int in_stride, int width, pelf* restrict out, int out_stride, const struct taps* t) //{{{
{
	for (int x0=0; x0<width; x0+=SCALE_V_STRIP)
	for (int o=0; o<t->n; o++) {
		const float* restrict	wk = t->w + (size_t)o*t->stride;
		const int				count = t->count[o];
		const pelf* restrict	s0 = in + (size_t)t->start[o]*in_stride;
		pelf* restrict			d = out + (size_t)o*out_stride;
		const int				x1 = x0+SCALE_V_STRIP < width ? x0+SCALE_V_STRIP : width;
		int						x = x0;

		// The rows output o+SCALE_V_AHEAD reaches beyond output o+SCALE_V_AHEAD-1's
		// are new to the cache: fetch their part of this strip now, so the loads
		// don't wait on memory when that output gets to them
		if (o + SCALE_V_AHEAD < t->n) {
			const int	oa = o + SCALE_V_AHEAD;
			const int	from = t->start[oa-1] + t->count[oa-1];
			const int	to = t->start[oa] + t->count[oa];
			for (int r=from; r<to; r++) {
				const char*	line = (const char*)(in + (size_t)r*in_stride + x0);
				const char*	end = (const char*)(in + (size_t)r*in_stride + x1);
				for (; line < end; line += 64)
					__builtin_prefetch(line, 0, 3);
			}
		}

#if SCALE_NEON
		for (; x+8 <= x1; x+=8) {
			float32x4_t	a0 = vdupq_n_f32(0), a1 = a0, a2 = a0, a3 = a0, a4 = a0, a5 = a0, a6 = a0, a7 = a0;
			const float* restrict	s = (const float*)(s0 + x);
			for (int k=0; k<count; k++, s += (size_t)in_stride*4) {
				const float	wv = wk[k];
				a0 = vfmaq_n_f32(a0, vld1q_f32(s +  0), wv);
				a1 = vfmaq_n_f32(a1, vld1q_f32(s +  4), wv);
				a2 = vfmaq_n_f32(a2, vld1q_f32(s +  8), wv);
				a3 = vfmaq_n_f32(a3, vld1q_f32(s + 12), wv);
				a4 = vfmaq_n_f32(a4, vld1q_f32(s + 16), wv);
				a5 = vfmaq_n_f32(a5, vld1q_f32(s + 20), wv);
				a6 = vfmaq_n_f32(a6, vld1q_f32(s + 24), wv);
				a7 = vfmaq_n_f32(a7, vld1q_f32(s + 28), wv);
			}
			float* restrict	dp = (float*)(d + x);
			vst1q_f32(dp +  0, a0); vst1q_f32(dp +  4, a1); vst1q_f32(dp +  8, a2); vst1q_f32(dp + 12, a3);
			vst1q_f32(dp + 16, a4); vst1q_f32(dp + 20, a5); vst1q_f32(dp + 24, a6); vst1q_f32(dp + 28, a7);
		}
		for (; x < x1; x++) {
			float32x4_t	a = vdupq_n_f32(0);
			const float* restrict	s = (const float*)(s0 + x);
			for (int k=0; k<count; k++, s += (size_t)in_stride*4)
				a = vfmaq_n_f32(a, vld1q_f32(s), wk[k]);
			vst1q_f32((float*)(d + x), a);
		}
#else
		for (; x < x1; x++) {
			float	a[4] = {0, 0, 0, 0};
			const pelf* restrict	s = s0 + x;
			for (int k=0; k<count; k++, s += in_stride)
				for (int c=0; c<4; c++) a[c] += wk[k] * s->chan[c];
			for (int c=0; c<4; c++) d[x].chan[c] = a[c];
		}
#endif
	}
}

//}}}
// One output of the horizontal pass: the sum of count taps from s.
static inline void h_one(const float* restrict s, const float* restrict wk, int count, float* restrict d) //{{{
{
	int		k = 0;
#if SCALE_NEON
	// Two accumulators, so consecutive FMAs don't wait on each other
	float32x4_t	a0 = vdupq_n_f32(0), a1 = a0;
	for (; k+2 <= count; k+=2) {
		a0 = vfmaq_n_f32(a0, vld1q_f32(s + 4*k),     wk[k]);
		a1 = vfmaq_n_f32(a1, vld1q_f32(s + 4*k + 4), wk[k+1]);
	}
	if (k < count) a0 = vfmaq_n_f32(a0, vld1q_f32(s + 4*k), wk[k]);
	vst1q_f32(d, vaddq_f32(a0, a1));
#else
	float	a[4] = {0, 0, 0, 0};
	for (; k<count; k++)
		for (int c=0; c<4; c++) a[c] += wk[k] * s[4*k + c];
	for (int c=0; c<4; c++) d[c] = a[c];
#endif
}

//}}}
#if SCALE_NEON
// Four outputs with the same number of taps, interleaved so the FMAs of eight
// accumulators overlap, instead of each output's chain of dependent FMAs
// running alone.  Weights four at a time (vector loads, multiplied by lane),
// not a scalar load per tap.
static inline void h_four(const float* restrict s0, const float* restrict s1, const float* restrict s2, const float* restrict s3, const float* restrict w0, int stride, int count, float* restrict d) //{{{
{
	const float* restrict	w1 = w0 + stride;
	const float* restrict	w2 = w1 + stride;
	const float* restrict	w3 = w2 + stride;
	float32x4_t	a0 = vdupq_n_f32(0), b0 = a0, a1 = a0, b1 = a0, a2 = a0, b2 = a0, a3 = a0, b3 = a0;
	int			k = 0;
#define TAP2(L0, L1) \
	a0 = vfmaq_laneq_f32(a0, vld1q_f32(s0 + 4*(k+L0)), q0, L0);	b0 = vfmaq_laneq_f32(b0, vld1q_f32(s0 + 4*(k+L1)), q0, L1); \
	a1 = vfmaq_laneq_f32(a1, vld1q_f32(s1 + 4*(k+L0)), q1, L0);	b1 = vfmaq_laneq_f32(b1, vld1q_f32(s1 + 4*(k+L1)), q1, L1); \
	a2 = vfmaq_laneq_f32(a2, vld1q_f32(s2 + 4*(k+L0)), q2, L0);	b2 = vfmaq_laneq_f32(b2, vld1q_f32(s2 + 4*(k+L1)), q2, L1); \
	a3 = vfmaq_laneq_f32(a3, vld1q_f32(s3 + 4*(k+L0)), q3, L0);	b3 = vfmaq_laneq_f32(b3, vld1q_f32(s3 + 4*(k+L1)), q3, L1);
	for (; k+4 <= count; k+=4) {
		const float32x4_t	q0 = vld1q_f32(w0 + k), q1 = vld1q_f32(w1 + k), q2 = vld1q_f32(w2 + k), q3 = vld1q_f32(w3 + k);
		TAP2(0, 1)
		TAP2(2, 3)
	}
#undef TAP2
	for (; k < count; k++) {
		a0 = vfmaq_n_f32(a0, vld1q_f32(s0 + 4*k), w0[k]);
		a1 = vfmaq_n_f32(a1, vld1q_f32(s1 + 4*k), w1[k]);
		a2 = vfmaq_n_f32(a2, vld1q_f32(s2 + 4*k), w2[k]);
		a3 = vfmaq_n_f32(a3, vld1q_f32(s3 + 4*k), w3[k]);
	}
	vst1q_f32(d,      vaddq_f32(a0, b0));
	vst1q_f32(d + 4,  vaddq_f32(a1, b1));
	vst1q_f32(d + 8,  vaddq_f32(a2, b2));
	vst1q_f32(d + 12, vaddq_f32(a3, b3));
}

//}}}
#endif
// Horizontal pass: in each of the h rows, out pixel o = sum over its taps of
// w[k] * in pixel (start+k).  A row at a time, so the row being read stays in
// cache.  Rows are in_stride and out_stride pixels apart.  Groups of four
// outputs with the same number of taps (all but the few at the edges) go
// through h_four.
static void apply_h(const pelf* restrict in, int in_stride, int h, pelf* restrict out, int out_stride, const struct taps* t) //{{{
{
	for (int y=0; y<h; y++) {
		const float* restrict	row = (const float*)(in + (size_t)y*in_stride);
		float* restrict			d = (float*)(out + (size_t)y*out_stride);
		int						o = 0;

		while (o < t->n) {
			const int	count = t->count[o];
#if SCALE_NEON
			if (o+4 <= t->n && t->count[o+1] == count && t->count[o+2] == count && t->count[o+3] == count) {
				h_four(row + 4*t->start[o], row + 4*t->start[o+1], row + 4*t->start[o+2], row + 4*t->start[o+3],
						t->w + (size_t)o*t->stride, t->stride, count, d + 4*o);
				o += 4;
				continue;
			}
#endif
			h_one(row + 4*t->start[o], t->w + (size_t)o*t->stride, count, d + 4*o);
			o++;
		}
	}
}

//}}}
static int default_wt(int orig, int new) //{{{
{
	// lanczos.c's choice of kernel width for the ratio
	if (new > orig)			return 4;
	if (new * 8 < orig)		return 15;
	if (new * 5 < orig)		return 13;
	if (new * 3 < orig)		return 10;
	return 9;
}

//}}}
#ifndef SCALE_SCRATCH_KEEP
#	define SCALE_SCRATCH_KEEP	(256u<<20)	// Bytes: a thread keeps its scratch buffer between calls up to this size
#endif

// The intermediate image between the two passes lives in a per-thread scratch
// buffer, reused from call to call: a fresh allocation of that size costs more
// in page faults (the kernel zeroing new pages) than a pass over it does.
typedef struct {
	void*	buf;
	size_t	bytes;
	int		exit_handler;
} scale_tsd;
static Tcl_ThreadDataKey	scratch_key;

static void scratch_free(void* cdata) //{{{
{
	scale_tsd*	t = cdata;
	free(t->buf);
	t->buf = NULL;
	t->bytes = 0;
}

//}}}
static pelf* scratch_get(size_t bytes, int* owned) //{{{
{
	scale_tsd*	t = Tcl_GetThreadData(&scratch_key, sizeof(scale_tsd));
	void*		p;

	if (bytes > SCALE_SCRATCH_KEEP) {
		// Too big to keep: the caller frees it
		if (posix_memalign(&p, 64, bytes)) Tcl_Panic("Could not allocate %zu bytes for scaling", bytes);
		*owned = 1;
		return p;
	}
	if (t->bytes < bytes) {
		free(t->buf);
		t->buf = NULL;
		t->bytes = 0;
		if (posix_memalign(&p, 64, bytes)) Tcl_Panic("Could not allocate %zu bytes for scaling", bytes);
		t->buf = p;
		t->bytes = bytes;
		if (!t->exit_handler) {
			Tcl_CreateThreadExitHandler(scratch_free, t);
			t->exit_handler = 1;
		}
	}
	*owned = 0;
	return t->buf;
}

//}}}
static int scratch_stride(int width) //{{{
{
	// Pixels per row of the intermediate: a whole number of 64 byte cache lines
	// (4 pelfs), and an odd number of them, so the rows the vertical pass reads
	// down a column spread over the cache sets.  At an even stride (2048 pixels
	// = 32 KB) they all land in the same few sets of the 4-way L1 and evict each
	// other, and the vertical pass waits on memory for most of its loads.
	int	s = (width + 3) & ~3;
	if (((s/4) & 1) == 0) s += 4;
	return s;
}

//}}}

// Scale src to new_w x new_h.  wt: the lanczos kernel width (0: chosen for each
// dimension's ratio).  Returns a new pmapf; src is unchanged.
//
// The horizontal pass runs first (reading src a row at a time, so src's own
// row stride doesn't matter), into the padded intermediate, then the vertical
// pass writes the result.
struct pmapf* pmapf_scale(const struct pmapf* src, int new_w, int new_h, int wt) //{{{
{
	const int		sw = src->width, sh = src->height;
	const int		wt_v = wt > 0 ? wt : default_wt(sh, new_h);
	const int		wt_h = wt > 0 ? wt : default_wt(sw, new_w);
	struct pmapf*	out = pmapf_new(new_w, new_h);

	if (sw == new_w && sh == new_h) {
		memcpy(out->pixel_data, src->pixel_data, sizeof(pelf) * (size_t)sw * sh);
	} else if (sh == new_h) {
		struct taps*	th = taps_lanczos(sw, new_w, wt_h);
		apply_h(src->pixel_data, sw, sh, out->pixel_data, new_w, th);
		taps_free(th);
	} else if (sw == new_w) {
		struct taps*	tv = taps_lanczos(sh, new_h, wt_v);
		apply_v(src->pixel_data, sw, sw, out->pixel_data, new_w, tv);
		taps_free(tv);
	} else {
		struct taps*	th = taps_lanczos(sw, new_w, wt_h);
		struct taps*	tv = taps_lanczos(sh, new_h, wt_v);
		const int		ms = scratch_stride(new_w);
		int				owned;
		pelf*			mid = scratch_get(sizeof(pelf) * (size_t)ms * sh, &owned);

		apply_h(src->pixel_data, sw, sh, mid, ms, th);
		apply_v(mid, ms, new_w, out->pixel_data, new_w, tv);
		if (owned) free(mid);
		taps_free(th);
		taps_free(tv);
	}

	return out;
}

//}}}

// vim: foldmethod=marker foldmarker={{{,}}}
