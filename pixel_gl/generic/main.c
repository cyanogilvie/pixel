// vim: ts=4 shiftwidth=4

#include "main.h"

static int _texture_unit_from_obj(interp, obj, unit) //{{{1
	Tcl_Interp*	interp;
	Tcl_Obj*	obj;
	GLenum*		unit;
{
	int			index;
	static CONST char* units[] = {
		"GL_TEXTURE0",
		"GL_TEXTURE1",
		"GL_TEXTURE2",
		"GL_TEXTURE3",
		"GL_TEXTURE4",
		"GL_TEXTURE5",
		"GL_TEXTURE6",
		"GL_TEXTURE7",
		"GL_TEXTURE8",
		"GL_TEXTURE9",
		"GL_TEXTURE10",
		"GL_TEXTURE11",
		"GL_TEXTURE12",
		"GL_TEXTURE13",
		"GL_TEXTURE14",
		"GL_TEXTURE15",
		"GL_TEXTURE16",
		"GL_TEXTURE17",
		"GL_TEXTURE18",
		"GL_TEXTURE19",
		"GL_TEXTURE20",
		"GL_TEXTURE21",
		"GL_TEXTURE22",
		"GL_TEXTURE23",
		"GL_TEXTURE24",
		"GL_TEXTURE25",
		"GL_TEXTURE26",
		"GL_TEXTURE27",
		"GL_TEXTURE28",
		"GL_TEXTURE29",
		"GL_TEXTURE30",
		"GL_TEXTURE31",
		(char *)NULL
	};
	GLenum unitmap[] = {
		GL_TEXTURE0,
		GL_TEXTURE1,
		GL_TEXTURE2,
		GL_TEXTURE3,
		GL_TEXTURE4,
		GL_TEXTURE5,
		GL_TEXTURE6,
		GL_TEXTURE7,
		GL_TEXTURE8,
		GL_TEXTURE9,
		GL_TEXTURE10,
		GL_TEXTURE11,
		GL_TEXTURE12,
		GL_TEXTURE13,
		GL_TEXTURE14,
		GL_TEXTURE15,
		GL_TEXTURE16,
		GL_TEXTURE17,
		GL_TEXTURE18,
		GL_TEXTURE19,
		GL_TEXTURE20,
		GL_TEXTURE21,
		GL_TEXTURE22,
		GL_TEXTURE23,
		GL_TEXTURE24,
		GL_TEXTURE25,
		GL_TEXTURE26,
		GL_TEXTURE27,
		GL_TEXTURE28,
		GL_TEXTURE29,
		GL_TEXTURE30,
		GL_TEXTURE31
	};

	TEST_OK(Tcl_GetIndexFromObj(interp, obj, units, "texture_unit", TCL_EXACT, &index));

	*unit = unitmap[index];
	return TCL_OK;
}

//}}}1
static const char* _render_mode_name(GLenum mode) //{{{1
{
	switch (mode) {
		case GL_RENDER:		return "GL_RENDER";
		case GL_SELECT:		return "GL_SELECT";
		case GL_FEEDBACK:	return "GL_FEEDBACK";
		default:			return "Invalid mode";
	}
}

static int glue_glShadeModel(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_FLAT",
		"GL_SMOOTH",
		(char *)NULL
	};
	GLenum map[] = {
		GL_FLAT,
		GL_SMOOTH
	};

	CHECK_ARGS(1, "type");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "type", TCL_EXACT,
				&index));

	glShadeModel(map[index]);

	return TCL_OK;
}


static int glue_glCullFace(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_FRONT",
		"GL_BACK",
		"GL_FRONT_AND_BACK",
		(char *)NULL
	};
	GLenum map[] = {
		GL_FRONT,
		GL_BACK,
		GL_FRONT_AND_BACK
	};

	CHECK_ARGS(1, "type");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "type", TCL_EXACT,
				&index));

	glCullFace(map[index]);

	return TCL_OK;
}


static int glue_glFrontFace(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_CW",
		"GL_CCW",
		(char *)NULL
	};
	GLenum map[] = {
		GL_CW,
		GL_CCW
	};

	CHECK_ARGS(1, "type");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "type", TCL_EXACT,
				&index));

	glFrontFace(map[index]);

	return TCL_OK;
}


static int glue_glEnable(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_ALPHA_TEST",
		"GL_POINT_SMOOTH",
		"GL_LINE_SMOOTH",
		"GL_POLYGON_SMOOTH",
		"GL_BLEND",
		"GL_COLOR_MATERIAL",
		"GL_CULL_FACE",
		"GL_DEPTH_TEST",
		"GL_DITHER",
		"GL_MAP1_VERTEX_3",
		"GL_MAP1_VERTEX_4",
		"GL_MAP1_INDEX",
		"GL_MAP1_COLOR_4",
		"GL_MAP1_NORMAL",
		"GL_MAP1_TEXTURE_COORD_1",
		"GL_MAP1_TEXTURE_COORD_2",
		"GL_MAP1_TEXTURE_COORD_3",
		"GL_MAP1_TEXTURE_COORD_4",
		"GL_MAP2_VERTEX_3",
		"GL_MAP2_VERTEX_4",
		"GL_AUTO_NORMAL",
		"GL_DEPTH_TEST",
		"GL_LIGHTING",
		"GL_LIGHT0",
		"GL_LIGHT1",
		"GL_LIGHT2",
		"GL_LIGHT3",
		"GL_LIGHT4",
		"GL_LIGHT5",
		"GL_LIGHT6",
		"GL_LIGHT7",
		"GL_LINE_STIPPLE",
		"GL_INDEX_LOGIC_OP",
		"GL_COLOR_LOGIC_OP",
		"GL_POLYGON_OFFSET_FILL",
		"GL_POLYGON_OFFSET_LINE",
		"GL_POLYGON_OFFSET_POINT",
		"GL_POLYGON_STIPPLE",
		"GL_RESCALE_NORMAL",
		"GL_NORMALIZE",
		"GL_STENCIL_TEST",
		"GL_TEXTURE_GEN_S",
		"GL_TEXTURE_GEN_T",
		"GL_TEXTURE_GEN_R",
		"GL_TEXTURE_GEN_Q",
		"GL_TEXTURE_1D",
		"GL_TEXTURE_2D",
		"GL_TEXTURE_3D",
		(char *)NULL
	};
	GLenum map[] = {
		GL_ALPHA_TEST,
		GL_POINT_SMOOTH,
		GL_LINE_SMOOTH,
		GL_POLYGON_SMOOTH,
		GL_BLEND,
		GL_COLOR_MATERIAL,
		GL_CULL_FACE,
		GL_DEPTH_TEST,
		GL_DITHER,
		GL_MAP1_VERTEX_3,
		GL_MAP1_VERTEX_4,
		GL_MAP1_INDEX,
		GL_MAP1_COLOR_4,
		GL_MAP1_NORMAL,
		GL_MAP1_TEXTURE_COORD_1,
		GL_MAP1_TEXTURE_COORD_2,
		GL_MAP1_TEXTURE_COORD_3,
		GL_MAP1_TEXTURE_COORD_4,
		GL_MAP2_VERTEX_3,
		GL_MAP2_VERTEX_4,
		GL_AUTO_NORMAL,
		GL_DEPTH_TEST,
		GL_LIGHTING,
		GL_LIGHT0,
		GL_LIGHT1,
		GL_LIGHT2,
		GL_LIGHT3,
		GL_LIGHT4,
		GL_LIGHT5,
		GL_LIGHT6,
		GL_LIGHT7,
		GL_LINE_STIPPLE,
		GL_INDEX_LOGIC_OP,
		GL_COLOR_LOGIC_OP,
		GL_POLYGON_OFFSET_FILL,
		GL_POLYGON_OFFSET_LINE,
		GL_POLYGON_OFFSET_POINT,
		GL_POLYGON_STIPPLE,
		GL_RESCALE_NORMAL,
		GL_NORMALIZE,
		GL_STENCIL_TEST,
		GL_TEXTURE_GEN_S,
		GL_TEXTURE_GEN_T,
		GL_TEXTURE_GEN_R,
		GL_TEXTURE_GEN_Q,
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D
	};

	CHECK_ARGS(1, "setting");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "setting", TCL_EXACT,
				&index));

	glEnable(map[index]);

	return TCL_OK;
}


