// vim: ts=4 shiftwidth=4 tags=../tags

#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include <X11/Xlib.h>
#include <tcl.h>
#include <tclstuff.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <magick/api.h>
#include <magick/error.h>

#include "2d.h"
#include "tcl_pmap.h"
#include "main.h"

static Tcl_Interp *g_interp;
static Tcl_Obj *g_progress_cb = NULL;
static ExceptionInfo	g_exception;
static ImageInfo		*g_imageinfo;

/*
static int progress_func(Imlib_Image image, char percent, //{{{1
		int update_x, int update_y, int update_w, int update_h)
{
	Tcl_Obj *o[2];
	
	if (g_progress_cb != NULL) {
		o[0] = g_progress_cb;
		o[1] = Tcl_NewIntObj(percent);
		Tcl_IncrRefCount(o[0]);
		Tcl_IncrRefCount(o[1]);
		
		if (Tcl_EvalObjv(g_interp, 2, o, TCL_EVAL_GLOBAL) == TCL_ERROR)
			Tcl_BackgroundError(g_interp);
		
		Tcl_DecrRefCount(o[0]);
		Tcl_DecrRefCount(o[1]);
	}
	return 1;
}
*/


static int glue_load_image(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	Image				*image;
	int					w, h;
	_pel				init;
	gimp_image_t		*new;
	
	if (objc < 2 || objc > 3)
		CHECK_ARGS(1, "filename ?progress_callback?");

	strncpy(g_imageinfo->filename, Tcl_GetString(objv[1]), MaxTextExtent-1);
	g_imageinfo->filename[MaxTextExtent-1] = 0;
	image = ReadImage(g_imageinfo, &g_exception);
	//if (g_exception.severity != UndefinedException)
		//CatchException(&g_exception);
	if (image == (Image *)NULL)
		THROW_ERROR("Cannot load image ", Tcl_GetString(objv[1]), ": ", g_exception.reason, g_exception.reason);

	if (objc == 3) {
		if (g_progress_cb != NULL) {
			Tcl_DecrRefCount(g_progress_cb);
			g_progress_cb = NULL;
		}
		g_interp = interp;
		g_progress_cb = objv[2];
		Tcl_IncrRefCount(g_progress_cb);
		// TODO: Register progress callback
	}

	w = image->columns;
	h = image->rows;
	DBG("Read image %p, w: %d, h: %d\n", image, w, h);
	init.c = 0x00000000;

	new = pmap_new(w, h, init);

	DispatchImage(image, 0, 0, w, h, "BGRA", 0,
			new->pixel_data, &g_exception);
	if (g_exception.severity != UndefinedException) {
		//CatchException(&g_exception);
		THROW_ERROR("Cannot load image ", Tcl_GetString(objv[1]), ": ", g_exception.reason, g_exception.reason);
	}
	DestroyImage(image);

	if (g_progress_cb != NULL) {
		Tcl_DecrRefCount(g_progress_cb);
		g_interp = NULL;
		g_progress_cb = NULL;
		// TODO: Deregister process callback
	}
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	
	return TCL_OK;
}


