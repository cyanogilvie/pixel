#include <tclstuff.h>
#include "2d.h"
#include "tcl_pmap.h"

#include <cairo.h>
#include <svg-cairo.h>


static int glue_load_svg(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	_pel				init;
	gimp_image_t		*new;
	int					w, h;
	unsigned int		sw, sh;
	cairo_surface_t		*surface;
	cairo_t				*cr;
	const char			*msg;
	svg_cairo_t			*svg_cairo;

	if (objc != 2 && objc != 4)
		CHECK_ARGS(1, "filename ?w h?");

	if (objc == 4) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &w));
		TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &h));
	}

	svg_cairo_create(&svg_cairo);
	svg_cairo_parse(svg_cairo, Tcl_GetString(objv[1]));

	svg_cairo_get_size(svg_cairo, &sw, &sh);
	if (objc == 2) {
		w = sw;
		h = sh;
	}
	//fprintf(stderr, "got w: %d, h: %d, sw: %d, sh: %d\n", w, h, sw, sh);

	init.c = 0xff0000ff;
	new = pmap_new(w, h, init);
	box(new, 0, 0, w, h, init, 0);

	surface = cairo_image_surface_create_for_data(new->pixel_data,
			CAIRO_FORMAT_ARGB32,
			w*4, h,
			w*4);
	cr = cairo_create(surface);
	cairo_surface_destroy(surface);

	if (objc == 4) {
		cairo_scale(cr,
				(double)w/sw,
				(double)h/sh);
	}

	svg_cairo_render(svg_cairo, cr);

	cairo_surface_flush(surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		cairo_destroy(cr);
		THROW_ERROR("Some cairo error");
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	svg_cairo_destroy(svg_cairo);
	cairo_destroy(cr);

	return TCL_OK;
}


int Pixel_svg_cairo_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
		return TCL_ERROR;
	}

	NEW_CMD("pixel::svg_cairo::load_svg", glue_load_svg);

	return TCL_OK;
}