static int glue_glDisable(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_ALPHA_TEST",
		"GL_POINT_SMOOTH",
		"GL_LINE_SMOOTH",
		"GL_POLYGON_SMOOTH",
		"GL_BLEND",
		"GL_COLOR_MATERIAL",
		"GL_CULL_FACE",
		"GL_DEPTH_TEST",
		"GL_DITHER",
		"GL_MAP1_VERTEX_3",
		"GL_MAP1_VERTEX_4",
		"GL_MAP1_INDEX",
		"GL_MAP1_COLOR_4",
		"GL_MAP1_NORMAL",
		"GL_MAP1_TEXTURE_COORD_1",
		"GL_MAP1_TEXTURE_COORD_2",
		"GL_MAP1_TEXTURE_COORD_3",
		"GL_MAP1_TEXTURE_COORD_4",
		"GL_MAP2_VERTEX_3",
		"GL_MAP2_VERTEX_4",
		"GL_AUTO_NORMAL",
		"GL_DEPTH_TEST",
		"GL_LIGHTING",
		"GL_LIGHT0",
		"GL_LIGHT1",
		"GL_LIGHT2",
		"GL_LIGHT3",
		"GL_LIGHT4",
		"GL_LIGHT5",
		"GL_LIGHT6",
		"GL_LIGHT7",
		"GL_LINE_STIPPLE",
		"GL_INDEX_LOGIC_OP",
		"GL_COLOR_LOGIC_OP",
		"GL_POLYGON_OFFSET_FILL",
		"GL_POLYGON_OFFSET_LINE",
		"GL_POLYGON_OFFSET_POINT",
		"GL_POLYGON_STIPPLE",
		"GL_RESCALE_NORMAL",
		"GL_NORMALIZE",
		"GL_STENCIL_TEST",
		"GL_TEXTURE_GEN_S",
		"GL_TEXTURE_GEN_T",
		"GL_TEXTURE_GEN_R",
		"GL_TEXTURE_GEN_Q",
		"GL_TEXTURE_1D",
		"GL_TEXTURE_2D",
		"GL_TEXTURE_3D",
		(char *)NULL
	};
	GLenum map[] = {
		GL_ALPHA_TEST,
		GL_POINT_SMOOTH,
		GL_LINE_SMOOTH,
		GL_POLYGON_SMOOTH,
		GL_BLEND,
		GL_COLOR_MATERIAL,
		GL_CULL_FACE,
		GL_DEPTH_TEST,
		GL_DITHER,
		GL_MAP1_VERTEX_3,
		GL_MAP1_VERTEX_4,
		GL_MAP1_INDEX,
		GL_MAP1_COLOR_4,
		GL_MAP1_NORMAL,
		GL_MAP1_TEXTURE_COORD_1,
		GL_MAP1_TEXTURE_COORD_2,
		GL_MAP1_TEXTURE_COORD_3,
		GL_MAP1_TEXTURE_COORD_4,
		GL_MAP2_VERTEX_3,
		GL_MAP2_VERTEX_4,
		GL_AUTO_NORMAL,
		GL_DEPTH_TEST,
		GL_LIGHTING,
		GL_LIGHT0,
		GL_LIGHT1,
		GL_LIGHT2,
		GL_LIGHT3,
		GL_LIGHT4,
		GL_LIGHT5,
		GL_LIGHT6,
		GL_LIGHT7,
		GL_LINE_STIPPLE,
		GL_INDEX_LOGIC_OP,
		GL_COLOR_LOGIC_OP,
		GL_POLYGON_OFFSET_FILL,
		GL_POLYGON_OFFSET_LINE,
		GL_POLYGON_OFFSET_POINT,
		GL_POLYGON_STIPPLE,
		GL_RESCALE_NORMAL,
		GL_NORMALIZE,
		GL_STENCIL_TEST,
		GL_TEXTURE_GEN_S,
		GL_TEXTURE_GEN_T,
		GL_TEXTURE_GEN_R,
		GL_TEXTURE_GEN_Q,
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D
	};

	CHECK_ARGS(1, "setting");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "setting", TCL_EXACT,
				&index));

	glDisable(map[index]);

	return TCL_OK;
}


static int glue_glClearColor(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	r, g, b, a;

	CHECK_ARGS(4, "r g b a");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &r));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &g));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &b));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &a));

	glClearColor(r, g, b, a);

	return TCL_OK;
}


static int glue_glViewport(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		x, y, w, h;

	CHECK_ARGS(4, "x y w h");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &h));

	glViewport(x, y, w, h);

	return TCL_OK;
}


static int glue_glGenList(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint		listid;

	CHECK_ARGS(0, "");

	listid = glGenLists(1);

	Tcl_SetObjResult(interp, Tcl_NewIntObj(listid));

	return TCL_OK;
}


static int glue_glNewList(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint		listid;

	CHECK_ARGS(1, "listid");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int *)&listid));

	glNewList(listid, GL_COMPILE);

	return TCL_OK;
}


static int glue_glEndList(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glEndList();

	return TCL_OK;
}


static int glue_glCallList(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint		listid;

	CHECK_ARGS(1, "listid");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int *)&listid));

	glCallList(listid);

	return TCL_OK;
}


static int glue_glBegin(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_POINTS",
		"GL_LINES",
		"GL_LINE_STRIP",
		"GL_LINE_LOOP",
		"GL_TRIANGLES",
		"GL_TRIANGLE_STRIP",
		"GL_TRIANGLE_FAN",
		"GL_QUADS",
		"GL_QUAD_STRIP",
		"GL_POLYGON",
		(char *)NULL
	};
	GLenum map[] = {
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_LINE_LOOP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_QUADS,
		GL_QUAD_STRIP,
		GL_POLYGON
	};

	CHECK_ARGS(1, "type");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "type", TCL_EXACT,
				&index));

	glBegin(map[index]);

	return TCL_OK;
}


static int glue_glEnd(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glEnd();

	return TCL_OK;
}


static int glue_glColor3f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	r, g, b;

	CHECK_ARGS(3, "r g b");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &r));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &g));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &b));

	glColor3f(r, g, b);

	return TCL_OK;
}


static int glue_glColor4f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	r, g, b, a;

	CHECK_ARGS(4, "r g b a");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &r));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &g));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &b));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &a));

	glColor4f(r, g, b, a);

	return TCL_OK;
}


static int glue_glPointSize(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	size;

	CHECK_ARGS(1, "size");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &size));

	glPointSize(size);

	return TCL_OK;
}


static int glue_glVertex3f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	x, y, z;

	CHECK_ARGS(3, "x y z");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &z));

	glVertex3f(x, y, z);

	return TCL_OK;
}


static int glue_glNormal3f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	x, y, z;

	CHECK_ARGS(3, "x y z");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &z));

	glNormal3f(x, y, z);

	return TCL_OK;
}


static int glue_glClear(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLbitfield		mask;
	static CONST char *mask_bits[] = {
		"GL_COLOR_BUFFER_BIT",
		"GL_DEPTH_BUFFER_BIT",
		"GL_ACCUM_BUFFER_BIT",
		"GL_STENCIL_BUFFER_BIT",
		(char *)NULL
	};
	int map[] = {
		GL_COLOR_BUFFER_BIT,
		GL_DEPTH_BUFFER_BIT,
		GL_ACCUM_BUFFER_BIT,
		GL_STENCIL_BUFFER_BIT
	};
	int		i, index;

	if (objc == 1)
		THROW_ERROR("No buffers specified");

	mask = 0;
	for (i=1; i<objc; i++) {
		TEST_OK(Tcl_GetIndexFromObj(interp, objv[i], mask_bits, "buffer", 
					TCL_EXACT, &index));
		mask |= map[index];
	}

	glClear(mask);

	return TCL_OK;
}


