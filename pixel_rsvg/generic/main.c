#include <tclstuff.h>
#include "2d.h"
#include "tcl_pmap.h"

#include <rsvg.h>
#include <cairo.h>
#include <rsvg-cairo.h>
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
	RsvgHandle			*handle;
	GError				*error;
	RsvgDimensionData	dims;
	cairo_surface_t		*surface;
	cairo_t				*cr;
	const char			*msg;
	
	if (objc != 2 && objc != 4)
		CHECK_ARGS(1, "filename ?w h?");

	if (objc == 4) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &w));
		TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &h));
	}

	handle = rsvg_handle_new_from_file(Tcl_GetString(objv[1]), &error);
	//msg = rsvg_handle_get_title(handle);
	//fprintf(stderr, "Title: %s\n", msg == NULL ? "NULL" : msg);
	//msg = rsvg_handle_get_desc(handle);
	//fprintf(stderr, "Desc: %s\n", msg == NULL ? "NULL" : msg);
	if (handle == NULL)
		THROW_ERROR("Failed to load SVG");

	rsvg_handle_get_dimensions(handle, &dims);
	if (objc == 2) {
		w = dims.width;
		h = dims.height;
		//fprintf(stderr, "got w: %d, h: %d\n", w, h);
	}
	init.c = 0;
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
				(double)w/dims.width,
				(double)h/dims.height);
	}

	rsvg_handle_render_cairo(handle, cr);

	cairo_surface_flush(surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		cairo_destroy(cr);
		THROW_ERROR("Some cairo error");
	}
	//fprintf(stderr, "cairo happy\n");

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	cairo_destroy(cr);

	return TCL_OK;
}


static int glue_load_svg2(cdata, interp, objc, objv)
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
		//fprintf(stderr, "got w: %d, h: %d\n", sw, sh);
	}
	init.c = 0;
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


int Pixel_rsvg_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
		return TCL_ERROR;
	}

	rsvg_init();

	NEW_CMD("pixel::rsvg::load_svg", glue_load_svg);
	NEW_CMD("pixel::rsvg::load_svg2", glue_load_svg2);

	return TCL_OK;
}


