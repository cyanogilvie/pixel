#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include "all.h"

#define SQUARE		0
#define	SINE		1
#define LINEAR		2

typedef struct ttf_feedback_data {
	Tcl_Interp	*interp;
	Tcl_Obj		*res;
	Tcl_Obj		*line;
} ttf_feedback_data;

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


// render_ttf colour fft_face px_size text ?width? {{{1
static int glue_render_ttf(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				px_size;
	pmap_list		*pmaps;
	pmap_list		*next;
	_pel			base_col;
	FT_Face			face;
	int				len, width;
	char			*utf8;
	Tcl_Obj			*res;
	
	
	if (objc < 5 || objc > 6)
		CHECK_ARGS(4, "colour fft_face px_size text ?width?");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int *)&base_col));
	TEST_OK(Tcl_GetTTFFaceFromObj(interp, objv[2], &face));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &px_size));
	utf8 = Tcl_GetStringFromObj(objv[4], &len);
	if (objc == 6) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &width));
	} else {
		width = 0;
	}
	pmaps = render_ttf(base_col, face, px_size, 
			Tcl_GetString(objv[4]), width, NULL, NULL);

	if (pmaps == NULL)
		THROW_ERROR("Could not render text: ", Tcl_NewStringObj(ttf_last_error_txt(), -1));

	if (width == 0) {
		Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmaps->pmap));
		free(pmaps);
	} else {
		res = Tcl_NewListObj(0, NULL);

		while (1) {
			Tcl_ListObjAppendElement(interp, res, Tcl_NewPMAPObj(pmaps->pmap));
			if (pmaps->next == NULL) {
				break;
			} else {
				next = pmaps->next;
				free(pmaps);
				pmaps = next;
			}
		}
		free(pmaps);

		Tcl_SetObjResult(interp, res);
	}

	return TCL_OK;
}


// render_ttf_adv meta text {{{1

static void render_ttf_feedback(void *clientdata, int what, int value)
{
	ttf_feedback_data	*feedback = (ttf_feedback_data *)clientdata;

	switch (what) {
		case TTF_FEEDBACK_LINESTART:
			//fprintf(stderr, "render_ttf_feedback: TTF_FEEDBACK_LINESTART: %d\n", value);
			Tcl_ListObjAppendElement(feedback->interp, feedback->res,
					Tcl_NewIntObj(value));
			Tcl_ListObjAppendElement(feedback->interp, feedback->res,
					feedback->line);
			feedback->line = Tcl_NewListObj(0, NULL);
			break;

		case TTF_FEEDBACK_CHAR:
			//fprintf(stderr, "render_ttf_feedback: TTF_FEEDBACK_CHAR: %d\n", value);
			Tcl_ListObjAppendElement(feedback->interp, feedback->line,
					Tcl_NewIntObj(value));
			break;

		default:
			fprintf(stderr, "render_ttf_feedback called with bogus tag\n");
			break;
	}
}

static int glue_render_ttf_adv(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj				*base_col_o;
	Tcl_Obj				*face_o;
	Tcl_Obj				*px_size_o;
	Tcl_Obj				*width_o;
	ttf_feedback_data	feedback;
	int					px_size;
	pmap_list			*pmaps;
	pmap_list			*next;
	_pel				base_col;
	FT_Face				face;
	int					len, width;
	char				*utf8;
	Tcl_Obj				*res;
	char				*base_arr;
	
	
	
	CHECK_ARGS(2, "meta text");

	width = 0;
	px_size = 14;
	base_col.c = 0xff000000;

	base_arr = Tcl_GetString(objv[1]);

	if ((base_col_o	= Tcl_GetVar2Ex(interp, base_arr, "base_col", 0)) != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, base_col_o, (int *)&base_col.c));
	if ((face_o		= Tcl_GetVar2Ex(interp, base_arr, "face", 0)) != NULL)
		TEST_OK(Tcl_GetTTFFaceFromObj(interp, face_o, &face));
	if ((px_size_o	= Tcl_GetVar2Ex(interp, base_arr, "px_size", 0)) != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, px_size_o, &px_size));
	if ((width_o	= Tcl_GetVar2Ex(interp, base_arr, "width", 0)) != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, width_o, &width));

	feedback.interp = interp;
	feedback.res = Tcl_NewListObj(0, NULL);
	feedback.line = Tcl_NewListObj(0, NULL);

	pmaps = render_ttf(base_col, face, px_size, 
			Tcl_GetString(objv[2]), width, render_ttf_feedback, (void *)&feedback);

	if (pmaps == NULL)
		THROW_ERROR("Could not render text: ", Tcl_NewStringObj(ttf_last_error_txt(), -1));

	if (width == 0) {
		Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmaps->pmap));
		free(pmaps);
	} else {
		res = Tcl_NewListObj(0, NULL);

		while (1) {
			Tcl_ListObjAppendElement(interp, res, Tcl_NewPMAPObj(pmaps->pmap));
			if (pmaps->next == NULL) {
				break;
			} else {
				next = pmaps->next;
				free(pmaps);
				pmaps = next;
			}
		}
		free(pmaps);

		Tcl_SetObjResult(interp, res);
	}

	Tcl_ObjSetVar2(interp, objv[1], Tcl_NewStringObj("feedback", -1), feedback.res, 0);

	return TCL_OK;
}