static int glue_glMatrixMode(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_MODELVIEW",
		"GL_PROJECTION",
		"GL_TEXTURE",
		(char *)NULL
	};
	GLenum map[] = {
		GL_MODELVIEW,
		GL_PROJECTION,
		GL_TEXTURE
	};

	CHECK_ARGS(1, "matrix");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "matrix", TCL_EXACT,
				&index));

	glMatrixMode(map[index]);

	return TCL_OK;
}


static int glue_glLoadIdentity(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glLoadIdentity();

	return TCL_OK;
}


static int glue_glTranslatef(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	x, y, z;

	CHECK_ARGS(3, "x y z");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &z));

	glTranslatef(x, y, z);

	return TCL_OK;
}


static int glue_glRotatef(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	angle, x, y, z;

	CHECK_ARGS(4, "angle x y z");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &angle));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &x));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &y));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &z));

	glRotatef(angle, x, y, z);

	return TCL_OK;
}


static int glue_gluPerspective(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double	fovy, aspect, near, far;

	CHECK_ARGS(4, "fovy aspect near far");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &fovy));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &aspect));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &near));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &far));

	gluPerspective(fovy, aspect, near, far);

	return TCL_OK;
}


static int glue_glGenTextures(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	*names;
	GLsizei	count;
	Tcl_Obj	*res;
	int		i;

	CHECK_ARGS(1, "count");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &count));

	names = (GLuint *)ckalloc(sizeof(GLuint) * count);

	glGenTextures(count, names);

	res = Tcl_NewListObj(0, NULL);
	for (i=0; i<count; i++) {
		if (Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(names[i])) != TCL_OK) {
			ckfree((char *)names);
			return TCL_ERROR;
		}
	}

	ckfree((char *)names);

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_glBindTexture(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	name;
	int		index;
	static CONST char *types[] = {
		"GL_TEXTURE_1D",
		"GL_TEXTURE_2D",
		"GL_TEXTURE_3D",
		(char *)NULL
	};
	GLenum map[] = {
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D
	};

	CHECK_ARGS(2, "target name");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "target", TCL_EXACT,
				&index));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], (int *)&name));

	glBindTexture(map[index], name);

	return TCL_OK;
}


static int glue_glTexParameter(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLenum	target;
	GLenum	name;
	GLuint	param;

	int		index;
	static CONST char *targets[] = {
		"GL_TEXTURE_1D",
		"GL_TEXTURE_2D",
		"GL_TEXTURE_3D",
		(char *)NULL
	};
	GLenum target_map[] = {
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D
	};
	static CONST char *names[] = {
		"GL_TEXTURE_MAG_FILTER",
		"GL_TEXTURE_MIN_FILTER",
		"GL_TEXTURE_WRAP_S",
		"GL_TEXTURE_WRAP_T",
		"GL_TEXTURE_WRAP_R",
		"GL_TEXTURE_BORDER_COLOR",
		"GL_TEXTURE_PRIORITY",
		"GL_TEXTURE_MIN_LOD",
		"GL_TEXTURE_MAX_LOD",
		"GL_TEXTURE_BASE_LEVEL",
		"GL_TEXTURE_MAX_LEVEL",
		(char *)NULL
	};
	GLenum name_map[] = {
		GL_TEXTURE_MAG_FILTER,
		GL_TEXTURE_MIN_FILTER,
		GL_TEXTURE_WRAP_S,
		GL_TEXTURE_WRAP_T,
		GL_TEXTURE_WRAP_R,
		GL_TEXTURE_BORDER_COLOR,
		GL_TEXTURE_PRIORITY,
		GL_TEXTURE_MIN_LOD,
		GL_TEXTURE_MAX_LOD,
		GL_TEXTURE_BASE_LEVEL,
		GL_TEXTURE_MAX_LEVEL
	};

	CHECK_ARGS(3, "target param value");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], targets, "target", TCL_EXACT,
				&index));
	target = target_map[index];

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[2], names, "param name", TCL_EXACT,
				&index));
	name = name_map[index];

	switch (name) {
		case GL_TEXTURE_WRAP_S:
		case GL_TEXTURE_WRAP_T:
		case GL_TEXTURE_WRAP_R: //{{{
			{
				static CONST char *values[] = {
					"GL_CLAMP",
					"GL_CLAMP_TO_EDGE",
					"GL_REPEAT",
					(char *)NULL
				};
				GLenum value_map[] = {
					GL_CLAMP,
					GL_CLAMP_TO_EDGE,
					GL_REPEAT
				};

				TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], values, "value",
							TCL_EXACT, &index));
				param = value_map[index];

				glTexParameteri(target, name, param);
				return TCL_OK;
			}
			break; //}}}

		case GL_TEXTURE_MAG_FILTER: //{{{
			{
				static CONST char *values[] = {
					"GL_NEAREST",
					"GL_LINEAR",
					(char *)NULL
				};
				GLenum value_map[] = {
					GL_NEAREST,
					GL_LINEAR
				};

				TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], values, "value",
							TCL_EXACT, &index));
				param = value_map[index];

				glTexParameteri(target, name, param);
				return TCL_OK;
			}
			break; //}}}

		case GL_TEXTURE_MIN_FILTER: //{{{
			{
				static CONST char *values[] = {
					"GL_NEAREST",
					"GL_LINEAR",
					"GL_NEAREST_MIPMAP_NEAREST",
					"GL_NEAREST_MIPMAP_LINEAR",
					"GL_LINEAR_MIPMAP_NEAREST",
					"GL_LINEAR_MIPMAP_LINEAR",
					(char *)NULL
				};
				GLenum value_map[] = {
					GL_NEAREST,
					GL_LINEAR,
					GL_NEAREST_MIPMAP_NEAREST,
					GL_NEAREST_MIPMAP_LINEAR,
					GL_LINEAR_MIPMAP_NEAREST,
					GL_LINEAR_MIPMAP_LINEAR
				};

				TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], values, "value",
							TCL_EXACT, &index));
				param = value_map[index];

				glTexParameteri(target, name, param);
				return TCL_OK;
			}
			break; //}}}

		case GL_TEXTURE_BORDER_COLOR: //{{{
			{
				Tcl_Obj		**v;
				int			count, i;
				double		hold;
				GLfloat	components[4];

				TEST_OK(Tcl_ListObjGetElements(interp, objv[3], &count, &v));
				if (count != 4)
					THROW_ERROR("Expecting a list of 4 elements");

				for (i=0; i<4; i++) {
					TEST_OK(Tcl_GetDoubleFromObj(interp, v[0], &hold));
					components[0] = hold;
				}

				glTexParameterfv(target, name, components);
				return TCL_OK;
			}
			break; //}}}

		case GL_TEXTURE_PRIORITY: //{{{
			{
				GLfloat		priority;
				double		value;

				TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &value));
				priority = value;

				glTexParameterf(target, name, priority);
				return TCL_OK;
			}
			break; //}}}

		case GL_TEXTURE_MIN_LOD:
		case GL_TEXTURE_MAX_LOD: //{{{
			{
				GLfloat		level;
				double		value;

				TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &value));
				level = value;

				glTexParameterf(target, name, level);
				return TCL_OK;
			}
			break; //}}}

		case GL_TEXTURE_BASE_LEVEL:
		case GL_TEXTURE_MAX_LEVEL: //{{{
			{
				GLuint		level;
				int			value;

				TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &value));
				if (level < 0)
					THROW_ERROR("level must be a positive integer");

				level = value;

				glTexParameteri(target, name, level);
				return TCL_OK;
			}
			break; //}}}

		default:
			THROW_ERROR("Unexpected name");
			break;
	}

	THROW_ERROR("Should not get here");
}


