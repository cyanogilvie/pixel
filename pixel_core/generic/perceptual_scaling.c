#include "pixel.h"

static void convValid(struct pmapf* out, struct pmapf* in, const int kern_size) //{{{
{
	int		x, y, kx, ky, c;
	pelf*	i;
	pelf*	o;

	for (y=0; y<in->height-kern_size+1; y++) {
		o = out->pixel_data + y*out->width;
		for (x=0; x<in->width-kern_size+1; x++, o++) {
			for (c=0; c<4; c++)
				o->chan[c] = 0.0f;

			for (ky=0; ky<kern_size; ky++) {
				i = in->pixel_data + (y+ky)*in->width + x;
				for (kx=0; kx<kern_size; kx++, i++) {
					for (c=0; c<4; c++)
						o->chan[c] += i->chan[c];
				}
			}

			for (c=0; c<4; c++)
				o->chan[c] /= kern_size*kern_size;
		}
	}
}

//}}}
static void convFull(struct pmapf* out, struct pmapf* in, const int kern_size) //{{{
{
	int		x, y, kx, ky, c;
	pelf*	i;
	pelf*	o;

	for (y=0; y<in->height; y++) {
		o = out->pixel_data + y*out->width;
		for (x=0; x<in->width; x++, o++) {
			int		end_ky = y+kern_size <= in->height ? kern_size : in->height - y;
			int		end_kx = x+kern_size <= in->width  ? kern_size : in->width  - x;

			for (c=0; c<4; c++)
				o->chan[c] = 0.0f;

			for (ky=0; ky<end_ky; ky++) {
				i = in->pixel_data + (y+ky)*in->width + x;
				for (kx=0; kx<end_kx; kx++, i++) {
					for (c=0; c<4; c++)
						o->chan[c] += i->chan[c];
				}
			}

			for (c=0; c<4; c++)
				o->chan[c] /= kern_size*kern_size;
		}
	}
}

//}}}
static void subSample(struct pmapf* out, struct pmapf* in, int interval) //{{{
{
	int		x, y;
	pelf*	o = out->pixel_data;
	pelf*	i = NULL;

	for (y=0; y<in->height; y+=interval) {
		i = in->pixel_data + y*in->width;
		for (x=0; x<in->width; x+=interval, o++, i+=interval)
			*o = *i;
	}
}

//}}}
static void pmapf_add(struct pmapf* pmapf, struct pmapf* operand) //{{{
{
	int		c, remain;
	pelf*	p = pmapf->pixel_data;
	pelf*	op = operand->pixel_data;

	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++, op++)
		for (c=0; c<4; c++)
			p->chan[c] += op->chan[c];
}

//}}}
static void pmapf_sub(struct pmapf* pmapf, struct pmapf* operand) //{{{
{
	int		c, remain;
	pelf*	p = pmapf->pixel_data;
	pelf*	op = operand->pixel_data;

	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++, op++)
		for (c=0; c<4; c++)
			p->chan[c] -= op->chan[c];
}

//}}}
static void pmapf_div(struct pmapf* pmapf, struct pmapf* operand) //{{{
{
	int		c, remain;
	pelf*	p = pmapf->pixel_data;
	pelf*	op = operand->pixel_data;

	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++, op++)
		for (c=0; c<4; c++)
			p->chan[c] /= op->chan[c];
}

//}}}
static void pmapf_sqrt(struct pmapf* pmapf) //{{{
{
	int		c, remain;
	pelf*	p = pmapf->pixel_data;

	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++)
		for (c=0; c<4; c++)
			p->chan[c] = sqrtf(p->chan[c]);
}

//}}}
static void pmapf_div_into(struct pmapf* out, struct pmapf* pmapf_a, struct pmapf* pmapf_b) //{{{
{
	int		c, remain;
	pelf*	o = out->pixel_data;
	pelf*	a = pmapf_a->pixel_data;
	pelf*	b = pmapf_b->pixel_data;

	for (remain=out->width*out->height; remain>0; remain--, a++, b++, o++)
		for (c=0; c<4; c++)
			o->chan[c] = a->chan[c] / b->chan[c];
}

//}}}
static void pmapf_mul_into(struct pmapf* out, struct pmapf* pmapf_a, struct pmapf* pmapf_b) //{{{
{
	int		c, remain;
	pelf*	o = out->pixel_data;
	pelf*	a = pmapf_a->pixel_data;
	pelf*	b = pmapf_b->pixel_data;

	for (remain=out->width*out->height; remain>0; remain--, a++, b++, o++)
		for (c=0; c<4; c++)
			o->chan[c] = a->chan[c] * b->chan[c];
}

//}}}
static void pmapf_square_into(struct pmapf* out, struct pmapf* in) //{{{
{
	int		c, remain;
	pelf*	o = out->pixel_data;
	pelf*	i = in->pixel_data;

	for (remain=out->width*out->height; remain>0; remain--, i++, o++)
		for (c=0; c<4; c++)
			o->chan[c] = i->chan[c] * i->chan[c];
}

