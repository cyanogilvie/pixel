#include <pixelInt.h>
#include "lanczos.h"

#define PI			3.141592653589793

#define SQUARE		0
#define	SINE		1
#define LINEAR		2

#ifdef __builtin_expect
#	define likely(exp)   __builtin_expect(!!(exp), 1)
#	define unlikely(exp) __builtin_expect(!!(exp), 0)
#else
#	define likely(exp)   (exp)
#	define unlikely(exp) (exp)
#endif

extern const PixelStubs* const pixelConstStubsPtr;

// pmap_new x y colour {{{1
static int glue_pmap_new(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	int				x, y;
	_pel			colour;
	gimp_image_t	*new;

	enum {A_cmd, A_X, A_Y, A_COLOUR, A_objc}; CHECK_ARGS("x y colour");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&colour.c));

	new = pmap_new(x, y, colour);
	// pmap_new leaves the buffer uninitialised by design (internal callers
	// overwrite immediately).  Script-visible pmaps may be inspected before
	// being written to (eg. stringified), so honour the requested fill here.
	//asm_pelset(new->pixel_data, colour, x*y);
	pmap_clr(new, colour);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


// pmap_clr dest colour {{{1
static int glue_pmap_clr(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	_pel			colour;
	gimp_image_t	*dest;

	enum {A_cmd, A_DEST, A_COLOUR, A_objc}; CHECK_ARGS("dest colour");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&colour.c));

	pmap_clr(dest, colour);

	return TCL_OK;
}


// pmapf_clr dest colour {{{1
static int glue_pmapf_clr(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	pelf			colour;
	struct pmapf*	dest;
	
	enum {A_cmd, A_DEST, A_COLOUR, A_objc}; CHECK_ARGS("dest colour");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[A_COLOUR], &colour));
	
	pmapf_clr(dest, colour);

	return TCL_OK;
}


// pmap_cut src x1 y1 x2 y2 {{{1
static int glue_pmap_cut(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*src;
	gimp_image_t	*new;
	int				x1, y1, x2, y2;
	
	enum {A_cmd, A_SRC, A_X1, A_Y1, A_X2, A_Y2, A_objc}; CHECK_ARGS("src x1 y1 x2 y2");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X1], &x1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y1], &y1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X2], &x2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y2], &y2));

	new = pmap_cut(src, x1, y1, x2, y2);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


// pmap_paste dest src x y flags {{{1
static int glue_pmap_paste(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	int					x, y, flags;
	
	enum {A_cmd, A_DEST, A_SRC, A_X, A_Y, A_FLAGS, A_objc}; CHECK_ARGS("dest src x y flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FLAGS], &flags));

	pmap_paste(dest, src, x, y, flags);

	return TCL_OK;
}


// pmap_paste_ref dest src ref x y flags {{{1
static int glue_pmap_paste_ref(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	gimp_image_t		*ref;
	int					x, y, flags;
	
	enum {A_cmd, A_DEST, A_SRC, A_REF, A_X, A_Y, A_FLAGS, A_objc}; CHECK_ARGS("dest src ref x y flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_REF], &ref));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FLAGS], &flags));

	pmap_paste_ref(dest, src, ref, x, y, flags);

	return TCL_OK;
}


// pmap_patch dest src sx sy sw sh dx dy flags {{{1
static int glue_pmap_patch(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	int					sx, sy, sw, sh, dx, dy, flags;
	
	enum {A_cmd, A_DEST, A_SRC, A_SX, A_SY, A_SW, A_SH, A_DX, A_DY, A_FLAGS, A_objc}; CHECK_ARGS("dest src sx sy sw sh dx dy flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SX], &sx));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SY], &sy));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SW], &sw));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SH], &sh));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_DX], &dx));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_DY], &dy));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FLAGS], &flags));

	pmap_patch(dest, src, sx, sy, sw, sh, dx, dy, flags);

	return TCL_OK;
}


// pmap_compose dest src x y flags {{{1
static int glue_pmap_compose(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	gimp_image_t		*new;
	int					x, y, flags;
	
	enum {A_cmd, A_DEST, A_SRC, A_X, A_Y, A_FLAGS, A_objc}; CHECK_ARGS("dest src x y flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FLAGS], &flags));

	new = pmap_compose(dest, src, x, y, flags);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


// pmap_filter dest flags factor {{{1
static int glue_pmap_filter(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t *	dest;
	int				flags;
	double			factor;
	
	enum {A_cmd, A_DEST, A_FLAGS, A_FACTOR, A_objc}; CHECK_ARGS("dest flags factor");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FLAGS], &flags));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_FACTOR], &factor));

	pmap_filter(dest, flags, factor);

	return TCL_OK;
}


// pmap_dropshadow src {{{1
static int glue_pmap_dropshadow(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*src;
	gimp_image_t	*new;
	int				blur;
	
	enum {A_cmd, A_SRC, A_BLUR, A_objc}; CHECK_ARGS("src blur");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_BLUR], &blur));

	new = pmap_dropshadow(src, blur);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	
	return TCL_OK;
}


// pmap_rotate src quads {{{1
static int glue_pmap_rotate(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*src;
	gimp_image_t	*new;
	int				quads;
	
	enum {A_cmd, A_SRC, A_QUADS, A_objc}; CHECK_ARGS("src quads");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_QUADS], &quads));

	new = pmap_rotate(src, quads);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	
	return TCL_OK;
}


// pmap_info pmap {{{1
static int glue_pmap_info(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	Tcl_Obj			*res;
	gimp_image_t	*pmap;
	
	enum {A_cmd, A_PMAP, A_objc}; CHECK_ARGS("pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmap->width)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmap->height)));

	Tcl_SetObjResult(interp, res);
	
	return TCL_OK;
}


// pmapf_info pmapf {{{1
static int glue_pmapf_info(ClientData /*unused*/, Tcl_Interp *interp, 
		int objc, Tcl_Obj *const objv[])
{
	Tcl_Obj*		res;
	struct pmapf*	pmapf;
	
	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &pmapf));

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmapf->width)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmapf->height)));

	Tcl_SetObjResult(interp, res);
	
	return TCL_OK;
}


// dup obj {{{1
static int glue_dup(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	enum {A_cmd, A_OBJ, A_objc}; CHECK_ARGS("obj");

	Tcl_SetObjResult(interp, Tcl_DuplicateObj(objv[A_OBJ]));

	return TCL_OK;
}

// blend frompel topel degree {{{1
static int glue_blend(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	_pel			frompel, topel, newpel;
	double			degree;

	enum {A_cmd, A_FROMPEL, A_TOPEL, A_DEGREE, A_objc}; CHECK_ARGS("frompel topel degree");
	
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FROMPEL], (int *)&frompel.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_TOPEL], (int *)&topel.c));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_DEGREE], &degree));

	if (degree < 0 || degree > 1.0)
		THROW_ERROR("degree out of range [0.0 - 1.0]");

//	fprintf(stderr, "frompel: 0x%08x topel: 0x%08x degree: %f\n",
//			frompel.c, topel.c, degree);
	newpel.ch.a = frompel.ch.a + (int)((topel.ch.a - frompel.ch.a) * degree);
	newpel.ch.r = frompel.ch.r + (int)((topel.ch.r - frompel.ch.r) * degree);
	newpel.ch.g = frompel.ch.g + (int)((topel.ch.g - frompel.ch.g) * degree);
	newpel.ch.b = frompel.ch.b + (int)((topel.ch.b - frompel.ch.b) * degree);
