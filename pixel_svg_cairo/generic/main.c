#include <tclstuff.h>
#include <librsvg/rsvg.h>
//#include <librsvg/rsvg-cairo.h>
#include <cairo.h>

#include <stdlib.h>
#include <pixel.h>
//#include "2d.h"
//#include "primitives.h"
//#include "tcl_pmap.h"


static int glue_load_svg(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	_pel					init;
	gimp_image_t*			new;
	int						w, h;
	cairo_surface_t*		surface;
	cairo_t*				cr;
	const unsigned char*	data;
	int						tmpint;
	gsize					datalen;
	GError*					gerror = NULL;
	RsvgHandle*				rsvg_handle = NULL;
	gdouble					intrinsic_w = 1.0, intrinsic_h = 1.0;
	int						stride;

	if (objc != 2 && objc != 4)
		CHECK_ARGS(1, "svgdata ?w h?");

	if (objc == 4) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &w));
		TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &h));
	}

	data = (const unsigned char*)Tcl_GetStringFromObj(objv[1], &tmpint);
	datalen = tmpint;
	rsvg_handle = rsvg_handle_new_from_data(data, datalen, &gerror);
	if (rsvg_handle == NULL) {
		// Some error
		THROW_ERROR("Error loading svg data: ", gerror->message);
	}

	if (!rsvg_handle_get_intrinsic_size_in_pixels(rsvg_handle, &intrinsic_w, &intrinsic_h)) {
		// Intrinsic size was not defined, what to do?
		//fprintf(stderr, "No intrinsic size in SVG\n");
		intrinsic_w = 1.0;
		intrinsic_h = 1.0;
	}

	if (objc == 2) {
		w = (int)(round(intrinsic_w));
		h = (int)(round(intrinsic_h));
	}
	//fprintf(stderr, "got w: %d, h: %d, sw: %d, sh: %d\n", w, h, sw, sh);

	//init.c = 0xff0000ff;
	init.c = 0x00000000;
	new = pmap_new(w, h, init);
	box(new, 0, 0, w, h, init, 0);

	stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, w);
	/*
	fprintf(stderr, "computed stride: %d, width: %d, width*4: %d\n",
			stride, w, w*4);
	*/
	surface = cairo_image_surface_create_for_data((unsigned char*)new->pixel_data,
			CAIRO_FORMAT_ARGB32,
			w, h,
			stride);
	cr = cairo_create(surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		fprintf(stderr, "cairo error1: %s\n", cairo_status_to_string(cairo_status(cr)));
		cairo_destroy(cr);
		THROW_ERROR("Some cairo error");
	}
	cairo_surface_destroy(surface);

	if (objc == 4) {
		cairo_scale(cr,
				w/intrinsic_w,
				h/intrinsic_h);
	}

	rsvg_handle_render_cairo(rsvg_handle, cr);

	//cairo_surface_finish(surface);
	cairo_surface_flush(surface);
	//cairo_surface_destroy(surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		fprintf(stderr, "cairo error: %s\n", cairo_status_to_string(cairo_status(cr)));
		cairo_destroy(cr);
		THROW_ERROR("Some cairo error");
	}
	cairo_destroy(cr);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	g_object_unref(rsvg_handle);

	return TCL_OK;
}


int Pixel_svg_cairo_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.6", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.5", 0) == NULL) return TCL_ERROR;

	NEW_CMD("pixel::svg_cairo::load_svg", glue_load_svg);

	return TCL_OK;
}


