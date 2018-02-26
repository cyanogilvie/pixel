#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include "pixel.h"
#include <math.h>

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

extern PixelStubs pixelStubs;

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


// pmapf_clr dest colour {{{1
static int glue_pmapf_clr(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	pelf			colour;
	struct pmapf*	dest;
	
	CHECK_ARGS(2, "dest colour");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &dest));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[2], &colour));
	
	pmapf_clr(dest, colour);

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


// pmap_paste_ref dest src ref x y flags {{{1
static int glue_pmap_paste_ref(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*dest;
	gimp_image_t		*src;
	gimp_image_t		*ref;
	int					x, y, flags;
	
	CHECK_ARGS(6, "dest src ref x y flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &dest));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &src));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[3], &ref));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[6], &flags));

	pmap_paste_ref(dest, src, ref, x, y, flags);

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


// pmapf_info pmapf {{{1
static int glue_pmapf_info(ClientData *foo, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj*		res;
	struct pmapf*	pmapf;
	
	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &pmapf));

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmapf->width)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(pmapf->height)));

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

// blend frompel topel degree {{{1
static int glue_blend(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	_pel			frompel, topel, newpel;
	double			degree;

	CHECK_ARGS(3, "frompel topel degree");
	
	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int *)&frompel.c));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], (int *)&topel.c));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &degree));

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
static int glue_center(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				x, y, w, h, pw, ph, cx, cy;
	gimp_image_t	*pmap;
	Tcl_Obj			*res;

	CHECK_ARGS(5, "x y w h pmap");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &h));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[5], &pmap));
	
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


static int glue_put_pixel(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*dest;
	_pel				col;
	int					x, y, flags;
	
	CHECK_ARGS(5, "dest x y colour flags");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &dest));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], (int *)&col));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &flags));

	put_pixel(dest, x, y, col, flags);

	return TCL_OK;
}


static int glue_get_pixel(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*src;
	_pel				col;
	int					x, y;
	Tcl_Obj				*res;
	
	CHECK_ARGS(3, "src x y");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &y));

	col = get_pixel(src, x, y);

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.r)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.g)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.b)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(col.ch.a)));

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_digest_region(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	gimp_image_t	*src;
	int				x, y, w, h;
	unsigned int	r, g, b, a;
	Tcl_Obj			*res;

	CHECK_ARGS(5, "src x y w h");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &h));

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


static int glue_pmapf_new(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				width, height;

	CHECK_ARGS(2, "width height");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &height));
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(pmapf_new(width, height)));

	return TCL_OK;
}


static int glue_pmapf_to_pmap(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	struct pmapf*	pmapf;

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &pmapf));
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmapf_to_pmap(pmapf)));

	return TCL_OK;
}


static int glue_pmap_to_pmapf(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	gimp_image_t*	pmap;

	CHECK_ARGS(1, "pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(pmap_to_pmapf(pmap)));

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


int Pixel_GetPELFFromObj(Tcl_Interp* interp, Tcl_Obj* obj, pelf* out) // Temporary: make this a proper Tcl_ObjType {{{
{
	Tcl_Obj**	ov;
	int			oc, c;

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
static int glue_gradient_radial(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				width, height;
	pelf			centre_colour, outer_colour;

	CHECK_ARGS(4, "width height centre_colour outer_colour");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &height));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[3], &centre_colour));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[4], &outer_colour));

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(pmapf_gradient_radial(width, height, &centre_colour, &outer_colour)));

	return TCL_OK;
}


static int glue_gradient_linear_v(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				width, height;
	pelf			top_colour, bottom_colour;

	CHECK_ARGS(4, "width height top_colour bottom_colour");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &height));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[3], &top_colour));
	TEST_OK(Pixel_GetPELFFromObj(interp, objv[4], &bottom_colour));

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(pmapf_gradient_linear_v(width, height, &top_colour, &bottom_colour)));

	return TCL_OK;
}


static int glue_pmapf_alpha_over(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{1
{
	int				x, y;
	struct pmapf*	dest = NULL;
	struct pmapf*	src = NULL;
	struct pmapf*	out = NULL;

	CHECK_ARGS(4, "dest src x y");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &dest));
	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[2], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &y));

	out = pmapf_alpha_over(dest, src, x, y);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}