//}}}
static void pmapf_clamp_below_epsilon(struct pmapf* pmapf, struct pmapf* test) //{{{
{
	int		c, remain;
	pelf*	p = pmapf->pixel_data;
	pelf*	t = test->pixel_data;

	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++, t++)
		for (c=0; c<4; c++)
			if (t->chan[c] < 1e-6)
				p->chan[c] = 0;
}

//}}}
struct pmapf* pmapf_copy(struct pmapf* src) //{{{
{
	struct pmapf*	out = pmapf_new(src->width, src->height);

	memcpy(out->pixel_data, src->pixel_data, out->width*out->height*out->bytes_per_pixel);

	return out;
}

//}}}
void dump_pmapf(struct pmapf* pmapf) //{{{
{
	int	x, y, c;

	for (y=0; y<pmapf->height; y++) {
		for (x=0; x<pmapf->width; x++) {
			fprintf(stderr, "(%d, %d):", x, y);
			for (c=0; c<4; c++)
				fprintf(stderr, " %.3f", pmapf->pixel_data[y*pmapf->width+x].chan[c]);
			fprintf(stderr, "\n");
		}
	}
}

//}}}
static void squish(struct pmapf* pmapf) //{{{
{
	float		max = -1e6f, min = 1e6f, range, bump, f=1.0f;
	int			c, remain;
	pelf*		p = pmapf->pixel_data;

	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++)
		for (c=0; c<4; c++) {
			if (p->chan[c] > max) max = p->chan[c];
			if (p->chan[c] < min) min = p->chan[c];
		}

	bump = -min;
	range = max-min;

	if (range > 1.0)
		f = 1.0/range;

	p = pmapf->pixel_data;
	fprintf(stderr, "min: %.4f, max: %.4f, range: %.4f, bump: %.4f, f: %.4f\n", min, max, range, bump, f);
	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++)
		for (c=0; c<4; c++)
			p->chan[c] = (p->chan[c]+bump) * f;
}

//}}}
static void clamp(struct pmapf* pmapf) //{{{
{
	int			c, remain;
	pelf*		p = pmapf->pixel_data;

	for (remain=pmapf->width*pmapf->height; remain>0; remain--, p++)
		for (c=0; c<4; c++) {
			if (p->chan[c] > 1.0) p->chan[c] = 1.0f;
			if (p->chan[c] < 0.0) p->chan[c] = 0.0f;
		}
}

//}}}
struct pmapf* scale_perceptual(struct pmapf* H, int s, int np) //{{{
{
	// Parameter and local variable names chosen to correspond with https://graphics.ethz.ch/~cengizo/Files/Sig15PerceptualDownscaling.pdf
	/*
	int				width = .5 + H->width / s;
	int				height = .5 + H->height / s;
	*/
	int				width = H->width / s;
	int				height = H->height / s;
	int				sqrt_np = sqrtf(np);
	int				c;
	pelf			white, black;
	struct pmapf*	tb = NULL;
	struct pmapf*	tb2 = NULL;
	struct pmapf*	t = NULL;
	struct pmapf*	t2 = NULL;
	struct pmapf*	tc = NULL;
	struct pmapf*	L = NULL;
	struct pmapf*	L2 = NULL;
	struct pmapf*	M = NULL;
	struct pmapf*	Sl = NULL;
	struct pmapf*	Sh = NULL;
	struct pmapf*	R = NULL;
	struct pmapf*	Im = NULL;
	struct pmapf*	N = NULL;
	struct pmapf*	T = NULL;
	struct pmapf*	D = NULL;

	for (c=0; c<4; c++) {
		white.chan[c] = 1.0f;
		black.chan[c] = 0.0f;
	}

	tb = pmapf_new(H->width, H->height);
	tb2 = pmapf_new(H->width, H->height);
	t = pmapf_new(width, height);
	t2 = pmapf_new(width, height);
	L = pmapf_new(width, height);
	L2 = pmapf_new(width, height);
	M = pmapf_new(width, height);
	Sl = pmapf_new(width, height);
	Sh = pmapf_new(width, height);
	R = pmapf_new(width, height);
	Im = pmapf_new(width, height);
	N = pmapf_new(width, height);
	T = pmapf_new(width, height);
	M = pmapf_new(width, height);
	R = pmapf_new(width, height);
	D = pmapf_new(width, height);

	pmapf_clr(tb, black);
	pmapf_clr(tb2, black);
	pmapf_clr(t, black);
	pmapf_clr(t2, black);
	pmapf_clr(L, black);
	pmapf_clr(L2, black);
	pmapf_clr(M, black);
	pmapf_clr(Sl, black);
	pmapf_clr(Sh, black);
	pmapf_clr(R, black);
	//pmapf_clr(Im, black);
	pmapf_clr(N, black);
	pmapf_clr(T, black);
	pmapf_clr(M, black);
	pmapf_clr(R, black);
	pmapf_clr(D, black);

