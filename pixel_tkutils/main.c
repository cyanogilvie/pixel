#include <tclstuff.h>
#include <tk.h>
#include "../2d.h"

static int glue_colour_tk2pel(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	XColor		*xcol;
	_pel		newcol;

	CHECK_ARGS(1, "colour");

	xcol = Tk_AllocColorFromObj(interp, Tk_MainWindow(interp), objv[1]);

	if (xcol == NULL)
		return TCL_ERROR;
		//THROW_ERROR("Invalid colour specification: (", Tcl_GetString(objv[1]), ")");

	newcol.ch.a = 0xff;
	newcol.ch.r = xcol->red;
	newcol.ch.g = xcol->green;
	newcol.ch.b = xcol->blue;

	Tk_FreeColor(xcol); xcol = NULL;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(newcol.c));

	return TCL_OK;
}


// Init {{{1
int Pixel_tkutils_Init(Tcl_Interp *interp)
{
	NEW_CMD("pixel::tkutils::colour_tk2pel", glue_colour_tk2pel);

	return TCL_OK;
}


