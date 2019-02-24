#include "tclstuff.h"
#include <pixel.h>
#include <complex.h>
#include <fftw3.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

typedef union {
	uint32_t	c;
	uint8_t		chan[4];
	struct {
		uint8_t	b,g,r,a;
	} ch;
} pel;


static int noise(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	int				width, height, x, y, c;
	gimp_image_t*	out = NULL;
	pel*			p = NULL;

	CHECK_ARGS(2, "width height");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &height));

	out = pmap_new(width, height, (_pel)(uint32_t)0);

	p = (pel*)out->pixel_data;

	for (y=0; y<height; y++)
		for (x=0; x<width; x++, p++) {
			for (c=0; c<3; c++)
				p->chan[c] = random() % 0xff;
			p->chan[CHAN_A] = 255;
		}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(out));

	return TCL_OK;
}

//}}}
/*
static int transform_pmap(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	inp=NULL;
	gimp_image_t*	outp=NULL;
	float*			in=NULL;
	fftwf_complex*	out=NULL;
	fftwf_plan		plan;
	struct pmapf*	inpf=NULL;
	int				x, y, c;
	pelf*			srow;
	pel*			drow;
	float*			p;
	float			min=1e6, max=-1e6;

	CHECK_ARGS(1, "pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &inp));
	inpf = pmap_to_pmapf(inp);

	outp = pmap_new(inp->width, inp->height, (_pel)(uint32_t)0);
	//pmap_clr(outp, (_pel)(uint32_t)0);

	in = fftwf_alloc_real(inp->width * inp->height);
	out = (fftwf_complex*)fftwf_alloc_complex(inp->height * inp->width);
	plan = fftwf_plan_dft_r2c_2d(inp->height, inp->width, in, out, FFTW_ESTIMATE);

	for (c=0; c<3; c++) {
		for (y=0; y<inp->height; y++) {
			srow = inpf->pixel_data + y*inpf->width;
			for (x=0; x<inp->width; x++)
				in[y*inp->width+x] = srow[x].chan[c];
		}

		fftwf_execute(plan);

		p = out;

		for (y=0; y<inp->height; y++) {
			drow = (pel*)(outp->pixel_data + y*inpf->width);
			srow = inpf->pixel_data + y*inpf->width;
			for (x=0; x<inp->width; x++, p++) {
				//float v = logf(.9+(( out[(int)((y/2.0)+.5)*(inp->width+2) + (int)((x)+.5)]   )/(7*13412.525391)))+.5;
				int		mx = x > inp->width/2 ? x : inp->width/2 - x;
				//float v = out[(int)(((y+inp->height/2)%inp->height)/2.0+.5)*(inp->width+2) + (int)((mx)+.5)]*300 / (float)(inp->width * inp->height) + .5;
				float v = out[(int)(((y+inp->height/2)%inp->height)/2.0+.5)*(inp->width+2) + (int)((mx)+.5)] / (float)(inp->width) + .5;
				if (v<0) v=0;
				if (v>1) v=1;
				//drow[x].chan[c] = out[y*(inp->width/2)+x/2] * .1;		// TODO: figure out scaling factor
				//drow[x].chan[c] = 255*(logf(.5+*p/(2*13412.525391)));		// TODO: figure out scaling factor
				//drow[x].chan[c] = srow[x].chan[c]*0xff;
				//drow[x].chan[c] = 255*(logf(.5+( out[(int)(logf(y*2)+.5)*inp->width + (int)(logf(x)+.5)]   )/(2*13412.525391)));		// TODO: figure out scaling factor
				drow[x].chan[c] = 255*v;		// TODO: figure out scaling factor
				if (v < min) min = v;
				if (v > max) max = v;
			}
			p+=2;
		}
	}

	fprintf(stderr, "min: %f, max: %f\n", min, max);

	for (y=0; y<inp->height; y++) {
		drow = (pel*)(outp->pixel_data + y*inpf->width);
		for (x=0; x<inp->width; x++) {
			drow[x].chan[CHAN_A] = 255;
		}
	}

	fftwf_destroy_plan(plan);
	fftwf_free(in); in=NULL;
	fftwf_free(out); out=NULL;

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(outp));

	pmapf_free(&inpf);

	return TCL_OK;
}

//}}}
*/
static int transform_pmapf(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	inp=NULL;
	struct pmapf*	outp=NULL;
	float*			in=NULL;
	fftwf_complex*	out=NULL;
	fftwf_plan		plan;
	int				x, y, c, out_width;
	pelf*			srow;
	pelf*			drow;
	float			min=1e6, max=-1e6, threshold, raw_max=-1e6, raw_max_less_1=-1e6;
	float			range;

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &inp));

	out_width = (inp->width/2 + 1);
	outp = pmapf_new(out_width, inp->height);
	//pmap_clr(outp, (_pel)(uint32_t)0);

	in = fftwf_alloc_real(inp->width * inp->height);
	out = fftwf_alloc_complex(inp->height * out_width*2);
	plan = fftwf_plan_dft_r2c_2d(inp->height, inp->width, in, out, FFTW_ESTIMATE);

	for (c=0; c<3; c++) {
		for (y=0; y<inp->height; y++) {
			srow = inp->pixel_data + y*inp->width;
			for (x=0; x<inp->width; x++)
				in[y*inp->width+x] = srow[x].chan[c];
		}

		fftwf_execute(plan);

		for (y=0; y<inp->height; y++) {
			drow = outp->pixel_data + y*outp->width;
			for (x=0; x<out_width; x++) {
				//int		mx = x > inp->width/2 ? x : inp->width/2 - x;
				//int		mx = x > inp->width/2   ?  inp->width - x   :    x;
				//int		my = y > inp->height/2  ?  y - inp->height/2  :  inp->height/2 - y;
				//float v = out[my*out_width + mx];
				float v = out[y*out_width + x];
				if (abs(v) > raw_max) {
					raw_max_less_1 = raw_max;
					raw_max = abs(v);
				}
				v = logf(1+abs(v));
				drow[x].chan[c] = v;
				if (v < min) min = v;
				if (v > max) max = v;
			}
		}
	}
	range = max-min;
	if (range == 0)
		range = 1;

	threshold = logf(1 + raw_max_less_1*0.00003);	// Ignore the DC sample
	//threshold = 0;

	//fprintf(stderr, "min: %f, max: %f, range: %f\n", min, max, range);

	for (y=0; y<inp->height; y++) {
		drow = outp->pixel_data + y*outp->width;
		for (x=0; x<outp->width; x++) {
			for (c=0; c<3; c++) {
				float v = drow[x].chan[c];
				drow[x].chan[c] = v > threshold ? (v - min)/range : 0;
			}
			drow[x].chan[CHAN_A] = 1.0;
		}
	}

	fftwf_destroy_plan(plan);
	fftwf_free(in); in=NULL;
	fftwf_free(out); out=NULL;

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(outp));

	return TCL_OK;
}

