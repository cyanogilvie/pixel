#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include "all.h"

// loadpng filename {{{1
static int glue_loadpng(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	gimp_image_t *	pmap;
	CHECK_ARGS(1, "filename");

	pmap = read_png(Tcl_GetString(objv[1]));

	if (pmap == NULL)
		THROW_ERROR("Error loading png file: ", Tcl_GetString(objv[1]));
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));

	return TCL_OK;
}


/*// savepng filename pmap {{{1
static int glue_savepng(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int				err;
	gimp_image_t	*pmap;
	int				quality;
	
	CHECK_ARGS(2, "filename pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &quality));
	

	if (err != 0)
		THROW_ERROR("Error saving png file: ", Tcl_GetString(objv[1]));
	
	return TCL_OK;
}

*/
// is_png filename {{{1
static int glue_is_png(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	FILE		*fp;
	char		buf[8];

	CHECK_ARGS(1, "filename");

	fp = fopen(Tcl_GetString(objv[1]), "rb");
	if (fp == NULL)
		THROW_ERROR("Unable to open file: (", Tcl_GetString(objv[1]), ")");

	if (fread(buf, 1, 8, fp) != 8) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(0));
		return TCL_OK;
	}
	
	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(!png_sig_cmp(buf, (png_size_t)0,
					8)));

	return TCL_OK;
}


// png_dimensions filename {{{1
static int glue_png_dimensions(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	FILE		*fp;
	char		buf[8];
	int			x, y;
	Tcl_Obj		*res;

	CHECK_ARGS(1, "filename");

	if (get_png_dimensions(Tcl_GetString(objv[1]), &x, &y) != 0)
		THROW_ERROR("Error reading PNG dimensions");

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(x)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(y)));
	
	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


// Init {{{1
int Pixel_png_Init(Tcl_Interp *interp)
{
	NEW_CMD("pixel::png::loadpng", glue_loadpng);
	//NEW_CMD("pixel::png::savepng", glue_savepng);
	NEW_CMD("pixel::png::is_png", glue_is_png);
	NEW_CMD("pixel::png::png_dimensions", glue_png_dimensions);
	
	return TCL_OK;
}