//	fprintf(stderr, "newpel.ch.a[%x] = frompel.ch.a[%x] + (int)((topel.ch.a[%x] - frompel.ch.a)[%x] * degree[%f]);\n", newpel.ch.a, frompel.ch.a, topel.ch.a, topel.ch.a - frompel.ch.a, degree);
//	fprintf(stderr, "newpel.ch.r[%x] = frompel.ch.r[%x] + (int)((topel.ch.r[%x] - frompel.ch.r)[%x] * degree[%f]);\n", newpel.ch.r, frompel.ch.r, topel.ch.r, topel.ch.r - frompel.ch.r, degree);
//	fprintf(stderr, "newpel.ch.g[%x] = frompel.ch.g[%x] + (int)((topel.ch.g[%x] - frompel.ch.g)[%x] * degree[%f]);\n", newpel.ch.g, frompel.ch.g, topel.ch.g, topel.ch.g - frompel.ch.g, degree);
//	fprintf(stderr, "newpel.ch.b[%x] = frompel.ch.b[%x] + (int)((topel.ch.b[%x] - frompel.ch.b)[%x] * degree[%f]);\n", newpel.ch.b, frompel.ch.b, topel.ch.b, topel.ch.b - frompel.ch.b, degree);

	Tcl_SetObjResult(interp, Tcl_NewIntObj(newpel.c));
	
	return TCL_OK;
}


// center x y w h pmap {{{1
static int glue_center(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	int				x, y, w, h, pw, ph, cx, cy;
	gimp_image_t	*pmap;
	Tcl_Obj			*res;

	enum {A_cmd, A_X, A_Y, A_W, A_H, A_PMAP, A_objc}; CHECK_ARGS("x y w h pmap");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_W], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_H], &h));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));
	
	pw = pmap->width;
	ph = pmap->height;

	res = Tcl_NewListObj(0, NULL);

	cx = x + (int)(w/2.0 - pw/2.0);
	cy = y + (int)(h/2.0 - ph/2.0);

	Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(cx));
	Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(cy));
	
	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_put_pixel(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t		*dest;
	_pel				col;
	int					x, y, flags;
	
	enum {A_cmd, A_DEST, A_X, A_Y, A_COLOUR, A_FLAGS, A_objc}; CHECK_ARGS("dest x y colour flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&col));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FLAGS], &flags));

	put_pixel(dest, x, y, col, flags);

	return TCL_OK;
}


static int glue_get_pixel(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t		*src;
	_pel				col;
	int					x, y;
	Tcl_Obj				*res;
	
	enum {A_cmd, A_SRC, A_X, A_Y, A_objc}; CHECK_ARGS("src x y");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));

	col = get_pixel(src, x, y);

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.r)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.g)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.b)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.a)));

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_digest_region(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*src;
	int				x, y, w, h;
	unsigned int	r, g, b, a;
	Tcl_Obj			*res;

	enum {A_cmd, A_SRC, A_X, A_Y, A_W, A_H, A_objc}; CHECK_ARGS("src x y w h");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_W], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_H], &h));

	digest_region(src, x, y, w, h, &r, &g, &b, &a);

	res = Tcl_NewObj();
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(r)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(g)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(b)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(a)));
	
	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


// box dest x y w h colour flags {{{1
static int glue_box(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t *	pmap;
	int				x, y, w, h, flags;
	_pel			colour;
	
	enum {A_cmd, A_DEST, A_X, A_Y, A_W, A_H, A_COLOUR, A_FLAGS, A_objc}; CHECK_ARGS("dest x y w h colour flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_DEST], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_W], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_H], &h));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&colour.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FLAGS], &flags));
	
	box(pmap, x, y, w, h, colour, flags);

	return TCL_OK;
}


// line x1 y1 x2 y2 colour pmap {{{1
static int glue_line(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	int				x1, y1, x2, y2;
	_pel			col;
	gimp_image_t	*dest;
	
	enum {A_cmd, A_X1, A_Y1, A_X2, A_Y2, A_COLOUR, A_PMAP, A_objc}; CHECK_ARGS("x1 y1 x2 y2 colour pmap");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X1], &x1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y1], &y1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X2], &x2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y2], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&col.c));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &dest));
	
	line(x1, y1, x2, y2, col, dest);
	
	return TCL_OK;
}

// line_aa x1 y1 x2 y2 colour pmap {{{1
static int glue_line_aa(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	int				x1, y1, x2, y2;
	_pel			col;
	gimp_image_t	*dest;
	
	enum {A_cmd, A_X1, A_Y1, A_X2, A_Y2, A_COLOUR, A_PMAP, A_objc}; CHECK_ARGS("x1 y1 x2 y2 colour pmap");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X1], &x1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y1], &y1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X2], &x2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y2], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&col.c));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &dest));
	
	line_aa(x1, y1, x2, y2, col, dest);
	
	return TCL_OK;
}


// line_aa_osa x1 y1 x2 y2 colour osa pmap {{{1
static int glue_line_aa_osa(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	double			x1, y1, x2, y2;
	int				osa;
	_pel			col;
	gimp_image_t	*dest;
	
	enum {A_cmd, A_X1, A_Y1, A_X2, A_Y2, A_COLOUR, A_OSA, A_PMAP, A_objc}; CHECK_ARGS("x1 y1 x2 y2 colour osa pmap");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_X1], &x1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_Y1], &y1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_X2], &x2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_Y2], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&col.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_OSA], &osa));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &dest));
	
	line_aa_osa(x1, y1, x2, y2, col, osa, dest);
	
	return TCL_OK;
}


static int glue_pmapf_new(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int		width, height;

	enum {A_cmd, A_WIDTH, A_HEIGHT, A_objc};
	CHECK_ARGS("width height");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH],	&width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT],	&height));
	struct pmapf* p = pmapf_new(width, height);
	pmapf_clr(p, (pelf){.ch={0}});
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(p));

	return TCL_OK;
}


static int glue_pmapf_to_pmap(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	struct pmapf*	pmapf;

	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &pmapf));
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmapf_to_pmap(pmapf)));

	return TCL_OK;
}


static int glue_pmap_to_pmapf(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	gimp_image_t*	pmap;

	enum {A_cmd, A_PMAP, A_objc}; CHECK_ARGS("pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(pmap_to_pmapf(pmap)));

	return TCL_OK;
}


// bezier x1 y1 cpx1 cpy1 cpx2 cpy2 x2 y2 colour osa pmap {{{1
static int glue_bezier(ClientData /*unused*/, Tcl_Interp *interp,
		int objc, Tcl_Obj *const objv[])
{
	double			x1, y1, cpx1, cpy1, cpx2, cpy2, x2, y2;
	int				osa;
	_pel			col;
	gimp_image_t	*dest;
	
	enum {A_cmd, A_X1, A_Y1, A_CPX1, A_CPY1, A_CPX2, A_CPY2, A_X2, A_Y2, A_COLOUR, A_OSA, A_PMAP, A_objc}; CHECK_ARGS("x1 y1 cpx1 cpy1 cpx2 cpy2 x2 y2 colour osa pmap");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_X1], &x1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_Y1], &y1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_CPX1], &cpx1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_CPY1], &cpy1));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_CPX2], &cpx2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_CPY2], &cpy2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_X2], &x2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_Y2], &y2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_COLOUR], (int *)&col.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_OSA], &osa));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &dest));
	
	bezier(x1, y1, cpx1, cpy1, cpx2, cpy2, x2, y2, col, osa, dest);
	
	return TCL_OK;
}