static int glue_glTexEnv(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLenum	target;
	GLenum	name;
	GLuint	param;

	int		index;
	static CONST char *targets[] = {
		"GL_TEXTURE_ENV",
		(char *)NULL
	};
	GLenum target_map[] = {
		GL_TEXTURE_ENV
	};
	static CONST char *names[] = {
		"GL_TEXTURE_ENV_MODE",

		"GL_TEXTURE_MIN_FILTER",
		"GL_TEXTURE_WRAP_S",
		"GL_TEXTURE_WRAP_T",
		"GL_TEXTURE_WRAP_R",
		"GL_TEXTURE_BORDER_COLOR",
		"GL_TEXTURE_PRIORITY",
		"GL_TEXTURE_MIN_LOD",
		"GL_TEXTURE_MAX_LOD",
		"GL_TEXTURE_BASE_LEVEL",
		"GL_TEXTURE_MAX_LEVEL",
		(char *)NULL
	};
	GLenum name_map[] = {
		GL_TEXTURE_ENV_MODE,

		GL_TEXTURE_MIN_FILTER,
		GL_TEXTURE_WRAP_S,
		GL_TEXTURE_WRAP_T,
		GL_TEXTURE_WRAP_R,
		GL_TEXTURE_BORDER_COLOR,
		GL_TEXTURE_PRIORITY,
		GL_TEXTURE_MIN_LOD,
		GL_TEXTURE_MAX_LOD,
		GL_TEXTURE_BASE_LEVEL,
		GL_TEXTURE_MAX_LEVEL
	};

	CHECK_ARGS(3, "target param value");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], targets, "target", TCL_EXACT,
				&index));
	target = target_map[index];

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[2], names, "param name", TCL_EXACT,
				&index));
	name = name_map[index];

	switch (name) {
		case GL_TEXTURE_ENV_MODE: //{{{
			{
				static CONST char *values[] = {
					"GL_DECAL",
					"GL_REPLACE",
					"GL_MODULATE",
					"GL_BLEND",
					(char *)NULL
				};
				GLenum value_map[] = {
					GL_DECAL,
					GL_REPLACE,
					GL_MODULATE,
					GL_BLEND
				};

				TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], values, "value",
							TCL_EXACT, &index));
				param = value_map[index];

				glTexEnvi(target, name, param);
				return TCL_OK;
			}
			break; //}}}

		case GL_TEXTURE_ENV_COLOR: //{{{
			{
				Tcl_Obj		**v;
				int			count, i;
				double		hold;
				GLfloat	components[4];

				TEST_OK(Tcl_ListObjGetElements(interp, objv[3], &count, &v));
				if (count != 4)
					THROW_ERROR("Expecting a list of 4 elements");

				for (i=0; i<4; i++) {
					TEST_OK(Tcl_GetDoubleFromObj(interp, v[0], &hold));
					components[0] = hold;
				}

				glTexEnvfv(target, name, components);
				return TCL_OK;
			}
			break; //}}}

		default:
			THROW_ERROR("Unexpected name");
			break;
	}

	THROW_ERROR("Should not get here");
}


static int glue_glTexImage2D(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *targets[] = {
		"GL_TEXTURE_1D",
		"GL_TEXTURE_2D",
		"GL_TEXTURE_3D",
		(char *)NULL
	};
	GLenum target_map[] = {
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D
	};
	GLint			level;
	GLuint			border;
	gimp_image_t	*pmap;

	CHECK_ARGS(4, "target level border pmap");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], targets, "target", TCL_EXACT,
				&index));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &level));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], (int *)&border));
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[4], &pmap));
	glPixelStorei(GL_UNPACK_ROW_LENGTH, pmap->width);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(
			target_map[index],
			level,
			GL_RGBA8,
			pmap->width,
			pmap->height,
			border,
			//GL_RGBA,
			GL_BGRA,
			GL_UNSIGNED_INT_8_8_8_8_REV,
			pmap->pixel_data
	);

	return TCL_OK;
}


static int glue_glTexSubImage2D(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	GLint	x, y;
	GLsizei	width, height;
	int		tmp;
	static CONST char *targets[] = {
		"GL_TEXTURE_1D",
		"GL_TEXTURE_2D",
		"GL_TEXTURE_3D",
		(char *)NULL
	};
	GLenum target_map[] = {
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D
	};
	GLint			level;
	gimp_image_t	*pmap;

	CHECK_ARGS(7, "target level x y width height pmap");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], targets, "target", TCL_EXACT,
				&index));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &level));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &tmp));	x = tmp;
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &tmp));	y = tmp;
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &tmp));	width = tmp;
	TEST_OK(Tcl_GetIntFromObj(interp, objv[6], &tmp));	height = tmp;
	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[7], &pmap));
	glPixelStorei(GL_UNPACK_ROW_LENGTH, pmap->width);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexSubImage2D(
			target_map[index],
			level,
			x, y,
			width, height,
			GL_BGRA,
			GL_UNSIGNED_INT_8_8_8_8_REV,
			pmap->pixel_data
	);

	return TCL_OK;
}


static int glue_glTexImage2D_luminance(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *targets[] = {
		"GL_TEXTURE_1D",
		"GL_TEXTURE_2D",
		"GL_TEXTURE_3D",
		(char *)NULL
	};
	GLenum target_map[] = {
		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D
	};
	GLint			level;
	GLuint			border;
	const unsigned char*	luminance;
	int						luminance_len;
	int						width, height, stride;

	CHECK_ARGS(7, "target level border luminance_data width height stride");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], targets, "target", TCL_EXACT,
				&index));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &level));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], (int *)&border));
	luminance = Tcl_GetByteArrayFromObj(objv[4], &luminance_len);
	TEST_OK(Tcl_GetIntFromObj(interp, objv[5], (int *)&width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[6], (int *)&height));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[7], (int *)&stride));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);

	glTexImage2D(
			target_map[index],
			level,
			GL_RGBA8,
			width,
			height,
			border,
			GL_ALPHA,
			GL_UNSIGNED_BYTE,
			luminance
	);

	return TCL_OK;
}


static int glue_glTexCoord2f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double		s, t;

	CHECK_ARGS(2, "s t");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &s));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &t));

	glTexCoord2f(s, t);

	return TCL_OK;
}


static int glue_glMultiTexCoord2f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double		s, t;
	GLenum		texture_unit;

	CHECK_ARGS(3, "texture_unit s t");

	TEST_OK(_texture_unit_from_obj(interp, objv[1], &texture_unit));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &s));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &t));

	glMultiTexCoord2f(texture_unit, s, t);

	return TCL_OK;
}


static int glue_glTexCoord3f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	double		s, t, r;

	CHECK_ARGS(3, "s t r");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &s));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &t));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &r));

	glTexCoord3f(s, t, r);

	return TCL_OK;
}


static int glue_glPushMatrix(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glPushMatrix();

	return TCL_OK;
}


static int glue_glPopMatrix(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glPopMatrix();

	return TCL_OK;
}


static int glue_glLoadMatrixf(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	float*		m[16];

	CHECK_ARGS(1, "glMatrix");

	TEST_OK(Tcl_GetGlMatrixFromObj(interp, objv[1], (float **)&m));

	glLoadMatrixf((float *)m);

	return TCL_OK;
}


static int glue_glMultMatrixf(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	float*		m[16];

	CHECK_ARGS(1, "glMatrix");

	TEST_OK(Tcl_GetGlMatrixFromObj(interp, objv[1], (float **)&m));

	glMultMatrixf((float *)m);

	return TCL_OK;
}


static int glue_glFlush(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glFlush();

	return TCL_OK;
}


static int glue_glFinish(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glFinish();

	return TCL_OK;
}


static int glue_glGetString(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *tokens[] = {
		"GL_VENDOR",
		"GL_RENDERER",
		"GL_VERSION",
		"GL_SHADING_LANGUAGE_VERSION",
		(char *)NULL
	};
	GLenum map[] = {
		GL_VENDOR,
		GL_RENDERER,
		GL_VERSION,
		GL_SHADING_LANGUAGE_VERSION
	};

	CHECK_ARGS(1, "token");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], tokens, "token", TCL_EXACT,
				&index));

	Tcl_SetObjResult(interp, Tcl_NewStringObj((const char*)glGetString(map[index]), -1));

	return TCL_OK;
}


static int glue_glCreateProgram(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	p;

	CHECK_ARGS(0, "");

	p = glCreateProgram();
	if (p == 0) {
		Tcl_SetErrorCode(interp, "GL", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Cannot create shader program", -1));
		return TCL_ERROR;
	}
	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp = p;
		Tcl_SetObjResult(interp, Tcl_NewWideIntObj(tmp));
	} else {
		Tcl_SetObjResult(interp, Tcl_NewIntObj((int)p));
	}

	return TCL_OK;
}