	clamp(H);

	//  2: L ←  subSample(convValid(H, P(s)), s)
	convValid(tb, H, s);
	subSample(L, tb, s);
	fprintf(stderr, "2: L\n"); dump_pmapf(L);

	//  3: L₂ ←  subSample(convValid(H², P(s)), s)
	pmapf_square_into(tb2, H);
	convValid(tb, tb2, s);
	subSample(L2, tb, s);
	fprintf(stderr, "3: L₂\n"); dump_pmapf(L2);

	//  4: M ←  convValid(L, P(sqrt(np)))
	convValid(M, L, sqrt_np);
	fprintf(stderr, "4: M\n"); dump_pmapf(M);

	pmapf_square_into(t2, M);
	//  5: Sl ←  convValid(L², P(sqrt(np))) − M²
	pmapf_square_into(t, L);
	convValid(Sl, t, sqrt_np);
	pmapf_sub(Sl, t2);
	fprintf(stderr, "5: Sl\n"); dump_pmapf(Sl);

	//  6: Sh ←  convValid(L₂, P(sqrt(np))) − M²
	convValid(Sh, L2, sqrt_np);
	pmapf_sub(Sh, t2);
	fprintf(stderr, "6: Sh\n"); dump_pmapf(Sh);

	//  7: R ←  sqrt(Sh / Sl)
	pmapf_div_into(R, Sh, Sl);
	pmapf_sqrt(R);
	fprintf(stderr, "7: R\n"); dump_pmapf(R);

	//  8: R(Sl < 1e-6) ←  0
	pmapf_clamp_below_epsilon(R, Sl);
	fprintf(stderr, "8: R\n"); dump_pmapf(R);

	//  9: N ←  convFull(Im, P(sqrt(np)))
	pmapf_clr(Im, white);
	convFull(N, Im, sqrt_np);
	fprintf(stderr, "9: N\n"); dump_pmapf(N);

	// 10: T ←  convFull(R × M, P(sqrt(np)))
	//pmapf_mul_into(T, R, M);
	pmapf_mul_into(t, R, M);
	convFull(T, t, sqrt_np);
	fprintf(stderr, "10: T\n"); dump_pmapf(T);

	// 11: M ←  convFull(M, P(sqrt(np)))
	tc = pmapf_copy(M);
	convFull(M, tc, sqrt_np);
	pmapf_free(&tc);
	fprintf(stderr, "11: M\n"); dump_pmapf(M);

	// 12: R ←  convFull(R, P(sqrt(np)))
	tc = pmapf_copy(R);
	convFull(R, tc, sqrt_np);
	pmapf_free(&tc);
	fprintf(stderr, "12: R\n"); dump_pmapf(R);

	// 13: D ←  (M + R × L − T)/N
	/*
	tc = pmapf_copy(M);
	pmapf_add(tc, R);
	pmapf_mul_into(D, tc, L);
	pmapf_free(&tc);
	pmapf_sub(D, T);
	pmapf_div(D, N);
	*/
	pmapf_mul_into(D, R, L);
	pmapf_add(D, M);
	pmapf_sub(D, T);
	fprintf(stderr, "13: D\n"); dump_pmapf(D);
	pmapf_div(D, N);
	fprintf(stderr, "13: D/N\n"); dump_pmapf(D);
	//squish(D);

	pmapf_free(&tb);
	pmapf_free(&tb2);
	pmapf_free(&t);
	pmapf_free(&t2);
	pmapf_free(&L);
	pmapf_free(&L2);
	pmapf_free(&M);
	pmapf_free(&Sl);
	pmapf_free(&Sh);
	pmapf_free(&R);
	pmapf_free(&Im);
	pmapf_free(&N);
	pmapf_free(&T);

	return D;
}

//}}}

static int glue_scale_perceptual(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	int				factor, patch_size=4;
	struct pmapf*	in = NULL;
	struct pmapf*	out = NULL;

	if (objc<3 || objc>4)
		CHECK_ARGS(3, "pmapf factor ?patch_size?");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &in));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &factor));

	if (objc >= 4) {
		float	sqrt_np;
		TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &patch_size));
		sqrt_np = sqrtf(patch_size);
		if ((int)sqrt_np * (int)sqrt_np != patch_size)
			THROW_ERROR("patch_size must be a square number");
	}

	if (factor < 1)
		THROW_ERROR("Invalid factor, must be an integer >= 1");

	if (factor == 1) {
		// Do nothing gracefully
		Tcl_SetObjResult(interp, objv[1]);
		return TCL_OK;
	}

	out = scale_perceptual(in, factor, patch_size);

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}

int perceptual_scaling_init(Tcl_Interp* interp) //{{{
{
	NEW_CMD("::pixel::scale_perceptual", glue_scale_perceptual);

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