static int glue_rle_encode(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap;
	unsigned char	*rle_data;
	unsigned int	len;
	int				status;

	CHECK_ARGS(1, "pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

	rle_data = rle_encode(pmap, &len, &status);
	if (rle_data == NULL)
		THROW_ERROR("Error encoding RLE: ", Tcl_GetString(Tcl_NewIntObj(status)));

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(rle_data, len));
	
	return TCL_OK;
}


static int glue_rle_decode(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap;
	unsigned char	*rle_data;
	unsigned int	len;
	int				status;

	CHECK_ARGS(1, "rle_data");

	rle_data = Tcl_GetByteArrayFromObj(objv[1], (int *)&len);

	pmap = rle_decode(rle_data, len, &status);
	if (pmap == NULL)
		THROW_ERROR("Error decoding RLE: ", Tcl_GetString(Tcl_NewIntObj(status)));

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));
	
	return TCL_OK;
}


static int glue_pmap2bmp(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
#ifdef BMP_CRUFT
	gimp_image_t	*pmap;
	unsigned char	*bmp;
	int				x, y;
	unsigned int	size, pixels, bperline, pad;
	_pel			*s;
	unsigned char	*d;
#endif

	CHECK_ARGS(1, "pmap");

#ifndef BMP_CRUFT
	THROW_ERROR("Need to rewrite bmp code");
#else
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

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


static int glue_pmap_sub(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap1, *pmap2;
	_pel			*s1, *s2, *d;
	_pel			*s1_max, *s2_max, *d_max;
	_pel			colour;
	int				i, total;
	gimp_image_t	*new;

	CHECK_ARGS(2, "pmap1 pmap2");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap1));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap2));

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
				d, new->pixel_data,
				new->width, new->height,
				d_max);
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


static int glue_channel_histogram(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap;
	_pel			*s;
	int				i, total;
	Tcl_Obj			*res, *ch_a, *ch_b, *ch_g, *ch_r;
	int				count_r[256];
	int				count_g[256];
	int				count_b[256];
	int				count_a[256];

	CHECK_ARGS(1, "pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

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


static int glue_flatten_sv(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap;
	_pel			*s;
	int				i, total;
	double			h, xs, xv, new_s, new_v;

	CHECK_ARGS(3, "pmap s v");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &new_s));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &new_v));

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


static int glue_rgb2hsv(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int			r, g, b;
	double		h, s, v;
	Tcl_Obj		*res;

	CHECK_ARGS(3, "r g b");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &r));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &g));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &b));

	rgb2hsv(r, g, b, &h, &s, &v);

	res = Tcl_NewListObj(0, NULL);

	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(h)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(s)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(v)));

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_hsv2rgb(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	unsigned char	r, g, b;
	double			h, s, v;
	Tcl_Obj			*res;

	CHECK_ARGS(3, "h s v");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &h));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &s));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &v));

	hsv2rgb(h, s, v, &r, &g, &b);

	res = Tcl_NewListObj(0, NULL);

	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(r)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(g)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(b)));

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_process_image_hsv(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap;
	_pel			*s;
	int				i, j, total, res;
	double			h, xs, xv;
	Tcl_Obj			*cb;
	Tcl_Obj			*o[5];
	Tcl_Obj			**newvals;
	int				new_c;
	double			nh, ns, nv;
	int				na;

	CHECK_ARGS(2, "pmap cb");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	cb = objv[2];

	total = pmap->width * pmap->height;
	for (i=0, s=pmap->pixel_data; i<total; i++, s++) {
		rgb2hsv(s->ch.r, s->ch.g, s->ch.b, &h, &xs, &xv);

		o[0] = cb;
		o[1] = Tcl_NewDoubleObj(h);
		o[2] = Tcl_NewDoubleObj(xs);
		o[3] = Tcl_NewDoubleObj(xv);
		o[4] = Tcl_NewIntObj(s->ch.a);

		Tcl_IncrRefCount(cb);
		for (j=0; j<5; j++) Tcl_IncrRefCount(o[j]);
		res = Tcl_EvalObjv(interp, 5, o, TCL_EVAL_GLOBAL);
		for (j=0; j<5; j++) Tcl_DecrRefCount(o[j]);
		Tcl_DecrRefCount(cb);
		switch (res) {
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
			THROW_ERROR("New values must be a list of 4 elements (h, s, v, a), got ", Tcl_GetString(Tcl_NewIntObj(new_c))," elements: (", Tcl_GetString(Tcl_GetObjResult(interp)), ")");

		TEST_OK(Tcl_GetDoubleFromObj(interp, newvals[0], &nh));
		TEST_OK(Tcl_GetDoubleFromObj(interp, newvals[1], &ns));
		TEST_OK(Tcl_GetDoubleFromObj(interp, newvals[2], &nv));
		TEST_OK(Tcl_GetIntFromObj(interp, newvals[3], &na));

		hsv2rgb(nh, ns, nv, &(s->ch.r), &(s->ch.g), &(s->ch.b));
		s->ch.a = (uint8)na;
	}

	return TCL_OK;
}


static int glue_scale_pmap(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	gimp_image_t	*pmap;
	gimp_image_t	*scaled;
	int				new_w, new_h;

	CHECK_ARGS(3, "src new_width new_height");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &new_h));

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