int Pixel_GetPELFFromObj(Tcl_Interp* interp, Tcl_Obj* obj, pelf* out) // Temporary: make this a proper Tcl_ObjType {{{
{
	Tcl_Obj**	ov;
	Tcl_Size	oc;
	int			c;

	if (Tcl_ListObjGetElements(interp, obj, &oc, &ov) != TCL_OK)
		return TCL_ERROR;

	if (oc < 3 || oc > 4)
		THROW_ERROR("pelf colour must be a list of 3 or 4 values between 0.0 and 1.0: {a r g b}");

	for (c=0; c<4; c++) {
		double	v;
		TEST_OK(Tcl_GetDoubleFromObj(interp, ov[3-c], &v));
		out->chan[c] = v;
	}

	return TCL_OK;
}

//}}}
static int glue_gradient_radial(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				width, height;
	pelf			centre_colour, outer_colour;

	enum {A_cmd, A_WIDTH, A_HEIGHT, A_CENTRE_COLOUR, A_OUTER_COLOUR, A_objc}; CHECK_ARGS("width height centre_colour outer_colour");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &height));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[A_CENTRE_COLOUR], &centre_colour));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[A_OUTER_COLOUR], &outer_colour));

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(pmapf_gradient_radial(width, height, &centre_colour, &outer_colour)));

	return TCL_OK;
}


static int glue_gradient_linear_v(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				width, height;
	pelf			top_colour, bottom_colour;

	enum {A_cmd, A_WIDTH, A_HEIGHT, A_TOP_COLOUR, A_BOTTOM_COLOUR, A_objc}; CHECK_ARGS("width height top_colour bottom_colour");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &height));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[A_TOP_COLOUR], &top_colour));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[A_BOTTOM_COLOUR], &bottom_colour));

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(pmapf_gradient_linear_v(width, height, &top_colour, &bottom_colour)));

	return TCL_OK;
}


static int glue_pmapf_alpha_over(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				x, y;
	struct pmapf*	dest = NULL;
	struct pmapf*	src = NULL;
	struct pmapf*	out = NULL;

	enum {A_cmd, A_DEST, A_SRC, A_X, A_Y, A_objc}; CHECK_ARGS("dest src x y");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_DEST], &dest));
	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y], &y));

	out = pmapf_alpha_over(dest, src, x, y);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}


static int glue_pmapf_rotate_90(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				quads;
	struct pmapf*	src = NULL;
	struct pmapf*	out = NULL;

	enum {A_cmd, A_SRC, A_QUADS, A_objc}; CHECK_ARGS("src quads");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_QUADS], &quads));

	out = pmapf_rotate_90(src, quads);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}


static int glue_pmapf_cut(ClientData /*unused*/, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) //{{{1
{
	struct pmapf*			src = NULL;
	struct pmapf* restrict	new = NULL;
	int						x1, y1, x2, y2;
	
	enum {A_cmd, A_SRC, A_X1, A_Y1, A_X2, A_Y2, A_objc}; CHECK_ARGS("src x1 y1 x2 y2");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_SRC], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X1], &x1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y1], &y1));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_X2], &x2));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_Y2], &y2));

	new = pmapf_cut(src, x1, y1, x2, y2);

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(new));

	return TCL_OK;
}


static int glue_rle_encode(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*pmap;
	unsigned char	*rle_data;
	unsigned int	len;
	int				status;

	enum {A_cmd, A_PMAP, A_objc}; CHECK_ARGS("pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));

	rle_data = rle_encode(pmap, &len, &status);
	if (rle_data == NULL)
		THROW_ERROR("Error encoding RLE: ", Tcl_GetString(Tcl_NewIntObj(status)));

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(rle_data, len));
	
	return TCL_OK;
}


static int glue_rle_decode(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*pmap;
	unsigned char	*rle_data;
	Tcl_Size		len;
	int				status;

	enum {A_cmd, A_RLE_DATA, A_objc}; CHECK_ARGS("rle_data");

	rle_data = Tcl_GetByteArrayFromObj(objv[A_RLE_DATA], &len);

	pmap = rle_decode(rle_data, len, &status);
	if (pmap == NULL)
		THROW_ERROR("Error decoding RLE: ", Tcl_GetString(Tcl_NewIntObj(status)));

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));
	
	return TCL_OK;
}


static int glue_pmap2bmp(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
#ifdef BMP_CRUFT
	gimp_image_t	*pmap;
	unsigned char	*bmp;
	int				x, y;
	unsigned int	size, pixels, bperline, pad;
	_pel			*s;
	unsigned char	*d;
#endif

	enum {A_cmd, A_PMAP, A_objc}; CHECK_ARGS("pmap");

#ifndef BMP_CRUFT
	THROW_ERROR("Need to rewrite bmp code");
#else
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));

	pixels = pmap->width * pmap->height;
	bperline = pmap->width * 3;
	while (bperline & 0x3) bperline++;
	pad = bperline - pmap->width * 3;
	size = 54 + bperline * pmap->height;
	d = bmp = (unsigned char *)malloc(size);

	*d++ = 'B';
	*d++ = 'M';
	*((unsigned int *)d)++ = size; 
	*((unsigned int *)d)++ = 0;
	*((unsigned int *)d)++ = 54;
	*((unsigned int *)d)++ = 40;
	*((unsigned int *)d)++ = pmap->width;
	*((unsigned int *)d)++ = pmap->height;
	*((unsigned int *)d)++ = 1 + (24 << 16);
	*((unsigned int *)d)++ = 0;
	*((unsigned int *)d)++ = bperline * pmap->height;
	*((unsigned int *)d)++ = 75*39;
	*((unsigned int *)d)++ = 75*39;
	*((unsigned int *)d)++ = 0;
	*((unsigned int *)d)++ = 0;
	for (y = pmap->height-1; y >= 0; y--) {
		s = pmap->pixel_data + (y * pmap->width);
		for (x = 0; x < pmap->width; x++, s++) {
			*d++ = s->ch.b;
			*d++ = s->ch.g;
			*d++ = s->ch.r;
		}
		for (x = 0; x < pad; x++)
			*d++ = 0;
		if ((unsigned int)d - (unsigned int)bmp > size)
			fprintf(stderr, "Eeek! overran buffer\n");
	}
	
	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(bmp, size));

	free(bmp);
#endif

	return TCL_OK;
}


