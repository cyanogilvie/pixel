#include "main.h"

static Tcl_Obj*	g_glew_error;

static int glue_glewInit(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	GLenum	err;

	CHECK_ARGS(0, "");

	err = glewInit();
	if (err != GLEW_OK) {
		Tcl_SetErrorCode(interp, "GLEW", glewGetErrorString(err));
		Tcl_SetObjResult(interp, Tcl_NewStringObj((const char*)glewGetErrorString(err), -1));
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp,
			Tcl_NewStringObj((const char*)glewGetString(GLEW_VERSION), -1));

	return TCL_OK;
}

//>>>
static int glue_glewIsSupported(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(1, "extensions");

	Tcl_SetObjResult(interp,
			Tcl_NewBooleanObj(glewIsSupported(Tcl_GetString(objv[1]))));

	return TCL_OK;
}

//>>>
int init_glew(Tcl_Interp* interp) //<<<
{
	g_glew_error = Tcl_NewStringObj("GLEW", -1);
	Tcl_IncrRefCount(g_glew_error);

	NEW_CMD("pixel::gl::glewInit", glue_glewInit);
	NEW_CMD("pixel::gl::glewIsSupported", glue_glewIsSupported);

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
