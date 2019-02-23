#include <stdint.h>
#include <webp/encode.h>
#include <webp/decode.h>
#include "tclstuff.h"
#include <pixel.h>


static int glue_encodewebp(ClientData* cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	gimp_image_t*	pmap;
	int				quality;
	size_t			length;
	uint8_t*		webp_data;
	
	CHECK_ARGS(2, "pmap quality");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &quality));

	length = WebPEncodeBGRA((uint8_t*)pmap->pixel_data, pmap->width, pmap->height, pmap->width*4, quality, &webp_data);

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(webp_data, length));

	free(webp_data);
	
	return TCL_OK;
}

//}}}
static int glue_encodewebp_noalpha(ClientData* cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	gimp_image_t*	pmap;
	int				quality;
	size_t			length;
	uint8_t*		webp_data;
	
	CHECK_ARGS(2, "pmap quality");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &quality));

	length = WebPEncodeBGR((uint8_t*)pmap->pixel_data, pmap->width, pmap->height, pmap->width*4, quality, &webp_data);

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(webp_data, length));

	free(webp_data);
	
	return TCL_OK;
}

//}}}
static int glue_decodewebp(ClientData* cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	uint8_t*		webp_data;
	int				length;
	gimp_image_t*	new;
	
	CHECK_ARGS(1, "webp_data");

	webp_data = Tcl_GetByteArrayFromObj(objv[1], &length);

	new = (gimp_image_t*)malloc(sizeof(gimp_image_t));
	new->bytes_per_pixel = 4;
	new->pixel_data = (_pel*)WebPDecodeBGRA(webp_data, length, (int*)&new->width, (int*)&new->height);
	if (new == NULL)
		THROW_ERROR("Error loading webp");

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}

//}}}
int Pixel_webp_Init(Tcl_Interp* interp) //{{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	NEW_CMD("pixel::webp::encode", glue_encodewebp);
	NEW_CMD("pixel::webp::encode_noalpha", glue_encodewebp_noalpha);
	NEW_CMD("pixel::webp::decode", glue_decodewebp);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: foldmethod=marker foldmarker={{{,}}}