static int glue_image_mimetype(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	unsigned char*	bytes;
	int				len;

	CHECK_ARGS(1, "bytes");

	bytes = Tcl_GetByteArrayFromObj(objv[1], &len);

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

	CHECK_ARGS(3, "pmap width height");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &new_h));

	dst = pmap_new(new_w, new_h, (_pel)(uint32_t)0);

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

#define Wt			9
#define W			(Wt*2+1)
#define FIXED_BITS	22		// Max is min(fixedpoint's bits-10, uint64_t's bits/2-1)
#define FIXED_TO_DOUBLE(f) ((f) / (double)(1LL<<FIXED_BITS))
typedef int32_t fixedpoint;

static float w1(float fc, float x) //{{{
{
	return x==0 ? 2*fc : sinf(2*PI*fc*x) / (PI*x);
}

//}}}
static float w2(int wt, float x) //{{{
{
	if (x < -wt || x > wt) {
		return 0.0;
	} else if (x == 0) {
		return 1.0;
	} else {
		const float X = PI * (x/wt);
		return sinf(X) / X;
	}
}

//}}}
static float h(int wt, float fc, float x) //{{{
{
	return w1(fc, x) * w2(wt, x);
}

//}}}
static void make_kern(fixedpoint* kern, int wt, const float f, const float frac, const int kstart, const int kend) //{{{
{
	int			k;
	fixedpoint	acc = 0;

	// Calculate the kernel weights for this fractional v
	for (k=kstart; k<kend; k++)
		acc += (kern[k] = h(wt, f, -wt+k-frac) * (1LL<<FIXED_BITS));

	// Normalize the kernel to eliminate bias
	for (k=kstart; k<kend; k++)
		kern[k] = ((int64_t)(kern[k])<<FIXED_BITS) / acc;
}

//}}}
static void make_kern_float(float* kern, int wt, const float f, const float frac, const int kstart, const int kend) //{{{
{
	int			k;
	float		acc = 0;

	// Calculate the kernel weights for this fractional v
	for (k=kstart; k<kend; k++)
		acc += (kern[k] = h(wt, f, -wt+k-frac));

	// Normalize the kernel to eliminate bias
	for (k=kstart; k<kend; k++)
		kern[k] /= acc;
}