static int glue_pmap_sub(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*pmap1, *pmap2;
	_pel			*s1, *s2, *d;
	_pel			*s1_max, *s2_max, *d_max;
	_pel			colour = {.c = 0};
	int				i, total;
	gimp_image_t	*new;

	enum {A_cmd, A_PMAP1, A_PMAP2, A_objc}; CHECK_ARGS("pmap1 pmap2");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP1], &pmap1));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP2], &pmap2));

	if (pmap1->width != pmap2->width || pmap1->height != pmap2->height)
		THROW_ERROR("Both pmaps must have the same dimensions");

	new = pmap_new(pmap1->width, pmap1->height, colour);

	s1_max = pmap1->pixel_data + pmap1->width * pmap1->height;
	s2_max = pmap2->pixel_data + pmap2->width * pmap2->height;
	d_max = new->pixel_data + new->width * new->height;

	total = pmap1->width * pmap1->height;
	for (
			i = 0,
			s1 = pmap1->pixel_data,
			s2 = pmap2->pixel_data,
			d = new->pixel_data
		;
			i < total
		;
			i++,
			s1++,
			s2++,
			d++
	) {
		if (d >= d_max) Tcl_Panic("d (%p) is out of range %p + (%d*%d) = %p",
				(void*)d, (void*)new->pixel_data,
				new->width, new->height,
				(void*)d_max);
		if (s1 >= s1_max) Tcl_Panic("s1 is out of range");
		if (s2 >= s2_max) Tcl_Panic("s2 is out of range");
		d->ch.a = abs(s1->ch.a - s2->ch.a);
		d->ch.b = abs(s1->ch.b - s2->ch.b);
		d->ch.g = abs(s1->ch.g - s2->ch.g);
		d->ch.r = abs(s1->ch.r - s2->ch.r);
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	return TCL_OK;
}


static int glue_channel_histogram(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*pmap;
	_pel			*s;
	int				i, total;
	Tcl_Obj			*res, *ch_a, *ch_b, *ch_g, *ch_r;
	int				count_r[256];
	int				count_g[256];
	int				count_b[256];
	int				count_a[256];

	enum {A_cmd, A_PMAP, A_objc}; CHECK_ARGS("pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));

	for (i=0; i<256; i++) {
		count_r[i] = 0;
		count_g[i] = 0;
		count_b[i] = 0;
		count_a[i] = 0;
	}

	total = pmap->width * pmap->height;
	for (i=0, s=pmap->pixel_data; i<total; i++, s++) {
		count_r[s->ch.r]++;
		count_g[s->ch.g]++;
		count_b[s->ch.b]++;
		count_a[s->ch.a]++;
	}

	ch_r = Tcl_NewListObj(0, NULL);
	ch_g = Tcl_NewListObj(0, NULL);
	ch_b = Tcl_NewListObj(0, NULL);
	ch_a = Tcl_NewListObj(0, NULL);

	for (i=0; i<256; i++) {
		TEST_OK(Tcl_ListObjAppendElement(interp, ch_r, Tcl_NewIntObj(count_r[i])));
		TEST_OK(Tcl_ListObjAppendElement(interp, ch_g, Tcl_NewIntObj(count_g[i])));
		TEST_OK(Tcl_ListObjAppendElement(interp, ch_b, Tcl_NewIntObj(count_b[i])));
		TEST_OK(Tcl_ListObjAppendElement(interp, ch_a, Tcl_NewIntObj(count_a[i])));
	}

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj("red", -1)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, ch_r));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj("green", -1)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, ch_g));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj("blue", -1)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, ch_b));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj("alpha", -1)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, ch_a));

	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}


static int glue_flatten_sv(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*pmap;
	_pel			*s;
	int				i, total;
	double			h, xs, xv, new_s, new_v;

	enum {A_cmd, A_PMAP, A_S, A_V, A_objc}; CHECK_ARGS("pmap s v");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_S], &new_s));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_V], &new_v));

	total = pmap->width * pmap->height;
	for (i=0, s=pmap->pixel_data; i<total; i++, s++) {
		rgb2hsv(s->ch.r, s->ch.g, s->ch.b, &h, &xs, &xv);
		if (h >= 18 && h <= 23) continue;
		if (h > 12 && h < 18 && xs < 90 && xs > 60 && xv < 30 && xv > 10) continue;
		if (h < 27 && h > 23 && xs > 30 && xs < 69 && xv > 59 && xv < 90) continue;
		s->ch.r = 0;
		s->ch.g = 0;
		s->ch.b = 0;
		s->ch.a = 0;
		//hsv2rgb(h, new_s, new_v, &(s->ch.r), &(s->ch.g), &(s->ch.b));
		//hsv2rgb(h, xs, xv, &(s->ch.r), &(s->ch.g), &(s->ch.b));
		//hsv2rgb(h, xs*0.1+new_s, xv*0.1+new_v, &(s->ch.r), &(s->ch.g), &(s->ch.b));
		//hsv2rgb(h, xs*0.05+new_s, xv*0.05+new_v, &(s->ch.r), &(s->ch.g), &(s->ch.b));
		//hsv2rgb(h, xs, new_v, &(s->ch.r), &(s->ch.g), &(s->ch.b));
		//hsv2rgb(h, new_s, xv, &(s->ch.r), &(s->ch.g), &(s->ch.b));
	}

	return TCL_OK;
}


static int glue_rgb2hsv(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	int			r, g, b;
	double		h, s, v;
	Tcl_Obj		*res;

	enum {A_cmd, A_R, A_G, A_B, A_objc}; CHECK_ARGS("r g b");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_R], &r));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_G], &g));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_B], &b));

	rgb2hsv(r, g, b, &h, &s, &v);

	res = Tcl_NewListObj(0, NULL);

	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(h)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(s)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(v)));

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_hsv2rgb(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	unsigned char	r, g, b;
	double			h, s, v;
	Tcl_Obj			*res;

	enum {A_cmd, A_H, A_S, A_V, A_objc}; CHECK_ARGS("h s v");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_H], &h));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_S], &s));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_V], &v));

	hsv2rgb(h, s, v, &r, &g, &b);

	res = Tcl_NewListObj(0, NULL);

	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(r)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(g)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(b)));

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_process_image_hsv(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*pmap;
	_pel			*s;
	int				total;
	Tcl_Obj			*o[5] = {0};	defer { for (int i=0; i<5; ++i) replace_tclobj(&o[i], NULL); }
	Tcl_Obj			**newvals;
	Tcl_Size		new_c;
	double			nh, ns, nv;
	int				na;

	enum {A_cmd, A_PMAP, A_CB, A_objc};
	CHECK_ARGS("pmap cb");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &pmap));

	total = pmap->width * pmap->height;
	int i;
	for (i=0, s=pmap->pixel_data; i<total; i++, s++) {
		double			h, xs, xv;

		rgb2hsv(s->ch.r, s->ch.g, s->ch.b, &h, &xs, &xv);

		replace_tclobj(&o[0], objv[A_CB]);
		replace_tclobj(&o[1], Tcl_NewDoubleObj(h));
		replace_tclobj(&o[2], Tcl_NewDoubleObj(xs));
		replace_tclobj(&o[3], Tcl_NewDoubleObj(xv));
		replace_tclobj(&o[4], Tcl_NewIntObj(s->ch.a));

		switch (Tcl_EvalObjv(interp, 5, o, TCL_EVAL_GLOBAL)) {
			case TCL_OK:
				break;

			case TCL_RETURN:
			case TCL_BREAK:
				return TCL_OK;

			case TCL_CONTINUE:
				continue;

			case TCL_ERROR:
				return TCL_ERROR;

			default:
				THROW_ERROR("Unexpected return code");
		}

		TEST_OK(Tcl_ListObjGetElements(interp, Tcl_GetObjResult(interp),
					&new_c, &newvals));

		if (new_c != 4)
			THROW_PRINTF("New values must be a list of 4 elements (h, s, v, a), got %" TCL_SIZE_MODIFIER "d elements: (%s)",
					new_c, Tcl_GetString(Tcl_GetObjResult(interp)));

		TEST_OK(Tcl_GetDoubleFromObj(interp, newvals[0], &nh));
		TEST_OK(Tcl_GetDoubleFromObj(interp, newvals[1], &ns));
		TEST_OK(Tcl_GetDoubleFromObj(interp, newvals[2], &nv));
		TEST_OK(Tcl_GetIntFromObj(interp, newvals[3], &na));

		hsv2rgb(nh, ns, nv, &(s->ch.r), &(s->ch.g), &(s->ch.b));
		s->ch.a = (uint8)na;
	}

	return TCL_OK;
}