//}}}
static int histogram(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	inp=NULL;
	struct pmapf*	outp=NULL;
	int				x, y, c, i, trig;
	pelf*			srow;
	pelf*			drow;
	pelf			bg, red, blue, green;
	float			row_max = 0.0;
	float			col_max = 0.0;
	float*			row_avg = NULL;
	float*			col_avg = NULL;
	float*			row_avg_smoothed = NULL;
	float*			col_avg_smoothed = NULL;

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &inp));

	memset(&bg, 0, sizeof(bg));
	memset(&red, 0, sizeof(red));
	memset(&blue, 0, sizeof(blue));
	memset(&green, 0, sizeof(green));
	//bg.chan[CHAN_A] = 1.0;
	red.chan[CHAN_A] = 1.0;
	blue.chan[CHAN_A] = 1.0;
	green.chan[CHAN_A] = 0.5;
	red.chan[CHAN_R] = 1.0;
	blue.chan[CHAN_B] = 1.0;
	green.chan[CHAN_G] = 1.0;

	outp = pmapf_new(inp->width, inp->height);
	pmapf_clr(outp, bg);

	row_avg = ckalloc(inp->height * sizeof(float));
	col_avg = ckalloc(inp->width * sizeof(float));
	memset(row_avg, 0, inp->height * sizeof(float));
	memset(col_avg, 0, inp->width * sizeof(float));
	row_avg_smoothed = ckalloc(inp->height * sizeof(float));
	col_avg_smoothed = ckalloc(inp->width * sizeof(float));
	memset(row_avg_smoothed, 0, inp->height * sizeof(float));
	memset(col_avg_smoothed, 0, inp->width * sizeof(float));

	for (y=0; y<inp->height; y++) {
		srow = inp->pixel_data + y*inp->width;
		for (x=0; x<inp->width; x++) {
			float cmax = 0;
			srow = inp->pixel_data + y*inp->width;

			for (c=0; c<3; c++)
				if (srow[x].chan[c] > cmax)
					cmax = srow[x].chan[c];

			row_avg[y] += cmax;
			col_avg[x] += cmax;
		}
	}

	for (y=0; y<inp->height; y++) {
		row_avg[y] /= inp->height;
		if (row_avg[y] > row_max)
			row_max = row_avg[y];
	}

	for (x=0; x<inp->width; x++) {
		col_avg[x] /= inp->width;
		if (col_avg[x] > col_max)
			col_max = col_avg[x];
	}