static int glue_glDeleteProgram(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	p;

	CHECK_ARGS(1, "programhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;
		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		p = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&p));
	}

	glDeleteProgram(p);

	// TODO: handle errors GL_INVALID_VALUE and GL_INVALID_OPERATION

	return TCL_OK;
}


static int glue_glCreateShader(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *types[] = {
		"GL_VERTEX_SHADER",
		"GL_FRAGMENT_SHADER",
		(char *)NULL
	};
	GLenum map[] = {
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER
	};
	GLuint	shaderhandle;

	CHECK_ARGS(1, "type");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], types, "type", TCL_EXACT,
				&index));

	shaderhandle = glCreateShader(map[index]);
	if (shaderhandle == 0) {
		Tcl_SetErrorCode(interp, "GL", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Cannot create shader", -1));
		return TCL_ERROR;
	}
	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp = shaderhandle;
		Tcl_SetObjResult(interp, Tcl_NewWideIntObj(tmp));
	} else {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(shaderhandle));
	}

	return TCL_OK;
}


static int glue_glDeleteShader(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	shaderhandle;

	CHECK_ARGS(1, "shaderhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		shaderhandle = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&shaderhandle));
	}

	glDeleteShader(shaderhandle);
	// TODO: Handle errors GL_INVALID_VALUE and GL_INVALID_OPERATION

	return TCL_OK;
}


static int glue_glShaderSource(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint			shaderhandle;
	const GLchar*	sources[objc-2];
	const GLint		lengths[objc-2];
	int				i, count;

	count = objc - 2;
	if (count < 1) {
		// For some reason glShaderSource takes an array of sources
		CHECK_ARGS(2, "shaderhandle shadersource ?shadersource ...?");
	}

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		shaderhandle = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&shaderhandle));
	}

	for (i=0; i<count; i++) {
		sources[i] = Tcl_GetStringFromObj(objv[i+2], (int*)&lengths[i]);
	}

	glShaderSource(shaderhandle, count, sources, lengths);
	// TODO: handle errors GL_INVALID_VALUE GL_INVALID_OPERATION

	return TCL_OK;
}


static int glue_glCompileShader(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	shaderhandle;
	GLint	compileok;

	CHECK_ARGS(1, "shaderhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		shaderhandle = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&shaderhandle));
	}

	glCompileShader(shaderhandle);
	// TODO: Handle errors GL_INVALID_VALUE and GL_INVALID_OPERATION
	glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compileok);
	if (compileok != GL_TRUE) {
		GLint	loglength;
		GLchar*	logdata;
		GLsizei	gotlength;

		glGetShaderiv(shaderhandle, GL_INFO_LOG_LENGTH, &loglength);
		if (loglength == 0)
			THROW_ERROR("Unspecified error compiling shader");

		logdata = (GLchar*)ckalloc(sizeof(GLchar) * loglength);
		glGetShaderInfoLog(shaderhandle, loglength, &gotlength, logdata);
		Tcl_SetErrorCode(interp, "GL", "COMPILE", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj(logdata, gotlength));
		ckfree((char*)logdata);
		return TCL_ERROR;
	}

	return TCL_OK;
}


static int glue_glAttachShader(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	shaderhandle;
	GLuint	p;

	CHECK_ARGS(2, "programhandle shaderhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		p = tmp;
		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[2], &tmp));
		shaderhandle = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&p));
		TEST_OK(Tcl_GetIntFromObj(interp, objv[2], (int*)&shaderhandle));
	}

	glAttachShader(p, shaderhandle);
	// TODO: Handle errors GL_INVALID_VALUE and GL_INVALID_OPERATION

	return TCL_OK;
}


static int glue_glLinkProgram(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	p;

	CHECK_ARGS(1, "programhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		p = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&p));
	}

	glLinkProgram(p);
	// TODO: Handle errors GL_INVALID_VALUE and GL_INVALID_OPERATION

	return TCL_OK;
}


static int glue_glValidateProgram(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	p;
	GLint	valid;

	CHECK_ARGS(1, "programhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		p = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&p));
	}

	glValidateProgram(p);
	glGetProgramiv(p, GL_VALIDATE_STATUS, &valid);
	if (valid == GL_TRUE) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(1));
	} else {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(0));
	}

	return TCL_OK;
}


static int glue_glGetProgramInfoLog(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	p;
	GLint	loglength;
	GLsizei	gotlength;
	GLchar*	logdata;

	CHECK_ARGS(1, "programhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		p = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&p));
	}

	glGetProgramiv(p, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength == 0) return TCL_OK;

	logdata = ckalloc(sizeof(GLchar) * loglength);
	glGetProgramInfoLog(p, loglength, &gotlength, logdata);

	Tcl_SetObjResult(interp, Tcl_NewStringObj(logdata, gotlength));

	ckfree((char*)logdata);

	return TCL_OK;
}


static int glue_glUseProgram(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	p;

	CHECK_ARGS(1, "programhandle");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		p = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&p));
	}

	glUseProgram(p);
	// TODO: Handle errors GL_INVALID_VALUE and GL_INVALID_OPERATION

	return TCL_OK;
}


static int glue_glGetUniformLocation(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint	p;
	GLint	loc;

	CHECK_ARGS(2, "programhandle varname");

	if (sizeof(GLuint) > sizeof(int)) {
		Tcl_WideInt	tmp;

		TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &tmp));
		p = tmp;
	} else {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int*)&p));
	}

	loc = glGetUniformLocation(p, Tcl_GetString(objv[2]));
	// TODO: Handle errors GL_INVALID_VALUE and GL_INVALID_OPERATION
	if (loc == -1)
		THROW_ERROR("Couldn't get uniform location for \"", Tcl_GetString(objv[2]), "\"");
	if (sizeof(GLint) > sizeof(int)) {
		Tcl_WideInt	tmp = loc;
		Tcl_SetObjResult(interp, Tcl_NewWideIntObj(tmp));
	} else {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(loc));
	}

	return TCL_OK;
}


static int glue_glUniform1f(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLint	loc;
	GLfloat	val;
	double	tmp;

	CHECK_ARGS(2, "uniformloc floatval");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &loc));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &tmp));
	val = tmp;

	glUniform1f(loc, val);

	return TCL_OK;
}


static int glue_glUniform1i(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLint	loc;
	GLint	val;
	int		tmp;

	CHECK_ARGS(2, "uniformloc intval");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &loc));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &tmp));
	val = tmp;

	glUniform1i(loc, val);

	return TCL_OK;
}


static int glue_glOrtho(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLdouble	left, right, bottom, top, near, far;

	CHECK_ARGS(6, "left right bottom top near far");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &left));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &right));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &bottom));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &top));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[5], &near));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &far));

	glOrtho(left, right, bottom, top, near, far);

	return TCL_OK;
}


static int glue_gluOrtho2D(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLdouble	left, right, bottom, top;

	CHECK_ARGS(4, "left right bottom top");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &left));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &right));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &bottom));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &top));

	gluOrtho2D(left, right, bottom, top);

	return TCL_OK;
}


static int glue_glVertexList(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLint		size;
	GLenum		type;
	GLsizei		stride;
	Tcl_Obj**	ov;
	int			oc;
	//int			i;

	int			index;
	static CONST char* types[] = {
		"GL_SHORT",
		"GL_INT",
		"GL_FLOAT",
		"GL_DOUBLE",
		(char *)NULL
	};
	GLenum typemap[] = {
		GL_SHORT,
		GL_INT,
		GL_FLOAT,
		GL_DOUBLE
	};

	CHECK_ARGS(4, "size type stride vertex_list");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &size));
	TEST_OK(Tcl_GetIndexFromObj(interp, objv[2], types, "type", TCL_EXACT, &index));
	type = typemap[index];
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &stride));
	TEST_OK(Tcl_ListObjGetElements(interp, objv[4], &oc, &ov));


	// TODO: Finish

	return TCL_OK;
}


static int glue_glActiveTexture(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLenum		texture_unit;

	CHECK_ARGS(1, "texture_unit");

	TEST_OK(_texture_unit_from_obj(interp, objv[1], &texture_unit));
	glActiveTexture(texture_unit);

	return TCL_OK;
}