//}}}
static int glue_kern_vis(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	out = NULL;
	int				width, height, x, k, yofs, from_x, from_y, to_x, to_y, wt, w;
	fixedpoint*		kern = NULL;
	double			frac, f, kf, k0;
	fixedpoint		min = 256LL<<FIXED_BITS, max = -256LL<<FIXED_BITS;
	double			minf, maxf, xf, yf;
	_pel			red = (_pel)(uint32_t)0xffff0000;
	_pel			green = (_pel)(uint32_t)0xff00ff00;
	_pel			lightgreen = (_pel)(uint32_t)0xffa0ffa0;
	_pel			black = (_pel)(uint32_t)0xffffffff;
	_pel			white = (_pel)(uint32_t)0xff000000;
	_pel			grey = (_pel)(uint32_t)0xffa0a0a0;
	_pel			yellow = (_pel)(uint32_t)0xffffff00;

	CHECK_ARGS(5, "width height wt f frac");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &height));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &wt));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &f));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[5], &frac));

	w = wt*2+1;
	kern = ckalloc(sizeof(fixedpoint) * w);

	out = pmap_new(width, height, (_pel)(uint32_t)0);
	pmap_clr(out, white);
	memset(kern, 0, w);
	make_kern(kern, wt, f, frac, 0, w);

	for (k=0; k<w; k++) {
		if (kern[k] < min) min = kern[k];
		if (kern[k] > max) max = kern[k];
	}

	minf = FIXED_TO_DOUBLE(min);
	maxf = FIXED_TO_DOUBLE(max);

	yf = 0.7 * (height-1) / (maxf-minf);
	xf = (width-1) / (double)w;

	yofs = maxf*yf + height*((1-0.7)/2.0);

	// Y: kern to out space: y_out = yofs - y*yf
	// X: kern to out space: x_out = x*xf

	/*
	0 = -wt + kf - frac
	-kf = -wt -frac
	kf = wt + frac
	w * x/width = wt + frac
	x = (width * (wt + frac)) / w
	*/
	// Zero in kernel space
	k0 = (width * (wt + frac)) / w +.5;
	line(k0, 0, k0, height-1, green, out);
	for (kf = k0+xf; kf < width; kf += xf)
		line(kf, 0, kf, height-1, lightgreen, out);
	for (kf = k0-xf; kf > 0; kf -= xf)
		line(kf, 0, kf, height-1, lightgreen, out);

	// Zero line
	line(0, yofs, width-1, yofs, red, out);

	// Kernel taps
	k = 0;
	from_x = (k * xf)+.5;
	from_y = (yofs - FIXED_TO_DOUBLE(kern[k])*yf)+.5;
	line(from_x, 0, from_x, height-1, yellow, out);
	for (k=1; k<w; k++) {
		to_x = (k * xf)+.5;
		to_y = (yofs - FIXED_TO_DOUBLE(kern[k])*yf)+.5;

		//fprintf(stderr, "(%d, %d) -> (%d, %d)\n", from_x, from_y, to_x, to_y);
		line_aa(from_x, from_y, to_x, to_y, black, out);

		from_x = to_x;
		from_y = to_y;
		line(from_x, 0, from_x, height-1, yellow, out);
	}

	// Kernel
	kf = 0;
	from_x = 0;
	from_y = (yofs - h(wt, f, -wt+k-frac) * yf)+.5;
	for (x=1; x<width; x++) {
		kf = w * x/(double)width;

		to_x = x;
		to_y = (yofs - h(wt, f, -wt+kf-frac) * yf)+.5;

		if (to_x < 0) to_x = 0;
		if (to_y < 0) to_y = 0;
		if (to_x >= width) to_x = width-1;
		if (to_y >= height) to_y = height-1;

		//fprintf(stderr, "(%d, %d) -> (%d, %d)\n", from_x, from_y, to_x, to_y);
		line_aa(from_x, from_y, to_x, to_y, grey, out);

		from_x = to_x;
		from_y = to_y;
	}

	ckfree(kern);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(out));
	return TCL_OK;
}