static int glue_scale_pmap(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	gimp_image_t	*pmap;
	gimp_image_t	*scaled;
	int				new_w, new_h;

	enum {A_cmd, A_SRC, A_NEW_WIDTH, A_NEW_HEIGHT, A_objc}; CHECK_ARGS("src new_width new_height");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_SRC], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_NEW_WIDTH], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_NEW_HEIGHT], &new_h));

	if (new_w <= 0) THROW_ERROR("new_width must be greater than 0");
	if (new_h <= 0) THROW_ERROR("new_height must be greater than 0");

	scaled = scale_pmap(pmap,
			0, 0, pmap->width, pmap->height,
			0, 0, new_w, new_h);

	if (scaled == NULL)
		THROW_ERROR("Error scaling pmap");

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(scaled));

	return TCL_OK;
}


static int glue_image_mimetype(ClientData /*unused*/, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *const objv[])
{
	unsigned char*	bytes;
	Tcl_Size		len;

	enum {A_cmd, A_BYTES, A_objc}; CHECK_ARGS("bytes");

	bytes = Tcl_GetByteArrayFromObj(objv[A_BYTES], &len);

	if (len >= 4) {
		// JPEG
		if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[len-2] == 0xff && bytes[len-1] == 0xd9) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/jpeg", 10));
			return TCL_OK;
		}
	}

	// Seems like these jpegs are encoded in an obsolete version of the JFIF standard from 1991
	if (len >= 11) {
		// JPEG/JFIF 1.01?
		if (
				bytes[0] == 0xFF &&
				bytes[1] == 0xD8 &&
				bytes[2] == 0xFF &&
				bytes[3] == 0xE0 &&
				bytes[6] == 'J' &&
				bytes[7] == 'F' &&
				bytes[8] == 'I' &&
				bytes[9] == 'F' &&
				bytes[10] == 0x00
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/jpeg", 10));
			return TCL_OK;
		}
		// JPEG/Exif 1.02?
		if (
				bytes[0] == 0xFF &&
				bytes[1] == 0xD8 &&
				bytes[2] == 0xFF &&
				bytes[3] == 0xE1 &&
				bytes[6] == 'E' &&
				bytes[7] == 'x' &&
				bytes[8] == 'i' &&
				bytes[9] == 'f' &&
				bytes[10] == 0x00
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/jpeg", 10));
			return TCL_OK;
		}
	}

	if (len >= 8) {
		// PNG
		if (
				bytes[0] == 0x89 &&
				bytes[1] == 'P' &&
				bytes[2] == 'N' &&
				bytes[3] == 'G' &&
				bytes[4] == 0x0d &&
				bytes[5] == 0x0a &&
				bytes[6] == 0x1a &&
				bytes[7] == 0x0a
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/png", 9));
			return TCL_OK;
		}
	}

	if (len >= 12) {
		// WEBP
		if (
				bytes[0]  == 'R' &&
				bytes[1]  == 'I' &&
				bytes[2]  == 'F' &&
				bytes[3]  == 'F' &&
				bytes[4]  == ((len-8)         & 0xff) &&
				bytes[5]  == (((len-8) >>  8) & 0xff) &&
				bytes[6]  == (((len-8) >> 16) & 0xff) &&
				bytes[7]  == (((len-8) >> 24) & 0xff) &&
				bytes[8]  == 'W' &&
				bytes[9]  == 'E' &&
				bytes[10] == 'B' &&
				bytes[11] == 'P'
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/webp", 10));
			return TCL_OK;
		}
	}

	if (len >= 5) {
		// GIF
		if (
				bytes[0] == 'G' &&
				bytes[1] == 'I' &&
				bytes[2] == 'F' &&
				bytes[3] == '8' &&
				bytes[4] == '9'
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/gif", 9));
			return TCL_OK;
		}
	}

	if (len >= 2) {
		// BMP
		if (
				bytes[0] == 'B' &&
				bytes[1] == 'M'
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/bmp", 9));
			return TCL_OK;
		}
	}

	if (len >= 4) {
		// TIFF
		if (
				bytes[0] == 'I' &&
				bytes[1] == 'I' &&
				bytes[2] == '*' &&
				bytes[3] == 0
		   ) {
			// little endian
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/tiff", 10));
			return TCL_OK;
		}
		if (
				bytes[0] == 'M' &&
				bytes[1] == 'M' &&
				bytes[2] == 0 &&
				bytes[3] == '*'
		   ) {
			// big endian
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/tiff", 10));
			return TCL_OK;
		}
	}

	if (len >= 4) {
		// PSD
		if (
				bytes[0] == '8' &&
				bytes[1] == 'B' &&
				bytes[2] == 'P' &&
				bytes[3] == 'S'
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/vnd.adobe.photoshop", 25));
			return TCL_OK;
		}
	}

	if (len >= 12) {
		// HEIC
		if (
				bytes[4]  == 'f' &&
				bytes[5]  == 't' &&
				bytes[6]  == 'y' &&
				bytes[7]  == 'p' &&
				bytes[8]  == 'h' &&
				bytes[9]  == 'e' &&
				bytes[10] == 'i' &&
				bytes[11] == 'c'
		   ) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("image/heic", 10));
			return TCL_OK;
		}
	}

	Tcl_SetErrorCode(interp, "PIXEL", "CORE", "UNKNOWN_FILETYPE", NULL);
	Tcl_SetObjResult(interp, Tcl_NewStringObj("unknown", 7));
	return TCL_ERROR;
}