#define MAX(a,b) \
	({ __typeof__ (a) _a = (a); \
			__typeof__ (b) _b = (b); \
			_a > _b ? _a : _b; })
#define MIN(a,b) \
	({ __typeof__ (a) _a = (a); \
			__typeof__ (b) _b = (b); \
			_a < _b ? _a : _b; })

#define T	7
	for (y=0; y<inp->height; y++) {
		int	from = MAX(1, y-T), to = MIN(inp->height-2, y+T);
		row_avg_smoothed[y] = 0.0;
		for (i=from; i<=to; i++) 
			row_avg_smoothed[y] += row_avg[i];
		row_avg_smoothed[y] /= (to-from) + 1;
	}

	for (x=0; x<inp->width; x++) {
		int	from = MAX(1, x-T), to = MIN(inp->width-1, x+T);
		col_avg_smoothed[x] = 0.0;
		for (i=from; i<=to; i++) 
			col_avg_smoothed[x] += col_avg[i];
		col_avg_smoothed[x] /= (to-from) + 1;
	}

	fprintf(stderr, "row_max: %.2f, col_max: %.2f\n, outp->width: %d, outp->height: %d\n", row_max, col_max, outp->width, outp->height);

	trig = 0;
	for (y=0; y<outp->height; y++) {
		float f = row_avg_smoothed[y]/row_max;
		drow = outp->pixel_data + y*outp->width;
		//fprintf(stderr, "row %d/%d: %.4f, %d\n", y, outp->height, f, (int)((outp->width * row_avg[y]/row_max)));
		if (f <= 0.018) {
			if (!trig) {
				trig = 1;
				fprintf(stderr, "yt: %.2f\n", (float)(y)/(outp->height/2-1));
			}
			for (x=0; x<outp->width; x++) drow[x] = green;
		}
		drow[(int)((outp->width * f))] = blue;
	}

	trig = 0;
	for (y=outp->height-1; y>=0; y--) {
		float f = row_avg_smoothed[y]/row_max;
		if (f <= 0.018) {
			if (!trig) {
				trig = 1;
				fprintf(stderr, "yb: %.2f\n", (float)(outp->height-1-y)/(outp->height/2-1));
				break;
			}
		}
	}

	trig = 0;
	for (x=0; x<outp->width; x++) {
		float f = col_avg_smoothed[x]/col_max;
		//fprintf(stderr, "col %d/%d: %.4f, %d\n", x, outp->width, f, (int)(outp->height - (col_avg[x]/col_max)*outp->height));
		if (f <= 0.016) {
			if (!trig) {
				trig = 1;
				fprintf(stderr, "x: %.2f\n", (float)(x)/(outp->width-1));
			}
			for (y=0; y<inp->height; y++) {
				drow = outp->pixel_data + y*outp->width;
				drow[x] = green;
			}
		}
		drow = outp->pixel_data + (int)(outp->height - f*outp->height)*outp->width;
		drow[x] = red;
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(outp));

	ckfree(row_avg); row_avg = NULL;
	ckfree(col_avg); col_avg = NULL;
	ckfree(row_avg_smoothed); row_avg_smoothed = NULL;
	ckfree(col_avg_smoothed); col_avg_smoothed = NULL;

	return TCL_OK;
}

