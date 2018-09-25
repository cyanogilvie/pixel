#include "tclstuff.h"
#include <pHash.h>
#include <pixel.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static const Cimg* pmapf_to_cimg(struct pmapf* pmapf) //{{{
{
	Cimg<float> out(pmapf->pixel_data, pmapf->width, pmapf->height, 1, 4, true);

	return out;
}

//}}}
static const Cimg* pmap_to_cimg(gimp_image_t* pmap) //{{{
{
	Cimg<uint8_t> out(pmap->pixel_data, pmap->width, pmap->height, 1, 4, true);

	return out;
}

//}}}
static int dct_imagehash(ClientData cdata, Tcl_Interp* interp, int objc, const Tcl_Obj* objv[]) //{{{
{
	const char*		fn;
	ulong64			hash = 0;
	Cimg*			cimg = NULL;
	int				ret;

	CHECK_ARGS(1, "fn");

	fn = Tcl_GetString(objv[1]);

	ret = ph_dct_imagehash(fn, hash);
	if (ret < 0)
		THROW_ERROR("Could not compute digest");

	Tcl_SetObjResult(interp, Tcl_NewWideIntObj(hash));

	return TCL_OK;
}

//}}}
static int image_digest(ClientData cdata, Tcl_Interp* interp, int objc, const Tcl_Obj* objv[]) //{{{
{
	gimp_image_t*	inp = NULL;
	ulong64			hash = 0;
	Cimg*			cimg = NULL;
	int				ret, N;
	double			sigma, gamma;
	Digest			digest;

	CHECK_ARGS(4, "pmap sigma gamma N");

	TEST_OK(Pixel_GetPMAPFromObj(interp, objv[1], &inp));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &sigma));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &gamma));
	TEST_OK(Pixel_GetPMAPFromObj(interp, objv[4], &N));

	cimg = pmap_to_cimg(inp);

	ret = _ph_image_digest(*cimg, sigma, gamma, digest, N);
	if (ret < 0)
		THROW_ERROR("Could not compute digest");


	THROW_ERROR("Not implemented yet");

	return TCL_OK;
}

//}}}
extern "C" int Pixel_phash(Tcl_Interp* interp) //{{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.5", 0) == NULL) return TCL_ERROR;

	NEW_CMD("::pixel::phash::image_digest", image_digest);
	NEW_CMD("::pixel::phash::dct_imagehash", dct_imagehash);
	/*
	NEW_CMD("::pixel::phash::compare_images", compare_images);
	NEW_CMD("::pixel::phash::hamming_distance", hamming_distance);
	NEW_CMD("::pixel::phash::mh_imagehash", mh_imagehash);
	NEW_CMD("::pixel::phash::hamming_distance2", hamming_distance2);
	*/


	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
