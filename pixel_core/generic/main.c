#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include "pixel.h"

#define SQUARE		0
#define	SINE		1
#define LINEAR		2

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

	TEST_OK(initvars(interp));

	TEST_OK(Tcl_PkgProvideEx(interp, PACKAGE_NAME, PACKAGE_VERSION, &pixelStubs));

	return TCL_OK;
}