/*
static double L(int a, double x) //{{{1
{
	return x == 0 ? 1.0 : (a * sinf(PI*x) * sinf(PI*x/a)) / (PI*PI * x*x);
}


static double L2d(int a, double x, double y) //{{{1
{
	if (abs(x) > a || abs(y) > a) return 0.0;
	return L(a, x) * L(a, y);
}


static _pel S(int La, double x, double y, gimp_image_t* pmap) //{{{1
{
	double	r=0,g=0,b=0,a=0, weight=0;
	_pel	out;
	int		xi, yi, x_from, x_to, y_from, y_to;

	y_from = floorf(y) - La+1;
	y_to   = floorf(y) + La;

	x_from = floorf(x) - La+1;
	x_to   = floorf(x) + La;

	// Clamp to source space
	if (y_from < 0) y_from = 0;
	if (x_from < 0) x_from = 0;
	if (y_to > pmap->height) y_to = pmap->height;
	if (x_to > pmap->width)  x_to = pmap->width;

	for (yi=y_from; yi < y_to; yi+=1) {
		for (xi=x_from; xi < x_to; xi+=1) {
			// (xi, yi) is the source sample coords
			_pel	src = pmap->pixel_data[yi*pmap->width + xi];
			double	Lf = L2d(La, x-xi, y-yi);
			double	Lff = Lf / 255.0;

			r += src.ch.r * Lff;
			g += src.ch.g * Lff;
			b += src.ch.b * Lff;
			a += src.ch.a * Lff;
			weight += Lf;
		}
	}

	// Normalize
	r /= weight;
	g /= weight;
	b /= weight;
	a /= weight;

	// Clamp
	if (r < 0) r = 0.0;
	if (g < 0) g = 0.0;
	if (b < 0) b = 0.0;
	if (a < 0) a = 0.0;
	if (r > 1.0) r = 1.0;
	if (g > 1.0) g = 1.0;
	if (b > 1.0) b = 1.0;
	if (a > 1.0) a = 1.0;

	// Convert to integer samples
	out.ch.r = (int)(r*255);
	out.ch.g = (int)(g*255);
	out.ch.b = (int)(b*255);
	out.ch.a = (int)(a*255);
	return out;
}


static int glue_scale_pmap_lanczos(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	gimp_image_t*	src = NULL;
	gimp_image_t*	dst = NULL;
	int				new_w, new_h, xi, yi;
	double			x, y;	// interpolated coordinates in the source for the new sample

	enum {A_cmd, A_PMAP, A_WIDTH, A_HEIGHT, A_objc}; CHECK_ARGS("pmap width height");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &new_h));

	dst = pmap_new(new_w, new_h, (_pel){.c = 0});

	for (yi = 0; yi<new_h; yi++) {
		for (xi = 0; xi<new_w; xi++) {
			x = src->width * (double)(xi+.0)/new_w;		// +.35 should be .5, not sure why but that causes a small displacement
			y = src->height * (double)(yi+.0)/new_h;
			dst->pixel_data[yi*new_w + xi] = S(2, x, y, src);
		}
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dst));

	return TCL_OK;
}

//}}}1
*/

static int initvars(Tcl_Interp* interp) //{{{1
{
#define MIRROR_FLAG(name, value) \
	if (Tcl_ObjSetVar2(interp, Tcl_NewStringObj((name), -1), NULL, \
			Tcl_NewIntObj((value)), TCL_LEAVE_ERR_MSG) == NULL) return TCL_ERROR;

	MIRROR_FLAG("pixel::MD_BLUR",			MD_BLUR);
	MIRROR_FLAG("pixel::MD_CHANNEL_MASK",	MD_CHANNEL_MASK);
	MIRROR_FLAG("pixel::MD_FILTER_R",		MD_FILTER_R);
	MIRROR_FLAG("pixel::MD_FILTER_G",		MD_FILTER_G);
	MIRROR_FLAG("pixel::MD_FILTER_B",		MD_FILTER_B);
	MIRROR_FLAG("pixel::MD_FILTER_A",		MD_FILTER_A);
	MIRROR_FLAG("pixel::MD_FILTER_SMOOTH",	MD_FILTER_SMOOTH);
	MIRROR_FLAG("pixel::MD_FILTER_ALPHA",	MD_FILTER_ALPHA);
	MIRROR_FLAG("pixel::MD_FILTER_ALPHA_Q",	MD_FILTER_ALPHA_Q);
	MIRROR_FLAG("pixel::MD_BLEND",			MD_BLEND);
	MIRROR_FLAG("pixel::MD_BLIT",			MD_BLIT);
	MIRROR_FLAG("pixel::MD_ALPHA",			MD_ALPHA);
	MIRROR_FLAG("pixel::MD_ALPHA_UNDER",	MD_ALPHA_UNDER);
	MIRROR_FLAG("pixel::MD_ADDITIVE",		MD_ADDITIVE);
	MIRROR_FLAG("pixel::MD_SCALE_SQUARE",	MD_SCALE_SQUARE);
	MIRROR_FLAG("pixel::MD_SCALE_SINE",		MD_SCALE_SINE);
	MIRROR_FLAG("pixel::MD_SCALE_LINEAR",	MD_SCALE_LINEAR);
	MIRROR_FLAG("pixel::MD_MASK_ALPHA",		MD_MASK_ALPHA);
	MIRROR_FLAG("pixel::MD_MASK_RED",		MD_MASK_RED);
	MIRROR_FLAG("pixel::MD_MASK_GREEN",		MD_MASK_GREEN);
	MIRROR_FLAG("pixel::MD_MASK_BLUE",		MD_MASK_BLUE);

#undef MIRROR_FLAG

	return TCL_OK;
}


//}}}1
static int glue_dump_pmapf(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	t = NULL;
	uint32_t		x, y;
	int				c;
	Tcl_Obj*		res = Tcl_NewObj();

	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &t));

	for (y=0; y<t->height; y++) {
		for (x=0; x<t->width; x++) {
			Tcl_AppendPrintfToObj(res, "(%u, %u):", x, y);
			for (c=0; c<4; c++)
				Tcl_AppendPrintfToObj(res, " %.4f", t->pixel_data[y*t->width+x].chan[c]);
			Tcl_AppendPrintfToObj(res, "\n");
		}
	}

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}

//}}}
static int glue_invert(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	uint32_t		x, y;
	int				c;
	struct pmapf*	out = NULL;
	pelf*			i;
	pelf*			o;

	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &in));
	out = pmapf_new(in->width, in->height);

	i = in->pixel_data;
	o = out->pixel_data;

	for (y=0; y<in->height; y++)
		for (x=0; x<in->width; x++, i++, o++) {
			for (c=0; c<3; c++)
				o->chan[c] = 1.0 - i->chan[c];
				//o->chan[c] = -i->chan[c];
			o->ch.a = i->ch.a;
		}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}
static int glue_neg(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	uint32_t		x, y;
	int				c;
	struct pmapf*	out = NULL;
	pelf* restrict	i;
	pelf* restrict	o;

	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &in));
	out = pmapf_new(in->width, in->height);

	i = in->pixel_data;
	o = out->pixel_data;

	for (y=0; y<in->height; y++)
		for (x=0; x<in->width; x++, i++, o++) {
			for (c=0; c<3; c++)
				o->chan[c] = -i->chan[c];
			o->ch.a = i->ch.a;
		}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}
static int glue_mul(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	double			factor;

	enum {A_cmd, A_PMAPF, A_FACTOR, A_objc}; CHECK_ARGS("pmapf factor");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_FACTOR], &factor));

	dst = pmapf_new(src->width, src->height);

	{
		pelf* restrict	s = src->pixel_data;
		pelf* restrict	d = dst->pixel_data;
		const int len = src->height * src->width;
		int i, c;

		for (i=0; i<len; i++, d++, s++) {
			for (c=0; c<3; c++)
				d->chan[c] = s->chan[c] * factor;
			d->chan[c] = s->chan[c];
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}
static int glue_add(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	pmapf_a = NULL;
	struct pmapf*	pmapf_b = NULL;
	uint32_t		x, y;
	int				c;
	struct pmapf*	out = NULL;
	pelf*			a;
	pelf*			b;
	pelf*			o;

	enum {A_cmd, A_PMAPF_A, A_PMAPF_B, A_objc}; CHECK_ARGS("pmapf_a pmapf_b");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF_A], &pmapf_a));
	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF_B], &pmapf_b));

	if (pmapf_a->height != pmapf_b->height || pmapf_a->width != pmapf_b->width)
		THROW_ERROR("Both input pmapfs must be the same dimensions");

	out = pmapf_new(pmapf_a->width, pmapf_a->height);

	a = pmapf_a->pixel_data;
	b = pmapf_b->pixel_data;
	o = out->pixel_data;

	for (y=0; y<out->height; y++)
		for (x=0; x<out->width; x++, a++, b++, o++) {
			for (c=0; c<3; c++)
				o->chan[c] = a->chan[c] + b->chan[c];
			o->ch.a = a->ch.a * b->ch.a;
		}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}
