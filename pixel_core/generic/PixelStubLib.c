#ifndef USE_TCL_STUBS
#define USE_TCL_STUBS
#endif
#undef USE_TCL_STUB_PROCS

#include "tcl.h"
#include "pixel.h"

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

PixelStubs *pixelStubsPtr;

CONST char *Pixel_InitStubs(Tcl_Interp *interp, char *version, int exact)
{
	char	*actualVersion;

	actualVersion = Tcl_PkgRequireEx(interp, "Pixel", version, exact,
			(ClientData *)&pixelStubsPtr);

	if (!actualVersion)
		return NULL;

	if (!pixelStubsPtr) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj("This implementation of Pixel does not support stubs", -1));
		return NULL;
	}

	return actualVersion;
}