//}}}
/*
static int glue_scale_pmap_lanczos2(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	src = NULL;
	gimp_image_t*	dst = NULL;
	int				new_w, new_h, xi, yi, k;
	float			x, y, xinc, yinc;	// interpolated coordinates in the source for the new sample
	float			xf, yf;
	_pel*			t;
	fixedpoint		kern[W];

	CHECK_ARGS(3, "pmap width height");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &new_h));

	// Intermediate buffer - scaled down on the horizontal axis
	//dst = pmap_new(new_w, src->height, (_pel)(uint32_t)0);
	//dst = pmap_new(src->width, src->height, (_pel)(uint32_t)0);
	dst = pmap_new(new_w, new_h, (_pel)(uint32_t)0);

	//t = (_pel*)malloc(src->width*src->height*sizeof(_pel));
	t = (_pel*)malloc(dst->width*src->height*sizeof(_pel));

	//t = dst->pixel_data;

	xf = (float)new_w / (2.0*src->width);
	xinc = src->width / (float)dst->width;
	for (xi=0, x=0; xi<dst->width; xi++, x+=xinc) {
		_pel*		d = t + xi;		// Points at the top of the column
		fixedpoint	acc=0;

		for (k=0; k<W; k++) {
			float	xk = fmodf(x, 1.0) + k - Wt;
			if (x+xk < src->width && x+xk >= 0)
				acc += (kern[k] = h(xf, xk) * (1LL<<FIXED_BITS));
		}

		// Normalize the kernel to eliminate bias
		for (k=0; k<W; k++)
			kern[k] = ((int64_t)(kern[k])<<FIXED_BITS) / acc;

		// Produce a column (xi) of the intermediate image
		for (yi=0; yi<src->height; yi++, d += dst->width) {
			// +.5 - trick to do cheap rounding.  round() is astonishingly slow
			int			k=0, xr=x+.5, kend=W;
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			if (xr+kend > src->width)
				kend = src->width - xr;	// Adjust the end to the end of the row

			if (xr-Wt < 0) {	// Adjist the start to the start of the row
				k = Wt - xr;
				xr = Wt;
			}

			s = src->pixel_data + yi*src->width + (xr-Wt);

			for (;  k<kend;  k++, s++) {
				if (s->ch.a == 0) continue;

				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
				a += s->ch.a * kern[k];
			}

			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a < 0) a=0;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;

			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
			d->ch.a = a>>FIXED_BITS;
		}
	}

	yf = (float)new_h / (2.0*src->height);
	yinc = src->height / (float)dst->height;
	for (yi=0, y=0; yi<dst->height; yi++, y+=yinc) {
		_pel*		d = dst->pixel_data + yi*dst->width;	// Points at the start of the row
		fixedpoint	acc=0;

		for (k=0; k<W; k++) {
			float yk = fmodf(y, 1.0) + k - Wt;
			if (y+yk < src->height && y+yk >= 0)
				acc += (kern[k] = h(yf, yk) * (1LL<<FIXED_BITS));
		}

		// Normalize the kernel to eliminate bias
		for (k=0; k<W; k++)
			kern[k] = ((int64_t)(kern[k])<<FIXED_BITS)/acc;

		// Produce a row (yi) of the final image
		for (xi=0; xi<dst->width; xi++, d++) {
			int			k=0, kend=W, yr=y+.5;
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			if (yr+kend > src->height)
				kend = src->height - yr; // Adjust the end to the bottom of the buffer

			if (Wt > yr) { // Adjust the start to the top of the buffer
				k = Wt - yr;
				yr = Wt;
			}

			s = t + (yr-Wt)*dst->width + xi;

			for (; k<kend; k++, s+=dst->width) {
				if (s->ch.a == 0) continue;

				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
				a += s->ch.a * kern[k];
			}

			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a < 0) a=0;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;

			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
			d->ch.a = a>>FIXED_BITS;
		}
	}

	free(t); t = NULL;
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dst));

	return TCL_OK;
}

//}}}
*/
void lanczos_half_scale(_pel* in, _pel* out, int orig_dim, int new_dim, int wt, int dsinc, int iiend, int dinc, int sinc, int sif) //{{{
{
	int				w = wt*2+1;
	fixedpoint		kern[w];
	int				oi, ii;
	float			v;
	const float		vinc = orig_dim/(float)new_dim, f = fminf(.45, new_dim/(2.0*orig_dim));
	_pel*			ds = out;

	// Loop across the dimension to be scaled (oi in "out" coords, v in "in" coords)
	// v is the fractional projection of oi into "in" coord space
	for (oi=0, v=0; oi<new_dim; oi++, v+=vinc, ds+=dsinc) {
		_pel*		d=ds;
		//fixedpoint	acc=0;
		int			vi=v, kend=w, k, kstart=0;
		float		frac=v-vi;

		// Adjust the start and end ranges of k to fit within the soure buffer
		if (vi-wt+kend > orig_dim)
			kend = orig_dim - vi + wt;

		if (wt > vi)
			kstart = wt - vi;

		make_kern((fixedpoint*)&kern, wt, f, frac, kstart, kend);

		// Loop across the fixed dimension (ii in "out" coords)
		for (ii=0; ii<iiend; ii++, d+=dinc) {
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			// Point at the first source pixel for the kernel
			s = in + (vi-wt+kstart)*sinc + ii*sif;

			// Accumulate samples from "in", scaled by the kernel weights
			for (k=kstart; k<kend; k++, s+=sinc) {
				if (s->ch.a == 0) continue;		// Prevents auto-vectorization, but a big nett win if many pixels are completely transparent
				a += s->ch.a * kern[k];
				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
			}

			// Clamp output values
			if (a < 0) a=0;
			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;

			// ADC (add with carry) would really help here with the rounding
			/*
			d->ch.a = (a>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.r = (r>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.g = (g>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.b = (b>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			*/
			d->ch.a = a>>FIXED_BITS;
			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
		}
	}
}