static int glue_fade(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	uint32_t		x, y;
	int				c;
	double			factor;
	struct pmapf*	out = NULL;
	pelf*			i;
	pelf*			o;

	enum {A_cmd, A_PMAPF, A_FACTOR, A_objc}; CHECK_ARGS("pmapf factor");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &in));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[A_FACTOR], &factor));

	out = pmapf_new(in->width, in->height);

	i = in->pixel_data;
	o = out->pixel_data;

	for (y=0; y<in->height; y++)
		for (x=0; x<in->width; x++, i++, o++) {
			for (c=0; c<3; c++)
				o->chan[c] = i->chan[c];
			o->ch.a = i->ch.a * factor;
		}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}
static inline float clamp_chan(float in) //{{{
{
	return in < 0.0 ? 0.0 :
		in > 1.0 ? 1.0 :
		in;
}

//}}}
static int glue_clamp(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	struct pmapf*	out = NULL;
	uint32_t		x, y;
	int				c;
	pelf*			i;
	pelf*			o;

	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &in));

	out = pmapf_new(in->width, in->height);

	i = in->pixel_data;
	o = out->pixel_data;

	for (y=0; y<in->height; y++)
		for (x=0; x<in->width; x++, i++, o++)
			for (c=0; c<4; c++)
				o->chan[c] = clamp_chan(o->chan[c]);

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}
static int glue_mirror_x(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	struct pmapf*	out = NULL;

	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &in));

	out = pmapf_new(in->width, in->height);

	for (uint32_t y=0; y<in->height; y++) {
		pelf* restrict	i = in->pixel_data + y*in->width;
		pelf* restrict	o = out->pixel_data + y*out->width + out->width - 1;
		for (uint32_t x=0; x<in->width; x++, i++, o--) {
			float* restrict	op = o->chan;
			float* restrict	ip = i->chan;

			for (int c=0; c<4; c++)
				op[c] = ip[c];
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}
static int glue_mirror_y(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	int				y, i;
	pelf* restrict	s;
	pelf* restrict	d;

	enum {A_cmd, A_PMAPF, A_objc}; CHECK_ARGS("pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));

	dst = pmapf_new(src->width, src->height);

	{
		const int h = dst->height;
		const int w = dst->width;
		const int len = w * 4;

		for (y=0; y<h; y++) {
			s = src->pixel_data + y*w;
			d = dst->pixel_data + (h-y-1)*w;
			{
				float* restrict	dp = d->chan;
				float* restrict	sp = s->chan;

				for (i=0; i<len; i++)
					*dp++ = *sp++;
			}
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}
static int glue_depixelize(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	uint32_t	x, y, sx=0, sy=0;
	int			run;
	_pel*	i;
	_pel*	o;
	_pel	last;

	gimp_image_t*	in = NULL;
	gimp_image_t*	out = NULL;

	enum {A_cmd, A_PMAP, A_objc}; CHECK_ARGS("pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[A_PMAP], &in));

	int*	samp_x = (int*)malloc(in->width * sizeof(int));
	defer { free(samp_x); }
	int*	samp_y = (int*)malloc(in->height * sizeof(int));
	defer { free(samp_y); }
	memset(samp_x, 0, in->width * sizeof(int));
	memset(samp_y, 0, in->height * sizeof(int));

	// Find sample intervals across x
	for (y=0; y<in->height; y++) {
		last = in->pixel_data[y*in->width];
		i = in->pixel_data + y*in->width + 1;
		run = 1;
		for (x=1, sx=0; x<in->width; x++, i++) {
			if (i->c == last.c && (samp_x[sx] == 0 || run < samp_x[sx])) {
				run++;
			} else {
				if (samp_x[sx] == 0) {
					samp_x[sx] = run;
				} else if (run < samp_x[sx]) {
					memmove(samp_x+sx+1, samp_x+sx, in->width-sx-1);
					samp_x[sx] = run;
					samp_x[sx+1] -= run;
				}
				run = 1;
				last = *i;
				sx++;
			}
		}

		if (samp_x[sx] == 0) {
			samp_x[sx] = run;
		} else if (run < samp_x[sx]) {
			memmove(samp_x+sx+1, samp_x+sx, in->width-sx-1);
			samp_x[sx] = run;
		}
		sx++;
	}

	/*
	{
		int t;
		fprintf(stderr, "Samp_x[%d]:\n", sx);
		for (t=0; samp_x[t]>0; t++)
			fprintf(stderr, " %d\n", samp_x[t]);
	}
	*/

	// Find sample intervals across y
	for (x=0; x<in->width; x++) {
		i = in->pixel_data + in->width;
		last = *i;
		run = 1;
		for (y=1, sy=0; y<in->height; y++, i+=in->width) {
			if (i->c == last.c && (samp_y[sy] == 0 || run < samp_y[sy])) {
				run++;
			} else {
				if (samp_y[sy] == 0) {
					samp_y[sy] = run;
				} else if (run < samp_y[sy]) {
					memmove(samp_y+sy+1, samp_y+sy, in->width-sy-1);
					samp_y[sy] = run;
					samp_y[sy+1] -= run;
				}
				run = 1;
				last = *i;
				sy++;
			}
		}

		if (samp_y[sy] == 0) {
			samp_y[sy] = run;
		} else if (run < samp_y[sy]) {
			memmove(samp_y+sy+1, samp_y+sy, in->width-sy-1);
			samp_y[sy] = run;
		}
		sy++;
	}

	/*
	{
		int t;
		fprintf(stderr, "Samp_y[%d]:\n", sy);
		for (t=0; samp_y[t]>0; t++)
			fprintf(stderr, " %d\n", samp_y[t]);
	}
	*/

	out = pmap_new(sx, sy, (_pel){.c = 0});
	o = out->pixel_data;
	for (sy=0, y=0; sy<out->height; y+=samp_y[sy], sy++) {
		i = in->pixel_data + y*in->width;
		for (sx=0, x=0; sx<out->width; x+=samp_x[sx], i+=samp_x[sx], sx++, o++)
			*o = *i;
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(out));

	return TCL_OK;
}

//}}}
static int glue_closeup(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	int		x, y, c, factor;

	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;

	enum {A_cmd, A_PMAPF, A_FACTOR, A_objc}; CHECK_ARGS("pmapf factor");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FACTOR], &factor));

	{
		const int new_w = src->width * factor;
		const int new_h = src->height * factor;
		float* restrict dp;

		dst = pmapf_new(new_w, new_h);
		dp = (float*)(dst->pixel_data);

		for (y=0; y<new_h; y++) {
			const int old_y = y/factor;
			for (x=0; x<new_w; x++) {
				const int old_x = x/factor;
				pelf* restrict s = src->pixel_data + old_y*src->width + old_x;
				float* restrict sp = s->chan;
				for (c=0; c<4; c++)
					*dp++ = sp[c];
			}
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}
static int glue_closeup_grid(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	int		x, y, c, factor;

	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;

	enum {A_cmd, A_PMAPF, A_FACTOR, A_objc}; CHECK_ARGS("pmapf factor");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[A_PMAPF], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_FACTOR], &factor));

	{
		const int new_w = src->width * factor;
		const int new_h = src->height * factor;
		float* restrict dp;

		dst = pmapf_new(new_w, new_h);
		dp = (float*)(dst->pixel_data);

		for (y=0; y<new_h; y++) {
			const int old_y = y/factor;
			for (x=0; x<new_w; x++) {
				const int old_x = x/factor;
				pelf* restrict s = src->pixel_data + old_y*src->width + old_x;
				float* restrict sp = s->chan;
				if (x%factor == 0 || y%factor == 0) {
					for (c=0; c<3; c++)
						*dp++ = 1.0-sp[c];
					*dp++ = (old_x%10 == 0 || old_y%10 == 0) ? 1.0 : .5;
				} else {
					for (c=0; c<4; c++)
						*dp++ = sp[c];
				}
			}
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}
static int glue_checkerboard(ClientData /*unused*/, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	int		x, y, c, new_w, new_h, size;
	pelf	col[2];

	struct pmapf*	dst = NULL;

	enum {A_cmd, A_WIDTH, A_HEIGHT, A_SIZE, A_COL1, A_COL1_2, A_objc}; CHECK_ARGS("width height size col1 col1");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_WIDTH], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_HEIGHT], &new_h));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[A_SIZE], &size));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[A_COL1], &col[0]));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[A_COL1_2], &col[1]));

	{
		float* restrict dp;

		dst = pmapf_new(new_w, new_h);
		dp = (float*)(dst->pixel_data);

		for (y=0; y<new_h; y++) {
			const int old_y = y/size;
			for (x=0; x<new_w; x++) {
				const int old_x = x/size;
				float* restrict sp = col[(old_x+old_y)%2].chan;
				for (c=0; c<4; c++)
					*dp++ = sp[c];
			}
		}
	}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}
