// vim: ts=4 shiftwidth=4 tags=../tags

#include <GL/gl.h>
#include <GL/glu.h>
#include <tcl.h>
#include <tclstuff.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <Pixel/pixel.h>

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


// Init {{{1
int Pixel_gl_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
		return TCL_ERROR;

	// GL
	NEW_CMD("pixel::sdl::glShadeModel", glue_glShadeModel);
	NEW_CMD("pixel::sdl::glCullFace", glue_glCullFace);
	NEW_CMD("pixel::sdl::glFrontFace", glue_glFrontFace);
	NEW_CMD("pixel::sdl::glEnable", glue_glEnable);
	NEW_CMD("pixel::sdl::glDisable", glue_glDisable);
	NEW_CMD("pixel::sdl::glClearColor", glue_glClearColor);
	NEW_CMD("pixel::sdl::glViewport", glue_glViewport);
	NEW_CMD("pixel::sdl::glGenList", glue_glGenList);
	NEW_CMD("pixel::sdl::glNewList", glue_glNewList);
	NEW_CMD("pixel::sdl::glEndList", glue_glEndList);
	NEW_CMD("pixel::sdl::glCallList", glue_glCallList);
	NEW_CMD("pixel::sdl::glBegin", glue_glBegin);
	NEW_CMD("pixel::sdl::glEnd", glue_glEnd);
	NEW_CMD("pixel::sdl::glColor3f", glue_glColor3f);
	NEW_CMD("pixel::sdl::glColor4f", glue_glColor4f);
	NEW_CMD("pixel::sdl::glPointSize", glue_glPointSize);
	NEW_CMD("pixel::sdl::glVertex3f", glue_glVertex3f);
	NEW_CMD("pixel::sdl::glNormal3f", glue_glNormal3f);
	NEW_CMD("pixel::sdl::glClear", glue_glClear);
	NEW_CMD("pixel::sdl::glMatrixMode", glue_glMatrixMode);
	NEW_CMD("pixel::sdl::glLoadIdentity", glue_glLoadIdentity);
	NEW_CMD("pixel::sdl::glTranslatef", glue_glTranslatef);
	NEW_CMD("pixel::sdl::glRotatef", glue_glRotatef);
	NEW_CMD("pixel::sdl::glGenTextures", glue_glGenTextures);
	NEW_CMD("pixel::sdl::glBindTexture", glue_glBindTexture);
	NEW_CMD("pixel::sdl::glTexParameter", glue_glTexParameter);
	NEW_CMD("pixel::sdl::glTexParameteri", glue_glTexParameter);
	NEW_CMD("pixel::sdl::glTexParameterf", glue_glTexParameter);
	NEW_CMD("pixel::sdl::glTexParameterfv", glue_glTexParameter);
	NEW_CMD("pixel::sdl::glTexEnv", glue_glTexEnv);
	NEW_CMD("pixel::sdl::glTexEnvi", glue_glTexEnv);
	NEW_CMD("pixel::sdl::glTexEnvfv", glue_glTexEnv);
	NEW_CMD("pixel::sdl::glTexImage2D", glue_glTexImage2D);
	NEW_CMD("pixel::sdl::glTexCoord2f", glue_glTexCoord2f);

	// GLU
	NEW_CMD("pixel::sdl::gluPerspective", glue_gluPerspective);

	return TCL_OK;
}