//}}}
void lanczos_half_scale_float(pelf* in, pelf* out, int orig_dim, int new_dim, int wt, float f, int dsinc, int iiend, int dinc, int sinc, int sif) //{{{
{
	const int		w = wt*2+1;
	float			kern[w];
	int				oi, ii;
	float			v;
	const float		vinc = orig_dim/(float)new_dim;
	pelf*			ds = out;

	//fprintf(stderr, "lanczos_half_scale_float: %d -> %d: f: %.4f\n", orig_dim, new_dim, f);
	// Loop across the dimension to be scaled (oi in "out" coords, v in "in" coords)
	// v is the fractional projection of oi into "in" coord space
	for (oi=0, v=0; oi<new_dim; oi++, v+=vinc, ds+=dsinc) {
		pelf*		ss;
		pelf*		d=ds;
		int			vi=v, kend=w, k, kstart=0;
		float		frac=v-vi;

		// Adjust the start and end ranges of k to fit within the soure buffer
		if (vi-wt+kend > orig_dim)
			kend = orig_dim - vi + wt;

		if (wt > vi)
			kstart = wt - vi;

		make_kern_float((float*)&kern, wt, f, frac, kstart, kend);

		// Point at the first source pixel for the kernel
		ss = in + (vi-wt+kstart)*sinc;

		// Loop across the fixed dimension (ii in "out" coords)
		for (ii=0; ii<iiend; ii++, d+=dinc, ss+=sif) {
			int			c;
			pelf*		s = ss;

			for (c=0; c<4; c++)
				d->chan[c] = 0;

			// Accumulate samples from "in", scaled by the kernel weights
			for (k=kstart; k<kend; k++, s+=sinc)
				for (c=0; c<4; c++)
					d->chan[c] += s->chan[c] * kern[k];

			/*
			// Clamp output values
			for (c=0; c<4; c++) {
				if (d->chan[c] < 0) {
					d->chan[c] = 0;
				} else if (d->chan[c] > 1.0) {
					d->chan[c] = 1.0;
				}
			}
			*/
		}
	}
}


//}}}
void lanczos_half_scale_f(_pel* in, _pel* out, int orig_dim, int new_dim, int wt, float f, int dsinc, int iiend, int dinc, int sinc, int sif) //{{{
{
	const int		w = wt*2+1;
	fixedpoint		kern[w];
	int				oi, ii;
	float			v;
	const float		vinc = orig_dim/(float)new_dim;
	_pel*			ds = out;

	// Loop across the dimension to be scaled (oi in "out" coords, v in "in" coords)
	// v is the fractional projection of oi into "in" coord space
	for (oi=0, v=0; oi<new_dim; oi++, v+=vinc, ds+=dsinc) {
		_pel*		d=ds;
		//fixedpoint	acc=0;
		int			vi=v, kend=w, k, kstart=0;
		float		frac=v-vi;

		// Adjust the start and end ranges of k to fit within the soure buffer
		if (vi-wt+kend > orig_dim)
			kend = orig_dim - vi + wt;

		if (wt > vi)
			kstart = wt - vi;

		make_kern((fixedpoint*)&kern, wt, f, frac, kstart, kend);

		// Loop across the fixed dimension (ii in "out" coords)
		for (ii=0; ii<iiend; ii++, d+=dinc) {
			fixedpoint	r=0, g=0, b=0, a=0;
			_pel*		s;

			// Point at the first source pixel for the kernel
			s = in + (vi-wt+kstart)*sinc + ii*sif;

			// Accumulate samples from "in", scaled by the kernel weights
			for (k=kstart; k<kend; k++, s+=sinc) {
				if (s->ch.a == 0) continue;		// Prevents auto-vectorization, but a big nett win if many pixels are completely transparent
				a += s->ch.a * kern[k];
				r += s->ch.r * kern[k];
				g += s->ch.g * kern[k];
				b += s->ch.b * kern[k];
			}

			// Clamp output values
			if (a < 0) a=0;
			if (r < 0) r=0;
			if (g < 0) g=0;
			if (b < 0) b=0;
			if (a > 255LL<<FIXED_BITS) a=255LL<<FIXED_BITS;
			if (r > 255LL<<FIXED_BITS) r=255LL<<FIXED_BITS;
			if (g > 255LL<<FIXED_BITS) g=255LL<<FIXED_BITS;
			if (b > 255LL<<FIXED_BITS) b=255LL<<FIXED_BITS;

			// ADC (add with carry) would really help here with the rounding
			/*
			d->ch.a = (a>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.r = (r>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.g = (g>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			d->ch.b = (b>>FIXED_BITS) + ((r>>(FIXED_BITS-1)) & 1);
			*/
			d->ch.a = a>>FIXED_BITS;
			d->ch.r = r>>FIXED_BITS;
			d->ch.g = g>>FIXED_BITS;
			d->ch.b = b>>FIXED_BITS;
		}
	}
}


