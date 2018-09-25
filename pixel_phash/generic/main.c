#include "tclstuff.h"
#include <pixel.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static int phash(ClientData cdata, Tcl_Interp* interp, int objc, const Tcl_Obj* objv[]) //{{{
{
	struct pmapf*	inp=NULL;
	TEST_OK(Pixel_GetPMAPFFromObj(interp, objv[1], &inp));

	return TCL_OK;
}

//}}}
int Pixel_phash(Tcl_Interp* interp) //{{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.5", 0) == NULL) return TCL_ERROR;

	NEW_CMD("::pixel::phash::phash", phash);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