static int glue_glBlendFunc(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *funcs[] = {
		"GL_ZERO",
		"GL_ONE",
		"GL_SRC_COLOR",
		"GL_ONE_MINUS_SRC_COLOR",
		"GL_DST_COLOR",
		"GL_ONE_MINUS_DST_COLOR",
		"GL_SRC_ALPHA",
		"GL_ONE_MINUS_SRC_ALPHA",
		"GL_DST_ALPHA",
		"GL_ONE_MINUS_DST_ALPHA",
		"GL_CONSTANT_COLOR",
		"GL_ONE_MINUS_CONSTANT_COLOR",
		"GL_CONSTANT_ALPHA",
		"GL_ONE_MINUS_CONSTANT_ALPHA",
		"GL_SRC_ALPHA_SATURATE",
		(char *)NULL
	};
	GLenum func_map[] = {
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_CONSTANT_COLOR,
		GL_ONE_MINUS_CONSTANT_COLOR,
		GL_CONSTANT_ALPHA,
		GL_ONE_MINUS_CONSTANT_ALPHA,
		GL_SRC_ALPHA_SATURATE
	};
	GLenum		srcfactor, dstfactor;

	CHECK_ARGS(2, "src dst");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], funcs, "func", TCL_EXACT,
				&index));
	srcfactor = func_map[index];
	TEST_OK(Tcl_GetIndexFromObj(interp, objv[2], funcs, "func", TCL_EXACT,
				&index));
	dstfactor = func_map[index];

	glBlendFunc(srcfactor, dstfactor);

	return TCL_OK;
}


static int glue_glBlendEquation(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		index;
	static CONST char *funcs[] = {
		"GL_FUNC_ADD",
		"GL_FUNC_SUBTRACT",
		"GL_FUNC_REVERSE_SUBTRACT",
		"GL_MIN",
		"GL_MAX",
		"GL_LOGIC_OP",
		(char *)NULL
	};
	GLenum func_map[] = {
		GL_FUNC_ADD,
		GL_FUNC_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_MIN,
		GL_MAX,
		GL_LOGIC_OP
	};
	GLenum		func;

	CHECK_ARGS(1, "func");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], funcs, "func", TCL_EXACT,
				&index));
	func = func_map[index];

	glBlendEquation(func);

	return TCL_OK;
}


static int glue_glRenderMode(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	Tcl_Obj*	buf = NULL;
	Tcl_Obj*	buftypeobj;
	int			buftype;
	int			index;
	static CONST char *modes[] = {
		"GL_RENDER",
		"GL_SELECT",
		"GL_FEEDBACK",
		(char *)NULL
	};
	GLenum mode_map[] = {
		GL_RENDER,
		GL_SELECT,
		GL_FEEDBACK
	};
	GLenum		mode;

	if (objc < 2 || objc > 3) {
		CHECK_ARGS(2, "mode ?buf?");
	}

	if (Tcl_GetIndexFromObj(interp, objv[1], modes, "mode", TCL_EXACT,
				&index) != TCL_OK)
		goto failed;

	mode = mode_map[index];

	if (objc >= 3) {
		GLint	render_mode;

		buf = objv[2];

		if ((Tcl_DictObjGet(interp, buf, Tcl_NewStringObj("buftype", -1), &buftypeobj)) != TCL_OK)
			goto failed;

		if (buftypeobj == NULL) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj("No buffer type", -1));
			goto failed;
		}

		if (Tcl_GetIntFromObj(interp, buftypeobj, &buftype) != TCL_OK)
			goto failed;

		glGetIntegerv(GL_RENDER_MODE, &render_mode);

		if (render_mode != buftype) {
			Tcl_SetObjResult(interp, Tcl_ObjPrintf("Supplied buffer doesn't match current render mode: current: %s, buffer: %s",
						_render_mode_name(render_mode),
						_render_mode_name(buftype)));
			goto failed;
		}

		switch (render_mode) {
			case GL_SELECT:
				{
					GLuint*		selectbuf;
					int			buflen, arraysize, i, j;
					double		z1, z2;
					GLuint		names;
					GLuint		*ptr;
					Tcl_Obj*	arraysizeobj;
					Tcl_Obj*	selectbufobj;
					GLint		hits;
					Tcl_Obj**	ov;
					Tcl_Obj*	nameslist;

					if (Tcl_DictObjGet(interp, buf, Tcl_NewStringObj("arraysize", -1), &arraysizeobj) != TCL_OK)
						goto failed;

					if (arraysizeobj == NULL) {
						Tcl_SetObjResult(interp, Tcl_NewStringObj("No arraysize element", -1));
						goto failed;
					}

					if (Tcl_GetIntFromObj(interp, arraysizeobj, &arraysize) != TCL_OK)
						goto failed;


					if (Tcl_DictObjGet(interp, buf, Tcl_NewStringObj("buf", -1),
								&selectbufobj) != TCL_OK)
						goto failed;
					if (selectbufobj == NULL) {
						Tcl_SetObjResult(interp,
								Tcl_NewStringObj("No buf entry", -1));
						goto failed;
					}

					selectbuf = (GLuint*)Tcl_GetByteArrayFromObj(
							selectbufobj, &buflen);

					if (buflen < sizeof(GLuint) * arraysize) {
						Tcl_SetObjResult(interp, Tcl_ObjPrintf("Buffer size mismatch, expected array of %d GLuints (%d bytes each) == %d bytes, got %d bytes",
									arraysize, sizeof(GLuint),
									arraysize * sizeof(GLuint), buflen));
						goto failed;
					}

					hits = glRenderMode(mode);

					if (hits > arraysize) {
						Tcl_SetObjResult(interp, Tcl_ObjPrintf("glRenderMode returned more hits than the buffer can hold: %d hits, array size: %d",
									hits, arraysize));
						goto failed;
					}

					ov = (Tcl_Obj**)ckalloc(sizeof(Tcl_Obj*) * hits);

					ptr = selectbuf;
					for (i=0; i<hits; i++) { // For each hit
						ov[i] = Tcl_NewListObj(0, NULL);
						names = *ptr++;
						z1 = (double)*ptr++ / 0x7fffffff;
						z2 = (double)*ptr++ / 0x7fffffff;

						if (Tcl_ListObjAppendElement(interp, ov[i],
									Tcl_NewDoubleObj(z1)) != TCL_OK)
							goto failed;
						if (Tcl_ListObjAppendElement(interp, ov[i],
									Tcl_NewDoubleObj(z2)) != TCL_OK)
							goto failed;
						nameslist = Tcl_NewListObj(0, NULL);
						if (Tcl_ListObjAppendElement(interp, ov[i],
									nameslist) != TCL_OK)
							goto failed;

						if (names > arraysize - i) {
							Tcl_SetObjResult(interp, Tcl_ObjPrintf("Too many names for remaining array entries: %d, remaining slots: %d",
										names, arraysize - (ptr - selectbuf)));
							goto failed;
						}
						for (j=0; j<names; j++) {
							if (Tcl_ListObjAppendElement(interp, nameslist,
										Tcl_NewIntObj(*ptr++)) != TCL_OK)
								goto failed;
						}
					}
					Tcl_SetObjResult(interp, Tcl_NewListObj(hits, ov));

					ckfree((char*)ov);
				}
				break;

			case GL_FEEDBACK:
				{
					glRenderMode(mode);
					Tcl_SetObjResult(interp, Tcl_NewStringObj("Feedback mode not implemented yet", -1));
					goto failed;
				}
				break;

			case GL_RENDER:
				glRenderMode(mode);
				break;

			default:
				Tcl_SetObjResult(interp, Tcl_ObjPrintf("Invalid render mode: %d", render_mode));
				goto failed;
				break;
		}
	} else {
		glRenderMode(mode);
	}

	if (buf != NULL) {
		Tcl_DecrRefCount(buf);
		buf = NULL;
	}

	return TCL_OK;

failed:
	if (buf != NULL) {
		Tcl_DecrRefCount(buf);
		buf = NULL;
	}
	return TCL_ERROR;
}