//}}}
static int glue_scale_pmap_lanczos3(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	src = NULL;
	gimp_image_t*	dst = NULL;
	gimp_image_t*	t = NULL;
	int				new_w, new_h, wt;

	if (objc < 4 || objc > 5)
		CHECK_ARGS(3, "pmapf width height ?wt?");


	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &new_h));
	if (objc >= 5) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &wt));
	} else {
		if (new_w > src->width) {
			wt = 3;
		} else if (new_w * 8 < src->width) {
			wt = 15;
		} else if (new_w * 5 < src->width) {
			wt = 13;
		} else if (new_w * 3 < src->width) {
			wt = 10;
		} else {
			wt = 9;
		}
	}

	// Intermediate buffer - scaled down on the horizontal axis
	//dst = pmap_new(new_w, src->height, (_pel)(uint32_t)0);
	//dst = pmap_new(src->width, src->height, (_pel)(uint32_t)0);
	dst = pmap_new(new_w, new_h, (_pel)(uint32_t)0);

	//t = pmap_new(src->width, src->height, (_pel)(uint32_t)0);

	/*
	t = pmap_new(dst->width, src->height, (_pel)(uint32_t)0);
	// first: scale width
	lanczos_half_scale(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			1,					// dsinc
			src->height,		// iiend
			dst->width,			// dinc
			1,					// sinc
			src->width			// sif
			);

	// second: scale height
	lanczos_half_scale(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			dst->width,			// dsinc
			dst->width,			// iiend
			1,					// dinc
			dst->width,			// sinc
			1					// sif
			);
	*/

	t = pmap_new(src->width, dst->height, (_pel)(uint32_t)0);
	// first: scale height
	lanczos_half_scale(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	//Tcl_SetObjResult(interp, Tcl_NewPMAPObj(t));
	//return TCL_OK;

	// second: scale width
	lanczos_half_scale(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);


	pmap_free(&t);
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dst));

	return TCL_OK;
}

//}}}
static int glue_lowpass_pmap_lanczos3(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	src = NULL;
	gimp_image_t*	dst = NULL;
	gimp_image_t*	t = NULL;
	double			f;

	CHECK_ARGS(2, "pmap f");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &f));

	dst = pmap_new(src->width, src->height, (_pel)(uint32_t)0);
	t = pmap_new(src->width, dst->height, (_pel)(uint32_t)0);

	// first: scale height
	lanczos_half_scale_f(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			Wt,
			f,					// f
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	//Tcl_SetObjResult(interp, Tcl_NewPMAPObj(t));
	//return TCL_OK;

	// second: scale width
	lanczos_half_scale_f(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			Wt,
			f,					// f
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);


	pmap_free(&t);
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dst));

	return TCL_OK;
}

//}}}
static int glue_scale_pmapf_lanczos3(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	struct pmapf*	t = NULL;
	int				new_w, new_h, wt;

	if (objc < 4 || objc > 5)
		CHECK_ARGS(3, "pmapf width height ?wt?");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &new_h));
	if (objc >= 5) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &wt));
	} else {
		if (new_w > src->width) {
			wt = 4;
		} else if (new_w * 8 < src->width) {
			wt = 15;
		} else if (new_w * 5 < src->width) {
			wt = 13;
		} else if (new_w * 3 < src->width) {
			wt = 10;
		} else {
			wt = 9;
		}
	}

	dst = pmapf_new(new_w, new_h);
	t = pmapf_new(src->width, dst->height);

	// first: scale height
	lanczos_half_scale_float(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			fminf(.5, dst->height/(2.0*src->height)),	// f
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	// second: scale width
	lanczos_half_scale_float(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			fminf(.5, dst->width/(2.0*t->width)),	// f
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);

	pmapf_free(&t);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}
static int glue_lowpass_pmapf_lanczos3(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	src = NULL;
	struct pmapf*	dst = NULL;
	struct pmapf*	t = NULL;
	double			f;
	int				wt;

	CHECK_ARGS(3, "pmapf wt f");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &wt));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &f));

	dst = pmapf_new(src->width, src->height);
	t = pmapf_new(src->width, dst->height);

	// first: scale height
	lanczos_half_scale_float(
			src->pixel_data,	// in
			t->pixel_data,		// out
			src->height,		// orig_dim
			dst->height,		// new_dim
			wt,
			f,					// f
			t->width,			// dsinc
			t->width,			// iiend
			1,					// dinc
			src->width,			// sinc
			1					// sif
			);

	// second: scale width
	lanczos_half_scale_float(
			t->pixel_data,		// in
			dst->pixel_data,	// out
			t->width,			// orig_dim
			dst->width,			// new_dim
			wt,
			f,					// f
			1,					// dsinc
			t->height,			// iiend
			dst->width,			// dinc
			1,					// sinc
			t->width			// sif
			);


	pmapf_free(&t);
	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(dst));

	return TCL_OK;
}

