// vim: ts=4 shiftwidth=4 tags=../tags
//   Heavily influenced by tkImgPhoto.c in the tk source

#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include <tcl.h>
#include <tclstuff.h>
#include <Hermes.h>

#include "2d.h"
#include "tcl_pmap.h"

static HermesHandle	g_hermes_handle;
static HermesHandle	g_hermes_clearer_handle;
static HermesFormat *g_hermes_pmap_format;


static int glue_scale_pmap(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*src_pmap;
	gimp_image_t		*dest_pmap;
	_pel				init;
	int					w, h;
	
	CHECK_ARGS(3, "src_pmap w h");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src_pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &h));

	init.c = 0x00000000;
	dest_pmap = pmap_new(w, h, init);

	Hermes_ConverterRequest(g_hermes_handle,
			g_hermes_pmap_format,
			g_hermes_pmap_format);
	Hermes_ConverterCopy(g_hermes_handle,
			src_pmap->pixel_data,
			0, 0,
			src_pmap->width,
			src_pmap->height,
			src_pmap->width * 4,
			dest_pmap->pixel_data,
			0, 0,
			dest_pmap->width,
			dest_pmap->height,
			dest_pmap->width * 4);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dest_pmap));

	return TCL_OK;
}


static int glue_clear_pmap(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*pmap;
	int					r, g, b, a, x, y, w, h, res;
	
	fprintf(stderr, "objc: %d\n", objc);
	if (objc < 2 || objc > 10)
		CHECK_ARGS(1, "pmap ?r? ?g? ?b? ?a? ?x? ?y? ?w? ?h?");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	
	r = 0;
	g = 0;
	b = 0;
	a = 0;
	x = 0;
	y = 0;
	w = pmap->width;
	h = pmap->height;
	
	if (objc >= 3) TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &r));
	if (objc >= 4) TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &g));
	if (objc >= 5) TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &b));
	if (objc >= 6) TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &a));
	if (objc >= 7) TEST_OK(Tcl_GetIntFromObj(interp, objv[6], &x));
	if (objc >= 8) TEST_OK(Tcl_GetIntFromObj(interp, objv[7], &y));
	if (objc >= 9) TEST_OK(Tcl_GetIntFromObj(interp, objv[8], &w));
	if (objc >= 10) TEST_OK(Tcl_GetIntFromObj(interp, objv[9], &h));

	// sanity check
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (w > pmap->width) w = pmap->width;
	if (h > pmap->height) h = pmap->height;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	if (a > 255) a = 255;
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (a < 0) a = 0;

	res = Hermes_ClearerClear(g_hermes_clearer_handle,
			pmap->pixel_data,
			x, y,
			w, h,
			pmap->width * 4,
			r, g, b, (char8)a);

	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(res != 0));

	return TCL_OK;
}


// Init {{{1
int Pixel_hermes_Init(Tcl_Interp *interp)
{
	if (Hermes_Init() == 0)
		THROW_ERROR("Failed to initialize Hermes");
	g_hermes_handle = Hermes_ConverterInstance(HERMES_CONVERT_DITHER);
	g_hermes_pmap_format = Hermes_FormatNew(32,
			MD_MASK_RED, MD_MASK_GREEN, MD_MASK_BLUE, MD_MASK_ALPHA, 0);
	if ((g_hermes_clearer_handle = Hermes_ClearerInstance()) == 0)
		THROW_ERROR("Failed to get Hermes_ClearerInstance()");
	if (Hermes_ClearerRequest(g_hermes_clearer_handle, g_hermes_pmap_format) == 0)
		THROW_ERROR("Failed to get a clearer for pmap format");

	NEW_CMD("pixel::hermes::scale_pmap", glue_scale_pmap);
	NEW_CMD("pixel::hermes::clear_pmap", glue_clear_pmap);

	return TCL_OK;
}