int Pixel_Init(Tcl_Interp *interp) // {{{1
{
	if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) return TCL_ERROR;

	Tcl_RegisterObjType(&tcl_pmap);

	init_2d();

	NEW_CMD("pixel::pmap_new", glue_pmap_new);
	NEW_CMD("pixel::pmap_clr", glue_pmap_clr);
	NEW_CMD("pixel::pmap_cut", glue_pmap_cut);
	NEW_CMD("pixel::pmap_paste", glue_pmap_paste);
	NEW_CMD("pixel::pmap_paste_ref", glue_pmap_paste_ref);
	NEW_CMD("pixel::pmap_patch", glue_pmap_patch);
	NEW_CMD("pixel::pmap_compose", glue_pmap_compose);
	NEW_CMD("pixel::pmap_filter", glue_pmap_filter);
	NEW_CMD("pixel::pmap_dropshadow", glue_pmap_dropshadow);
	NEW_CMD("pixel::pmap_rotate", glue_pmap_rotate);
	NEW_CMD("pixel::pmap_info", glue_pmap_info);
	NEW_CMD("pixel::dup", glue_dup);
	NEW_CMD("pixel::blend", glue_blend);
	NEW_CMD("pixel::center", glue_center);
	NEW_CMD("pixel::put_pixel", glue_put_pixel);
	NEW_CMD("pixel::get_pixel", glue_get_pixel);
	NEW_CMD("pixel::digest_region", glue_digest_region);

	// pmapf
	NEW_CMD("pixel::pmapf_new", glue_pmapf_new);
	NEW_CMD("pixel::pmapf_clr", glue_pmapf_clr);
	NEW_CMD("pixel::pmapf_info", glue_pmapf_info);
	NEW_CMD("pixel::pmap_to_pmapf", glue_pmap_to_pmapf);
	NEW_CMD("pixel::pmapf_to_pmap", glue_pmapf_to_pmap);
	NEW_CMD("pixel::gradient_radial", glue_gradient_radial);
	NEW_CMD("pixel::gradient_linear_v", glue_gradient_linear_v);
	NEW_CMD("pixel::pmapf_alpha_over", glue_pmapf_alpha_over);
	NEW_CMD("pixel::pmapf_rotate_90", glue_pmapf_rotate_90);
	NEW_CMD("pixel::pmapf_cut", glue_pmapf_cut);
	NEW_CMD("pixel::dump_pmapf", glue_dump_pmapf);
	NEW_CMD("pixel::invert", glue_invert);
	NEW_CMD("pixel::neg", glue_neg);
	NEW_CMD("pixel::mul", glue_mul);
	NEW_CMD("pixel::add", glue_add);
	NEW_CMD("pixel::fade", glue_fade);
	NEW_CMD("pixel::clamp", glue_clamp);
	NEW_CMD("pixel::mirror_x", glue_mirror_x);
	NEW_CMD("pixel::mirror_y", glue_mirror_y);

	// Primitives
	NEW_CMD("pixel::box", glue_box);
	NEW_CMD("pixel::line", glue_line);
	NEW_CMD("pixel::line_aa", glue_line_aa);
	NEW_CMD("pixel::line_aa_osa", glue_line_aa_osa);
	NEW_CMD("pixel::bezier", glue_bezier);

	// Misc
	NEW_CMD("pixel::rle_encode", glue_rle_encode);
	NEW_CMD("pixel::rle_decode", glue_rle_decode);
	NEW_CMD("pixel::pmap2bmp", glue_pmap2bmp);
	NEW_CMD("pixel::pmap_sub", glue_pmap_sub);
	NEW_CMD("pixel::channel_histogram", glue_channel_histogram);
	NEW_CMD("pixel::flatten_sv", glue_flatten_sv);
	NEW_CMD("pixel::rgb2hsv", glue_rgb2hsv);
	NEW_CMD("pixel::hsv2rgb", glue_hsv2rgb);
	NEW_CMD("pixel::process_image_hsv", glue_process_image_hsv);
	NEW_CMD("pixel::scale_pmap", glue_scale_pmap);
	NEW_CMD("pixel::image_mimetype", glue_image_mimetype);
	NEW_CMD("pixel::depixelize", glue_depixelize);
	NEW_CMD("pixel::closeup", glue_closeup);
	NEW_CMD("pixel::closeup_grid", glue_closeup_grid);
	NEW_CMD("pixel::checkerboard", glue_checkerboard);

	TEST_OK(lanczos_init(interp));
	TEST_OK(perceptual_scaling_init(interp));

	TEST_OK(initvars(interp));

	TEST_OK(Tcl_PkgProvideEx(interp, PACKAGE_NAME, PACKAGE_VERSION, (void *)pixelConstStubsPtr));
	TEST_OK(Tcl_PkgProvideEx(interp, "Pixel", PACKAGE_VERSION, (void *)pixelConstStubsPtr));

	return TCL_OK;
}