/*
static int glue_save_image(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	Image			image;
	gimp_image_t	*pmap;
	char			*filename;
	char			*type;
	
	if (objc < 3 || objc > 5)
		CHECK_ARGS(2, "pmap filename ?type? ?progress_callback?");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	
//	fprintf(stderr, "pmap->width: %ld, pmap->height: %ld\n", pmap->width, pmap->height);
	image = imlib_create_image_using_data(pmap->width, pmap->height,
			(DATA32 *)pmap->pixel_data);
	if (image == NULL)
		THROW_ERROR("Failed to wrap pmap in an imlib2 image");

	imlib_context_set_image(image);
	
	filename = Tcl_GetString(objv[2]);
	type = NULL;
	if (objc >= 4) {
		type = Tcl_GetString(objv[3]);
		imlib_image_set_format(type);
	} else {
		type = strrchr(filename, '.');
		if (type)
			imlib_image_set_format(type + 1);
	}
	
	if (objc == 5) {
		if (g_progress_cb != NULL) {
			Tcl_DecrRefCount(g_progress_cb);
			g_progress_cb = NULL;
		}
		g_interp = interp;
		g_progress_cb = objv[4];
		Tcl_IncrRefCount(g_progress_cb);
		imlib_context_set_progress_function(progress_func);
		imlib_context_set_progress_granularity(10);
	}
	
	imlib_save_image_with_error_return(filename, &error);
	imlib_free_image();
	
	if (g_progress_cb != NULL) {
		Tcl_DecrRefCount(g_progress_cb);
		g_interp = NULL;
		g_progress_cb = NULL;
		imlib_context_set_progress_function(NULL);
	}

	if (error != IMLIB_LOAD_ERROR_NONE)
		THROW_ERROR("Error saving image to file ", filename, ": ", lookup_load_error(error));

	return TCL_OK;
}

*/
static int glue_scale_pmap(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*src_pmap;
	gimp_image_t		*dest_pmap;
	Image				*src;
	Image				*dest;
	_pel				init;
	int					w, h, filter, index;
	double				blur;

	static CONST char	*filters[] = {
		"bessel", "blackman", "box", "catrom", "cubic", "gaussian", 
		"hanning", "hermite", "lanczos", "mitchell", "point", "quadratic",
		"sinc", "triangle", (char *)NULL
	};
	enum {
		F_BESSEL, F_BLACKMAN, F_BOX, F_CATROM, F_CUBIC, F_GAUSSIAN,
		F_HANNING, F_HERMITE, F_LANCZOS, F_MITCHELL, F_POINT, F_QUADRATIC,
		F_SINC, F_TRIANGLE
	};
	
	if (objc < 4 || objc > 6)
		CHECK_ARGS(3, "src_pmap w h ?filter? ?blur?");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src_pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &h));
	if (objc >= 5) {
		TEST_OK(Tcl_GetIndexFromObj(interp, objv[4], (char **)filters, "filter",
					0, &index));
		switch (index) { //{{{
			case F_BESSEL:
				filter = BesselFilter;
				break;
			case F_BLACKMAN:
				filter = BlackmanFilter;
				break;
			case F_BOX:
				filter = BoxFilter;
				break;
			case F_CATROM:
				filter = CatromFilter;
				break;
			case F_CUBIC:
				filter = CubicFilter;
				break;
			case F_GAUSSIAN:
				filter = GaussianFilter;
				break;
			case F_HANNING:
				filter = HanningFilter;
				break;
			case F_HERMITE:
				filter = HermiteFilter;
				break;
			case F_LANCZOS:
				filter = LanczosFilter;
				break;
			case F_MITCHELL:
				filter = MitchellFilter;
				break;
			case F_POINT:
				filter = PointFilter;
				break;
			case F_QUADRATIC:
				filter = QuadraticFilter;
				break;
			case F_SINC:
				filter = SincFilter;
				break;
			case F_TRIANGLE:
				filter = TriangleFilter;
				break;
			default:
				THROW_ERROR("Error fetching filter lookup, unrecognised filter");
				break;
		} //}}}
	} else {
		filter = LanczosFilter;
	}
	if (objc >= 6) {
		TEST_OK(Tcl_GetDoubleFromObj(interp, objv[5], &blur));
	} else {
		blur = 1.0;
	}

	src = ConstituteImage(src_pmap->width, src_pmap->height,
			"BGRA", CharPixel, src_pmap->pixel_data, &g_exception);
	if (src == NULL)
		THROW_ERROR("Failed to wrap src_pmap in an imagemagick image",
				g_exception.description);

	dest = ResizeImage(src, w, h, filter, blur, &g_exception);
	DestroyImage(src);

	if (dest == NULL)
		THROW_ERROR("Failed to scale image");

	init.c = 0x00000000;
	dest_pmap = pmap_new(w, h, init);
	DispatchImage(dest, 0, 0, w, h, "BGRA", 0,
			dest_pmap->pixel_data, &g_exception);
	if (g_exception.severity != UndefinedException) {
		//CatchException(&g_exception);
		THROW_ERROR("Cannot export imagemagic image to pmap: ", g_exception.description, g_exception.reason);
	}
	DestroyImage(dest);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dest_pmap));

	return TCL_OK;
}
	

int Pixel_magick_Init(Tcl_Interp *interp) //{{{1
{
	g_imageinfo = CloneImageInfo((ImageInfo *)NULL);
	GetExceptionInfo(&g_exception);
	
	NEW_CMD("pixel::magick::load_image", glue_load_image);
//	NEW_CMD("pixel::magick::save_image", glue_save_image);
	NEW_CMD("pixel::magick::scale_pmap", glue_scale_pmap);
	
	return TCL_OK;
}


