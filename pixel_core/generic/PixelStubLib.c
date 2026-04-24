#if HAVE_CONFIG_H
#	include <config.h>
#endif

#undef USE_TCL_STUBS
#undef USE_PIXEL_STUBS
#define USE_TCL_STUBS 1
#define USE_PIXEL_STUBS 1

#include "pixel.h"

MODULE_SCOPE const PixelStubs*	pixelStubsPtr;
const PixelStubs*				pixelStubsPtr = NULL;

#undef Pixel_InitStubs
const char *Pixel_InitStubs(Tcl_Interp *interp, const char *version, int exact)
{
	const char	*actualVersion;

	actualVersion = Tcl_PkgRequireEx(interp, PACKAGE_NAME, version, exact,
			(void *)&pixelStubsPtr);

	if (!actualVersion)
		return NULL;

	if (!pixelStubsPtr) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(
				"This implementation of " PACKAGE_NAME " does not support stubs", -1));
		return NULL;
	}

	return actualVersion;
}
