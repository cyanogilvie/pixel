#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

//#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <tcl.h>
#include <tclstuff.h>
#include <sys/time.h>
#include <unistd.h>
#include "2d.h"
#include "ttf.h"
#include "tcl_pmap.h"
#include "primitives.h"

#define SQUARE		0
#define	SINE		1
#define LINEAR		2

// pmap_new x y colour {{{1
static int glue_pmap_new(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	int				x, y;
	_pel			colour;
	gimp_image_t	*new;
	
	CHECK_ARGS(3, "x y colour");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], (int *)&colour.c));

	new = pmap_new(x, y, colour);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


// pmap_clr dest colour {{{1
static int glue_pmap_clr(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	_pel			colour;
	gimp_image_t	*dest;
	
	CHECK_ARGS(2, "dest colour");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &dest));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], (int *)&colour.c));
	
	pmap_clr(dest, colour);

	return TCL_OK;
}


// pmap_cut src x1 y1 x2 y2 {{{1
static int glue_pmap_cut(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*src;
	gimp_image_t	*new;
	int				x1, y1, x2, y2;
	
	CHECK_ARGS(5, "src x1 y1 x2 y2");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &x1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &y1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &x2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &y2));

	new = pmap_cut(src, x1, y1, x2, y2);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


// pmap_paste dest src x y flags {{{1
static int glue_pmap_paste(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	int					x, y, flags;
	
	CHECK_ARGS(5, "dest src x y flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &flags));

	pmap_paste(dest, src, x, y, flags);

	return TCL_OK;
}


// pmap_patch dest src sx sy sw sh dx dy flags {{{1
static int glue_pmap_patch(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	int					sx, sy, sw, sh, dx, dy, flags;
	
	CHECK_ARGS(9, "dest src sx sy sw sh dx dy flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &sx));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &sy));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &sw));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[6], &sh));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[7], &dx));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[8], &dy));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[9], &flags));

	pmap_patch(dest, src, sx, sy, sw, sh, dx, dy, flags);

	return TCL_OK;
}


// pmap_compose dest src x y flags {{{1
static int glue_pmap_compose(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	gimp_image_t		*new;
	int					x, y, flags;
	
	CHECK_ARGS(5, "dest src x y flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &flags));

	new = pmap_compose(dest, src, x, y, flags);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


// pmap_filter dest flags factor {{{1
static int glue_pmap_filter(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *	dest;
	int				flags;
	double			factor;
	
	CHECK_ARGS(3, "dest flags factor");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &dest));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &flags));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &factor));

	pmap_filter(dest, flags, factor);

	return TCL_OK;
}


// pmap_dropshadow src {{{1
static int glue_pmap_dropshadow(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*src;
	gimp_image_t	*new;
	int				blur;
	
	CHECK_ARGS(2, "src blur");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &blur));

	new = pmap_dropshadow(src, blur);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	
	return TCL_OK;
}


// pmap_rotate src quads {{{1
static int glue_pmap_rotate(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*src;
	gimp_image_t	*new;
	int				quads;
	
	CHECK_ARGS(2, "src quads");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &quads));

	new = pmap_rotate(src, quads);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	
	return TCL_OK;
}


// pmap_info pmap {{{1
static int glue_pmap_info(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj			*res;
	gimp_image_t	*pmap;
	
	CHECK_ARGS(1, "pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmap->width)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmap->height)));

	Tcl_SetObjResult(interp, res);
	
	return TCL_OK;
}


// dup obj {{{1
static int glue_dup(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	CHECK_ARGS(1, "obj");

	Tcl_SetObjResult(interp, Tcl_DuplicateObj(objv[1]));

	return TCL_OK;
}

// render_ttf colour fft_face px_size text {{{1
static int glue_render_ttf(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				px_size;
	gimp_image_t *	pmap;
	_pel			base_col;
	FT_Face			face;
	
	
	CHECK_ARGS(4, "colour fft_face px_size text");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int *)&base_col));
	TEST_OK(Tcl_GetTTFFaceFromObj(interp, objv[2], &face));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &px_size));
	
	pmap = render_ttf(base_col, face, px_size, Tcl_GetString(objv[4]));
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));
	
	return TCL_OK;
}