//}}}
static int overscale(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	inp=NULL;
	int				x, y, c, i;
	pelf*			srow;
	float			row_max = 0.0;
	float			col_max = 0.0;
	float*			row_avg = NULL;
	float*			col_avg = NULL;
	float*			row_avg_smoothed = NULL;
	float*			col_avg_smoothed = NULL;
	Tcl_Obj*		res = Tcl_NewObj();

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &inp));

	row_avg = ckalloc(inp->height * sizeof(float));
	col_avg = ckalloc(inp->width * sizeof(float));
	memset(row_avg, 0, inp->height * sizeof(float));
	memset(col_avg, 0, inp->width * sizeof(float));
	row_avg_smoothed = ckalloc(inp->height * sizeof(float));
	col_avg_smoothed = ckalloc(inp->width * sizeof(float));
	memset(row_avg_smoothed, 0, inp->height * sizeof(float));
	memset(col_avg_smoothed, 0, inp->width * sizeof(float));

	for (y=0; y<inp->height; y++) {
		srow = inp->pixel_data + y*inp->width;
		for (x=0; x<inp->width; x++) {
			float cmax = 0;
			srow = inp->pixel_data + y*inp->width;

			for (c=0; c<3; c++)
				if (srow[x].chan[c] > cmax)
					cmax = srow[x].chan[c];

			row_avg[y] += cmax;
			col_avg[x] += cmax;
		}
	}

	for (y=0; y<inp->height; y++) {
		row_avg[y] /= inp->height;
		if (row_avg[y] > row_max)
			row_max = row_avg[y];
	}

	for (x=0; x<inp->width; x++) {
		col_avg[x] /= inp->width;
		if (col_avg[x] > col_max)
			col_max = col_avg[x];
	}

#define MAX(a,b) \
	({ __typeof__ (a) _a = (a); \
			__typeof__ (b) _b = (b); \
			_a > _b ? _a : _b; })
#define MIN(a,b) \
	({ __typeof__ (a) _a = (a); \
			__typeof__ (b) _b = (b); \
			_a < _b ? _a : _b; })

#define T	7
	for (y=0; y<inp->height; y++) {
		int	from = MAX(1, y-T), to = MIN(inp->height-2, y+T);
		row_avg_smoothed[y] = 0.0;
		for (i=from; i<=to; i++) 
			row_avg_smoothed[y] += row_avg[i];
		row_avg_smoothed[y] /= (to-from) + 1;
	}

	for (x=0; x<inp->width; x++) {
		int	from = MAX(1, x-T), to = MIN(inp->width-1, x+T);
		col_avg_smoothed[x] = 0.0;
		for (i=from; i<=to; i++) 
			col_avg_smoothed[x] += col_avg[i];
		col_avg_smoothed[x] /= (to-from) + 1;
	}

	for (y=0; y<inp->height; y++) {
		float f = row_avg_smoothed[y]/row_max;
		if (f <= 0.018) {
			TEST_OK(Tcl_DictObjPut(interp, res, Tcl_NewStringObj("yt", 2), Tcl_NewDoubleObj((float)(y)/(inp->height/2-1))));
			break;
		}
	}

	for (y=inp->height-1; y>=0; y--) {
		float f = row_avg_smoothed[y]/row_max;
		if (f <= 0.018) {
			TEST_OK(Tcl_DictObjPut(interp, res, Tcl_NewStringObj("yb", 2), Tcl_NewDoubleObj((float)(inp->height-1-y)/(inp->height/2-1))));
			break;
		}
	}

	for (x=0; x<inp->width; x++) {
		float f = col_avg_smoothed[x]/col_max;
		if (f <= 0.016) {
			TEST_OK(Tcl_DictObjPut(interp, res, Tcl_NewStringObj("x", 1), Tcl_NewDoubleObj((float)(x)/(inp->width-1))));
			break;
		}
	}

	Tcl_SetObjResult(interp, res);

	ckfree(row_avg); row_avg = NULL;
	ckfree(col_avg); col_avg = NULL;
	ckfree(row_avg_smoothed); row_avg_smoothed = NULL;
	ckfree(col_avg_smoothed); col_avg_smoothed = NULL;

	return TCL_OK;
}

//}}}
int Pixel_fftw_Init(Tcl_Interp* interp) //{{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	//NEW_CMD("::pixel::fftw::transform_pmap", transform_pmap);
	NEW_CMD("::pixel::fftw::transform_pmapf", transform_pmapf);
	NEW_CMD("::pixel::fftw::noise", noise);
	NEW_CMD("::pixel::fftw::histogram", histogram);
	NEW_CMD("::pixel::fftw::overscale", overscale);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