// compile_face ttf_face {{{1
static int glue_compile_face(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	FT_Face			face;
	
	CHECK_ARGS(1, "ttf_face");

	TEST_OK(Tcl_GetTTFFaceFromObj(interp, objv[1], &face));

	return TCL_OK;
}


// ttf_info ttf_face {{{1
static int glue_ttf_info(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	FT_Face			face;
	FT_SfntName		finfo;
	Tcl_Obj			*res;
	int				i, count;
	FT_Error		err;
	Tcl_Obj			*item;

	CHECK_ARGS(1, "ttf_face");

	TEST_OK(Tcl_GetTTFFaceFromObj(interp, objv[1], &face));

	count = FT_Get_Sfnt_Name_Count(face);
	
	res = Tcl_NewObj();

	for (i=0; i<count; i++) {
		err = FT_Get_Sfnt_Name(face, i, &finfo);
		if (err != 0)
			THROW_ERROR("Error getting name info for idx: ", Tcl_GetString(Tcl_NewIntObj(i)));
		item = Tcl_NewObj();
		Tcl_ListObjAppendElement(interp, item, Tcl_NewStringObj("platform", -1));
		switch (finfo.platform_id) {
			case TT_PLATFORM_APPLE_UNICODE:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_APPLE_UNICODE", -1));
				
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_APPLE_ID_DEFAULT:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_DEFAULT", -1));
						break;

					case TT_APPLE_ID_UNICODE_1_1:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_UNICODE_1_1", -1));
						break;

					case TT_APPLE_ID_ISO_10646:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_ISO_10646", -1));
						break;

					case TT_APPLE_ID_UNICODE_2_0:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_UNICODE_2_0", -1));
						break;

					case TT_APPLE_ID_UNICODE_32:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_UNICODE_32", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_MACINTOSH:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_MACINTOSH", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_MAC_ID_ROMAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ROMAN", -1));
						break;

					case TT_MAC_ID_JAPANESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_JAPANESE", -1));
						break;

					case TT_MAC_ID_TRADITIONAL_CHINESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TRADITIONAL_CHINESE", -1));
						break;

					case TT_MAC_ID_KOREAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_KOREAN", -1));
						break;

					case TT_MAC_ID_ARABIC:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ARABIC", -1));
						break;

					case TT_MAC_ID_HEBREW:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_HEBREW", -1));
						break;

					case TT_MAC_ID_GREEK:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GREEK", -1));
						break;

					case TT_MAC_ID_RUSSIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_RUSSIAN", -1));
						break;

					case TT_MAC_ID_RSYMBOL:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_RSYMBOL", -1));
						break;

					case TT_MAC_ID_DEVANAGARI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_DEVANAGARI", -1));
						break;

					case TT_MAC_ID_GURMUKHI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GURMUKHI", -1));
						break;

					case TT_MAC_ID_GUJARATI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GUJARATI", -1));
						break;

					case TT_MAC_ID_ORIYA:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ORIYA", -1));
						break;

					case TT_MAC_ID_BENGALI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_BENGALI", -1));
						break;

					case TT_MAC_ID_TAMIL:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TAMIL", -1));
						break;

					case TT_MAC_ID_TELUGU:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TELUGU", -1));
						break;

					case TT_MAC_ID_KANNADA:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_KANNADA", -1));
						break;

					case TT_MAC_ID_MALAYALAM:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_MALAYALAM", -1));
						break;

					case TT_MAC_ID_SINHALESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SINHALESE", -1));
						break;

					case TT_MAC_ID_BURMESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_BURMESE", -1));
						break;

					case TT_MAC_ID_KHMER:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_KHMER", -1));
						break;

					case TT_MAC_ID_THAI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_THAI", -1));
						break;

					case TT_MAC_ID_LAOTIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_LAOTIAN", -1));
						break;

					case TT_MAC_ID_GEORGIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GEORGIAN", -1));
						break;

					case TT_MAC_ID_ARMENIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ARMENIAN", -1));
						break;

					/* This is the same as SIMPLIFIED CHINESE
					case TT_MAC_ID_MALDIVIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_MALDIVIAN", -1));
						break;
						*/

					case TT_MAC_ID_SIMPLIFIED_CHINESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SIMPLIFIED_CHINESE", -1));
						break;

					case TT_MAC_ID_TIBETAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TIBETAN", -1));
						break;

					case TT_MAC_ID_MONGOLIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_MONGOLIAN", -1));
						break;

					case TT_MAC_ID_GEEZ:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GEEZ", -1));
						break;

					case TT_MAC_ID_SLAVIC:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SLAVIC", -1));
						break;

					case TT_MAC_ID_VIETNAMESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_VIETNAMESE", -1));
						break;

					case TT_MAC_ID_SINDHI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SINDHI", -1));
						break;

					case TT_MAC_ID_UNINTERP:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_UNINTERP", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_ISO:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_ISO", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_ISO_ID_7BIT_ASCII:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ISO_ID_7BIT_ASCII", -1));
						break;

					case TT_ISO_ID_10646:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ISO_ID_10646", -1));
						break;

					case TT_ISO_ID_8859_1:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ISO_ID_8859_1", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_MICROSOFT:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_MICROSOFT", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_MS_ID_SYMBOL_CS:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_SYMBOL_CS", -1));
						break;

					case TT_MS_ID_UNICODE_CS:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_UNICODE_CS", -1));
						break;

					case TT_MS_ID_SJIS:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_SJIS", -1));
						break;

					case TT_MS_ID_GB2312:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_GB2312", -1));
						break;

					case TT_MS_ID_BIG_5:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_BIG_5", -1));
						break;

					case TT_MS_ID_WANSUNG:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_WANSUNG", -1));
						break;

					case TT_MS_ID_JOHAB:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_JOHAB", -1));
						break;

					case TT_MS_ID_UCS_4:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_UCS_4", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_CUSTOM:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_CUSTOM", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("UNKNOWN", -1));

				break;

			case TT_PLATFORM_ADOBE:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_ADOBE", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_ADOBE_ID_STANDARD:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ADOBE_ID_STANDARD", -1));
						break;

					case TT_ADOBE_ID_EXPERT:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ADOBE_ID_EXPERT", -1));
						break;

					case TT_ADOBE_ID_CUSTOM:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ADOBE_ID_CUSTOM", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			default:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("UNKNOWN", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

		}
		Tcl_ListObjAppendElement(interp, item, Tcl_NewStringObj("type", -1));
		switch (finfo.name_id) {
			case TT_NAME_ID_COPYRIGHT:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_COPYRIGHT", -1));
				break;

			case TT_NAME_ID_FONT_FAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_FONT_FAMILY", -1));
				break;

			case TT_NAME_ID_FONT_SUBFAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_FONT_SUBFAMILY", -1));
				break;

			case TT_NAME_ID_UNIQUE_ID:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_UNIQUE_ID", -1));
				break;

			case TT_NAME_ID_FULL_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_FULL_NAME", -1));
				break;

			case TT_NAME_ID_VERSION_STRING:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_VERSION_STRING", -1));
				break;

			case TT_NAME_ID_PS_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_PS_NAME", -1));
				break;

			case TT_NAME_ID_TRADEMARK:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_TRADEMARK", -1));
				break;

			case TT_NAME_ID_MANUFACTURER:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_MANUFACTURER", -1));
				break;

			case TT_NAME_ID_DESIGNER:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_DESIGNER", -1));
				break;

			case TT_NAME_ID_DESCRIPTION:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_DESCRIPTION", -1));
				break;

			case TT_NAME_ID_VENDOR_URL:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_VENDOR_URL", -1));
				break;

			case TT_NAME_ID_DESIGNER_URL:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_DESIGNER_URL", -1));
				break;

			case TT_NAME_ID_LICENSE:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_LICENSE", -1));
				break;

			case TT_NAME_ID_LICENSE_URL:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_LICENSE_URL", -1));
				break;

			case TT_NAME_ID_PREFERRED_FAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_PREFERRED_FAMILY", -1));
				break;

			case TT_NAME_ID_PREFERRED_SUBFAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_PREFERRED_SUBFAMILY", -1));
				break;

			case TT_NAME_ID_MAC_FULL_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_MAC_FULL_NAME", -1));
				break;

			case TT_NAME_ID_SAMPLE_TEXT:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_SAMPLE_TEXT", -1));
				break;

			case TT_NAME_ID_CID_FINDFONT_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_CID_FINDFONT_NAME", -1));
				break;

			default:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("UNKNOWN", -1));
				break;
		}
		
		Tcl_ListObjAppendElement(interp, item,
				Tcl_NewStringObj("string", -1));
		
		Tcl_ListObjAppendElement(interp, item,
				Tcl_NewStringObj(finfo.string, finfo.string_len));

		Tcl_ListObjAppendElement(interp, res, item);
	}

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

	rle_data = Tcl_GetByteArrayFromObj(objv[1], &len);

	pmap = rle_decode(rle_data, len, &status);
	if (pmap == NULL)
		THROW_ERROR("Error decoding RLE: ", Tcl_GetString(Tcl_NewIntObj(status)));

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));
	
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

	// Primitives
	NEW_CMD("pixel::box", glue_box);
	NEW_CMD("pixel::line", glue_line);
	NEW_CMD("pixel::line_aa", glue_line_aa);
	NEW_CMD("pixel::line_aa_osa", glue_line_aa_osa);
	NEW_CMD("pixel::bezier", glue_bezier);

	// TrueType font rendering
	NEW_CMD("pixel::render_ttf", glue_render_ttf);
	NEW_CMD("pixel::render_ttf_adv", glue_render_ttf_adv);
	NEW_CMD("pixel::compile_face", glue_compile_face);
	NEW_CMD("pixel::ttf_info", glue_ttf_info);

	// Misc
	NEW_CMD("pixel::rle_encode", glue_rle_encode);
	NEW_CMD("pixel::rle_decode", glue_rle_decode);

	return TCL_OK;
}