static int glue_glInitNames(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glInitNames();

	return TCL_OK;
}


static int glue_glPushName(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			id;

	CHECK_ARGS(1, "id");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &id));

	glPushName(id);

	return TCL_OK;
}


static int glue_glLoadName(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			id;

	CHECK_ARGS(1, "id");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &id));

	glLoadName(id);

	return TCL_OK;
}


static int glue_glPopName(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	glPopName();

	return TCL_OK;
}


static int glue_glSelectBuffer(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			arraysize, buflen;
	GLuint		*selectbuf;
	Tcl_Obj*	buf;
	Tcl_Obj*	res;

	CHECK_ARGS(1, "arraysize");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &arraysize));

	res = Tcl_NewDictObj();

	Tcl_DictObjPut(interp, res, Tcl_NewStringObj("buftype", -1),
			Tcl_NewIntObj(GL_SELECT));

	// Eeek
	buf = Tcl_NewByteArrayObj(NULL, sizeof(GLuint) * arraysize);
	Tcl_DictObjPut(interp, res, Tcl_NewStringObj("buf", -1), buf);

	Tcl_DictObjPut(interp, res, Tcl_NewStringObj("arraysize", -1),
			Tcl_NewIntObj(arraysize));

	Tcl_IncrRefCount(res);

	selectbuf = (GLuint*)Tcl_GetByteArrayFromObj(buf, &buflen);

	glSelectBuffer(arraysize, selectbuf);

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_gluPickMatrix(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLdouble	x, y, width, height;
	GLint		viewport[4];

	CHECK_ARGS(4, "x y width height");

	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &width));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[4], &height));

	glGetIntegerv(GL_VIEWPORT, viewport);

	gluPickMatrix(x, y, width, height, viewport);

	return TCL_OK;
}


static int glue_glGenBuffers(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint*		buffers;
	int			count, i;
	Tcl_Obj*	res;

	CHECK_ARGS(1, "count");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &count));

	buffers = (GLuint*)malloc(sizeof(GLuint) * count);
	glGenBuffers(count, buffers);

	res = Tcl_NewListObj(0, NULL);

	for (i=0; i<count; i++) {
		TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(buffers[i])));
	}
	free(buffers); buffers = NULL;

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_glIsBuffer(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			buffer;

	CHECK_ARGS(1, "buffer");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &buffer));

	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(glIsBuffer(buffer) == GL_TRUE));

	return TCL_OK;
}


static int glue_glDeleteBuffers(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLuint*		buffers = NULL;
	Tcl_Obj**	ov;
	int			oc, i, tmp;

	CHECK_ARGS(1, "buffers");

	TEST_OK(Tcl_ListObjGetElements(interp, objv[1], &oc, &ov));
	buffers = (GLuint*)malloc(sizeof(GLuint) * oc);
	for (i=0; i<oc; i++) {
		if (Tcl_GetIntFromObj(interp, ov[i], &tmp) != TCL_OK)
			goto error;
		buffers[i] = tmp;
	}

	glDeleteBuffers(oc, buffers);

	free(buffers);

	return TCL_OK;

error:
	if (buffers != NULL) {
		free(buffers);
	}
	return TCL_ERROR;
}


static int glue_glBindBuffer(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int		buffer;
	int		index;
	GLenum	target;
	static CONST char *targets[] = {
		"GL_ARRAY_BUFFER",
		"GL_ELEMENT_ARRAY_BUFFER",
		"GL_PIXEL_PACK_BUFFER",
		"GL_PIXEL_UNPACK_BUFFER",
		(char *)NULL
	};
	GLenum target_map[] = {
		GL_ARRAY_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER,
		GL_PIXEL_PACK_BUFFER,
		GL_PIXEL_UNPACK_BUFFER
	};

	CHECK_ARGS(2, "target buffer");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], targets, "target", TCL_EXACT,
				&index));
	target = target_map[index];
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &buffer));

	glBindBuffer(target, buffer);

	return TCL_OK;
}


static int glue_glBufferData(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLsizeiptr	size = 0;
	int			tmp;
	GLvoid*		data = NULL;
	int			index;
	GLenum		target;
	static CONST char *targets[] = {
		"GL_ARRAY_BUFFER",
		"GL_ELEMENT_ARRAY_BUFFER",
		"GL_PIXEL_PACK_BUFFER",
		"GL_PIXEL_UNPACK_BUFFER",
		(char *)NULL
	};
	GLenum target_map[] = {
		GL_ARRAY_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER,
		GL_PIXEL_PACK_BUFFER,
		GL_PIXEL_UNPACK_BUFFER
	};
	GLenum	usage;
	static CONST char *usages[] = {
		"GL_STREAM_DRAW",
		"GL_STREAM_READ",
		"GL_STREAM_COPY",
		"GL_STATIC_DRAW",
		"GL_STATIC_READ",
		"GL_STATIC_COPY",
		"GL_DYNAMIC_DRAW",
		"GL_DYNAMIC_READ",
		"GL_DYNAMIC_COPY",
		(char *)NULL
	};
	GLenum usage_map[] = {
		GL_ARRAY_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER,
		GL_PIXEL_PACK_BUFFER,
		GL_PIXEL_UNPACK_BUFFER
	};

	CHECK_ARGS(3, "target data usage");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], targets, "target", TCL_EXACT,
				&index));
	target = target_map[index];
	TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], usages, "usage", TCL_EXACT,
				&index));
	usage = usage_map[index];

	switch (target) {
		case GL_ARRAY_BUFFER:
		case GL_ELEMENT_ARRAY_BUFFER:
			data = Tcl_GetByteArrayFromObj(objv[2], &tmp);
			size = tmp;
			break;

		case GL_PIXEL_PACK_BUFFER:
		case GL_PIXEL_UNPACK_BUFFER:
			{
				gimp_image_t*	pmap;

				if (!Tcl_IsPMAPObj(objv[3]))
					THROW_ERROR("Need a pmap object to bind buffer to");

				TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap));

				size = pmap->width * pmap->height * 4;
				if (target == GL_PIXEL_PACK_BUFFER) {
					data = pmap->pixel_data;
				}
			}
			break;

		default:
			THROW_ERROR("Unhandled target");
	}

	glBufferData(target, size, data, usage);
	// TODO: check for GL_OUT_OF_MEMORY and GL_INVALID_VALUE

	return TCL_OK;
}


static int glue_glReadPixels(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int				x, y, width, height;
	gimp_image_t*	pmap;
	Tcl_Obj*		res;
	_pel			init;

	CHECK_ARGS(4, "x y width height");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &height));

	pmap = pmap_new(width, height, init);
	res = Tcl_NewPMAPObj(pmap);

	glPixelStorei(GL_PACK_ROW_LENGTH, pmap->width);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(x, y, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
			pmap->pixel_data);

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


static int glue_glReadBuffer(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			index;
	GLenum		mode;
	static CONST char *modes[] = {
		"GL_FRONT",
		"GL_FRONT_LEFT",
		"GL_FRONT_RIGHT",
		"GL_BACK",
		"GL_BACK_LEFT",
		"GL_BACK_RIGHT",
		"GL_LEFT",
		"GL_RIGHT",
		"GL_NONE",
		"GL_AUX1",
		"GL_AUX2",
		"GL_AUX3",
		(char *)NULL
	};
	GLenum mode_map[] = {
		GL_FRONT,
		GL_FRONT_LEFT,
		GL_FRONT_RIGHT,
		GL_BACK,
		GL_BACK_LEFT,
		GL_BACK_RIGHT,
		GL_LEFT,
		GL_RIGHT,
		GL_NONE,
		GL_AUX1,
		GL_AUX2,
		GL_AUX3
	};

	CHECK_ARGS(1, "mode");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], modes, "mode", TCL_EXACT,
				&index));
	mode = mode_map[index];

	glReadBuffer(mode);

	return TCL_OK;
}


