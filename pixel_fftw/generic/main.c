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
	plan = fftwf_plan_dft_r2c_2d(inp->width, inp->height, in, out, FFTW_ESTIMATE);

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
static int transform_pmapf(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	inp=NULL;
	struct pmapf*	outp=NULL;
	float*			in=NULL;
	fftwf_complex*	out=NULL;
	fftwf_plan		plan;
	int				x, y, c;
	pelf*			srow;
	pelf*			drow;
	float*			p;
	float			min=1e6, max=-1e6;

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &inp));

	outp = pmapf_new(inp->width, inp->height);
	//pmap_clr(outp, (_pel)(uint32_t)0);

	in = fftwf_alloc_real(inp->width * inp->height);
	out = fftwf_alloc_complex(inp->height * inp->width);
	plan = fftwf_plan_dft_r2c_2d(inp->width, inp->height, in, out, FFTW_ESTIMATE);

	for (c=0; c<3; c++) {
		for (y=0; y<inp->height; y++) {
			srow = inp->pixel_data + y*inp->width;
			for (x=0; x<inp->width; x++)
				in[y*inp->width+x] = srow[x].chan[c];
		}

		fftwf_execute(plan);

		p = out;

		for (y=0; y<inp->height; y++) {
			drow = outp->pixel_data + y*outp->width;
			srow = inp->pixel_data + y*inp->width;
			for (x=0; x<inp->width; x++, p++) {
				//float v = logf(.9+(( out[(int)((y/2.0)+.5)*(inp->width+2) + (int)((x)+.5)]   )/(7*13412.525391)))+.5;
				int		mx = x > inp->width/2 ? x : inp->width/2 - x;
				//float v = out[(int)(((y+inp->height/2)%inp->height)/2.0+.5)*(inp->width+2) + (int)((mx)+.5)]*300 / (float)(inp->width * inp->height) + .5;
				float v = out[(int)(((y+inp->height/2)%inp->height)/2.0+.5)*(inp->width+2) + (int)(mx+.5)] / (float)(inp->width) + .5;
				//drow[x].chan[c] = .5;
				//drow[x].chan[c] = logf(.5+( out[(int)(logf(y*2)+.5)*inp->width + (int)(logf(x)+.5)]   )/(2*13412.525391));		// TODO: figure out scaling factor
				drow[x].chan[c] = v;
				if (v < min) min = v;
				if (v > max) max = v;
			}
			p+=2;
		}
	}

	fprintf(stderr, "min: %f, max: %f\n", min, max);

	for (y=0; y<inp->height; y++) {
		drow = outp->pixel_data + y*outp->width;
		for (x=0; x<inp->width; x++) {
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
int Pixel_fftw_Init(Tcl_Interp* interp) //{{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	NEW_CMD("::pixel::fftw::transform_pmap", transform_pmap);
	NEW_CMD("::pixel::fftw::transform_pmapf", transform_pmapf);
	NEW_CMD("::pixel::fftw::noise", noise);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