// box dest x y w h colour flags {{{1
static int glue_box(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *	pmap;
	int				x, y, w, h, flags;
	_pel			colour;
	
	CHECK_ARGS(7, "dest x y w h colour flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &h));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[6], (int *)&colour.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[7], &flags));
	
	box(pmap, x, y, w, h, colour, flags);

	return TCL_OK;
}


// line x1 y1 x2 y2 colour pmap {{{1
static int glue_line(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				x1, y1, x2, y2;
	_pel			col;
	gimp_image_t	*dest;
	
	CHECK_ARGS(6, "x1 y1 x2 y2 colour pmap");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &x1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &y1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &x2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], (int *)&col.c));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[6], &dest));
	
	line(x1, y1, x2, y2, col, dest);
	
	return TCL_OK;
}

// line_aa x1 y1 x2 y2 colour pmap {{{1
static int glue_line_aa(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				x1, y1, x2, y2;
	_pel			col;
	gimp_image_t	*dest;
	
	CHECK_ARGS(6, "x1 y1 x2 y2 colour pmap");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &x1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &y1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &x2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], (int *)&col.c));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[6], &dest));
	
	line_aa(x1, y1, x2, y2, col, dest);
	
	return TCL_OK;
}


// line_aa_osa x1 y1 x2 y2 colour osa pmap {{{1
static int glue_line_aa_osa(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	double			x1, y1, x2, y2;
	int				osa;
	_pel			col;
	gimp_image_t	*dest;
	
	CHECK_ARGS(7, "x1 y1 x2 y2 colour osa pmap");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &x1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &y1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &x2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], (int *)&col.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[6], &osa));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[7], &dest));
	
	line_aa_osa(x1, y1, x2, y2, col, osa, dest);
	
	return TCL_OK;
}


// bezier x1 y1 cpx1 cpy1 cpx2 cpy2 x2 y2 colour osa pmap {{{1
static int glue_bezier(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	double			x1, y1, cpx1, cpy1, cpx2, cpy2, x2, y2;
	int				osa;
	_pel			col;
	gimp_image_t	*dest;
	
	CHECK_ARGS(11, "x1 y1 cpx1 cpy1 cpx2 cpy2 x2 y2 colour osa pmap");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &x1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &y1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &cpx1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &cpy1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[5], &cpx2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[6], &cpy2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[7], &x2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[8], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[9], (int *)&col.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[10], &osa));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[11], &dest));
	
	bezier(x1, y1, cpx1, cpy1, cpx2, cpy2, x2, y2, col, osa, dest);
	
	return TCL_OK;
}


// Init {{{1
int Pixel_Init(Tcl_Interp *interp)
{
	Tcl_RegisterObjType(&tcl_pmap);
	Tcl_RegisterObjType(&tcl_ttf_face);

	init_2d();
	init_ttf();
	
	NEW_CMD("pixel::pmap_new", glue_pmap_new);
	NEW_CMD("pixel::pmap_clr", glue_pmap_clr);
	NEW_CMD("pixel::pmap_cut", glue_pmap_cut);
	NEW_CMD("pixel::pmap_paste", glue_pmap_paste);
	NEW_CMD("pixel::pmap_patch", glue_pmap_patch);
	NEW_CMD("pixel::pmap_compose", glue_pmap_compose);
	NEW_CMD("pixel::pmap_filter", glue_pmap_filter);
	NEW_CMD("pixel::pmap_dropshadow", glue_pmap_dropshadow);
	NEW_CMD("pixel::pmap_rotate", glue_pmap_rotate);
	NEW_CMD("pixel::pmap_info", glue_pmap_info);
	NEW_CMD("pixel::dup", glue_dup);

	// Primitives
	NEW_CMD("pixel::box", glue_box);
	NEW_CMD("pixel::line", glue_line);
	NEW_CMD("pixel::line_aa", glue_line_aa);
	NEW_CMD("pixel::line_aa_osa", glue_line_aa_osa);
	NEW_CMD("pixel::bezier", glue_bezier);

	// TrueType font rendering
	NEW_CMD("pixel::render_ttf", glue_render_ttf);

	return TCL_OK;
}