static int glue_glDrawBuffer(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			index;
	GLenum		mode;
	static CONST char *modes[] = {
		"GL_FRONT",
		"GL_FRONT_LEFT",
		"GL_FRONT_RIGHT",
		"GL_BACK",
		"GL_BACK_LEFT",
		"GL_BACK_RIGHT",
		"GL_LEFT",
		"GL_RIGHT",
		"GL_NONE",
		"GL_AUX1",
		"GL_AUX2",
		"GL_AUX3",
		"GL_FRONT_AND_BACK",
		"GL_NONE",
		(char *)NULL
	};
	GLenum mode_map[] = {
		GL_FRONT,
		GL_FRONT_LEFT,
		GL_FRONT_RIGHT,
		GL_BACK,
		GL_BACK_LEFT,
		GL_BACK_RIGHT,
		GL_LEFT,
		GL_RIGHT,
		GL_NONE,
		GL_AUX1,
		GL_AUX2,
		GL_AUX3,
		GL_FRONT_AND_BACK,
		GL_NONE
	};


	CHECK_ARGS(1, "mode");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], modes, "mode", TCL_EXACT,
				&index));
	mode = mode_map[index];

	glDrawBuffer(mode);

	return TCL_OK;
}


// Init {{{1
int Pixel_gl_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.4", 0) == NULL) return TCL_ERROR;

	// GL
	NEW_CMD("pixel::gl::glShadeModel", glue_glShadeModel);
	NEW_CMD("pixel::gl::glCullFace", glue_glCullFace);
	NEW_CMD("pixel::gl::glFrontFace", glue_glFrontFace);
	NEW_CMD("pixel::gl::glEnable", glue_glEnable);
	NEW_CMD("pixel::gl::glDisable", glue_glDisable);
	NEW_CMD("pixel::gl::glClearColor", glue_glClearColor);
	NEW_CMD("pixel::gl::glViewport", glue_glViewport);
	NEW_CMD("pixel::gl::glGenList", glue_glGenList);
	NEW_CMD("pixel::gl::glNewList", glue_glNewList);
	NEW_CMD("pixel::gl::glEndList", glue_glEndList);
	NEW_CMD("pixel::gl::glCallList", glue_glCallList);
	NEW_CMD("pixel::gl::glBegin", glue_glBegin);
	NEW_CMD("pixel::gl::glEnd", glue_glEnd);
	NEW_CMD("pixel::gl::glColor3f", glue_glColor3f);
	NEW_CMD("pixel::gl::glColor4f", glue_glColor4f);
	NEW_CMD("pixel::gl::glPointSize", glue_glPointSize);
	NEW_CMD("pixel::gl::glVertex3f", glue_glVertex3f);
	NEW_CMD("pixel::gl::glNormal3f", glue_glNormal3f);
	NEW_CMD("pixel::gl::glClear", glue_glClear);
	NEW_CMD("pixel::gl::glMatrixMode", glue_glMatrixMode);
	NEW_CMD("pixel::gl::glLoadIdentity", glue_glLoadIdentity);
	NEW_CMD("pixel::gl::glTranslatef", glue_glTranslatef);
	NEW_CMD("pixel::gl::glRotatef", glue_glRotatef);
	NEW_CMD("pixel::gl::glGenTextures", glue_glGenTextures);
	NEW_CMD("pixel::gl::glBindTexture", glue_glBindTexture);
	NEW_CMD("pixel::gl::glTexParameter", glue_glTexParameter);
	NEW_CMD("pixel::gl::glTexParameteri", glue_glTexParameter);
	NEW_CMD("pixel::gl::glTexParameterf", glue_glTexParameter);
	NEW_CMD("pixel::gl::glTexParameterfv", glue_glTexParameter);
	NEW_CMD("pixel::gl::glTexEnv", glue_glTexEnv);
	NEW_CMD("pixel::gl::glTexEnvi", glue_glTexEnv);
	NEW_CMD("pixel::gl::glTexEnvfv", glue_glTexEnv);
	NEW_CMD("pixel::gl::glTexImage2D", glue_glTexImage2D);
	NEW_CMD("pixel::gl::glTexSubImage2D", glue_glTexSubImage2D);
	NEW_CMD("pixel::gl::glTexImage2D_luminance", glue_glTexImage2D_luminance);
	NEW_CMD("pixel::gl::glTexCoord2f", glue_glTexCoord2f);
	NEW_CMD("pixel::gl::glMultiTexCoord2f", glue_glMultiTexCoord2f);
	NEW_CMD("pixel::gl::glTexCoord3f", glue_glTexCoord3f);
	NEW_CMD("pixel::gl::glPushMatrix", glue_glPushMatrix);
	NEW_CMD("pixel::gl::glPopMatrix", glue_glPopMatrix);
	NEW_CMD("pixel::gl::glLoadMatrixf", glue_glLoadMatrixf);
	NEW_CMD("pixel::gl::glMultMatrixf", glue_glMultMatrixf);
	NEW_CMD("pixel::gl::glFlush", glue_glFlush);
	NEW_CMD("pixel::gl::glFinish", glue_glFinish);
	NEW_CMD("pixel::gl::glGetString", glue_glGetString);
	NEW_CMD("pixel::gl::glCreateProgram", glue_glCreateProgram);
	NEW_CMD("pixel::gl::glDeleteProgram", glue_glDeleteProgram);
	NEW_CMD("pixel::gl::glCreateShader", glue_glCreateShader);
	NEW_CMD("pixel::gl::glDeleteShader", glue_glDeleteShader);
	NEW_CMD("pixel::gl::glShaderSource", glue_glShaderSource);
	NEW_CMD("pixel::gl::glCompileShader", glue_glCompileShader);
	NEW_CMD("pixel::gl::glAttachShader", glue_glAttachShader);
	NEW_CMD("pixel::gl::glLinkProgram", glue_glLinkProgram);
	NEW_CMD("pixel::gl::glValidateProgram", glue_glValidateProgram);
	NEW_CMD("pixel::gl::glUseProgram", glue_glUseProgram);
	NEW_CMD("pixel::gl::glGetProgramInfoLog", glue_glGetProgramInfoLog);
	NEW_CMD("pixel::gl::glGetUniformLocation", glue_glGetUniformLocation);
	NEW_CMD("pixel::gl::glUniform1f", glue_glUniform1f);
	NEW_CMD("pixel::gl::glUniform1i", glue_glUniform1i);
	NEW_CMD("pixel::gl::glOrtho", glue_glOrtho);
	NEW_CMD("pixel::gl::glVertexList", glue_glVertexList);
	NEW_CMD("pixel::gl::glActiveTexture", glue_glActiveTexture);
	NEW_CMD("pixel::gl::glBlendFunc", glue_glBlendFunc);
	NEW_CMD("pixel::gl::glBlendEquation", glue_glBlendEquation);
	NEW_CMD("pixel::gl::glRenderMode", glue_glRenderMode);
	NEW_CMD("pixel::gl::glInitNames", glue_glInitNames);
	NEW_CMD("pixel::gl::glPushName", glue_glPushName);
	NEW_CMD("pixel::gl::glLoadName", glue_glLoadName);
	NEW_CMD("pixel::gl::glPopName", glue_glPopName);
	NEW_CMD("pixel::gl::glSelectBuffer", glue_glSelectBuffer);
	NEW_CMD("pixel::gl::glGenBuffers", glue_glGenBuffers);
	NEW_CMD("pixel::gl::glIsBuffer", glue_glIsBuffer);
	NEW_CMD("pixel::gl::glDeleteBuffers", glue_glDeleteBuffers);
	NEW_CMD("pixel::gl::glBindBuffer", glue_glBindBuffer);
	NEW_CMD("pixel::gl::glBufferData", glue_glBufferData);
	NEW_CMD("pixel::gl::glReadPixels", glue_glReadPixels);
	NEW_CMD("pixel::gl::glReadBuffer", glue_glReadBuffer);
	NEW_CMD("pixel::gl::glDrawBuffer", glue_glDrawBuffer);

	// GLU
	NEW_CMD("pixel::gl::gluPerspective", glue_gluPerspective);
	NEW_CMD("pixel::gl::gluOrtho2D", glue_gluOrtho2D);
	NEW_CMD("pixel::gl::gluPickMatrix", glue_gluPickMatrix);

	// GLEW
	init_glew(interp);

	// Helpers

	TEST_OK(Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION));

	return TCL_OK;
}