//}}}
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
static int glue_dump_pmapf(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	t = NULL;
	int				x, y, c;
	Tcl_Obj*		res = Tcl_NewObj();

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &t));

	for (y=0; y<t->height; y++) {
		for (x=0; x<t->width; x++) {
			Tcl_AppendPrintfToObj(res, "(%d, %d):", x, y);
			for (c=0; c<4; c++)
				Tcl_AppendPrintfToObj(res, " %.4f", t->pixel_data[y*t->width+x].chan[c]);
			Tcl_AppendPrintfToObj(res, "\n");
		}
	}

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}

//}}}
static int glue_invert(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	int				x, y, c;
	struct pmapf*	out = NULL;
	pelf*			i;
	pelf*			o;

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &in));
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
static int glue_neg(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	int				x, y, c;
	struct pmapf*	out = NULL;
	pelf*			i;
	pelf*			o;

	CHECK_ARGS(1, "pmapf");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &in));
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
static int glue_mul(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	int				x, y, c;
	double			factor;
	struct pmapf*	out = NULL;
	pelf*			i;
	pelf*			o;

	CHECK_ARGS(2, "pmapf factor");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &in));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &factor));

	out = pmapf_new(in->width, in->height);

	i = in->pixel_data;
	o = out->pixel_data;

	for (y=0; y<in->height; y++)
		for (x=0; x<in->width; x++, i++, o++) {
			for (c=0; c<3; c++)
				o->chan[c] = i->chan[c] * factor;
			o->ch.a = i->ch.a;
		}

	Tcl_SetObjResult(interp, Pixel_NewPMAPFObj(out));

	return TCL_OK;
}

//}}}
static int glue_add(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	pmapf_a = NULL;
	struct pmapf*	pmapf_b = NULL;
	int				x, y, c;
	struct pmapf*	out = NULL;
	pelf*			a;
	pelf*			b;
	pelf*			o;

	CHECK_ARGS(2, "pmapf_a pmapf_b");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &pmapf_a));
	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[2], &pmapf_b));

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
static int glue_fade(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	struct pmapf*	in = NULL;
	int				x, y, c;
	double			factor;
	struct pmapf*	out = NULL;
	pelf*			i;
	pelf*			o;

	CHECK_ARGS(2, "pmapf factor");

	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &in));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &factor));

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
int Pixel_Init(Tcl_Interp *interp) // {{{1
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) return TCL_ERROR;

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
	NEW_CMD("pixel::scale_pmapf_lanczos", glue_scale_pmapf_lanczos3);
	NEW_CMD("pixel::lowpass_pmapf_lanczos", glue_lowpass_pmapf_lanczos3);
	NEW_CMD("pixel::gradient_radial", glue_gradient_radial);
	NEW_CMD("pixel::gradient_linear_v", glue_gradient_linear_v);
	NEW_CMD("pixel::pmapf_alpha_over", glue_pmapf_alpha_over);
	NEW_CMD("pixel::dump_pmapf", glue_dump_pmapf);
	NEW_CMD("pixel::invert", glue_invert);
	NEW_CMD("pixel::neg", glue_neg);
	NEW_CMD("pixel::mul", glue_mul);
	NEW_CMD("pixel::add", glue_add);
	NEW_CMD("pixel::fade", glue_fade);

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
	NEW_CMD("pixel::scale_pmap_lanczos", glue_scale_pmap_lanczos3);
	NEW_CMD("pixel::lowpass_pmap_lanczos", glue_lowpass_pmap_lanczos3);
	NEW_CMD("pixel::kern_vis", glue_kern_vis);
	NEW_CMD("pixel::image_mimetype", glue_image_mimetype);

	TEST_OK(initvars(interp));

	TEST_OK(Tcl_PkgProvideEx(interp, PACKAGE_NAME, PACKAGE_VERSION, &pixelStubs));

	return TCL_OK;
}


