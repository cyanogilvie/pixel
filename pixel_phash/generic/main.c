#include <pHash.h>

extern "C" {
#include "tclstuff.h"
#include <pixel.h>
}
#include "digest_type.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*
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
*/
static int dct_imagehash(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	const char*		fn;
	ulong64			hash = 0;
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
static int image_digest(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	gimp_image_t*	inp = NULL;
	int				ret, N;
	double			sigma, gamma;
	Digest			digest;

	// Use sigma=1, gamma=1, N=180
	CHECK_ARGS(4, "pmap sigma gamma N");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &inp));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &sigma));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &gamma));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &N));

	CImg<uint8_t> cimg((uint8_t*)inp->pixel_data, inp->width, inp->height, 1, 4, true);

	ret = _ph_image_digest(cimg, sigma, gamma, digest, N);
	if (ret < 0)
		THROW_ERROR("Could not compute digest");

	Tcl_SetObjResult(interp, Pixel_NewPHashDigestObj(&digest));

	return TCL_OK;
}

//}}}
static int image_digest_filename(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	int				ret, N;
	double			sigma, gamma;
	Digest			digest;
	const char*		fn;

	// Use sigma=1, gamma=1, N=180
	CHECK_ARGS(4, "filename sigma gamma N");

	fn = Tcl_GetString(objv[1]);
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &sigma));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &gamma));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &N));

	ret = ph_image_digest(fn, sigma, gamma, digest, N);
	if (ret < 0)
		THROW_ERROR("Could not compute digest");

	Tcl_SetObjResult(interp, Pixel_NewPHashDigestObj(&digest));

	return TCL_OK;
}

//}}}
static int hamming_distance(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	Tcl_WideInt		thash1, thash2;
	ulong64			hash1, hash2;

	CHECK_ARGS(2, "hash1 hash2");

	TEST_OK(Tcl_GetWideIntFromObj(interp, objv[1], &thash1));
	TEST_OK(Tcl_GetWideIntFromObj(interp, objv[2], &thash2));

	hash1 = thash1;
	hash2 = thash2;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(ph_hamming_distance(hash1, hash2)));

	return TCL_OK;
}

//}}}
static int crosscorr(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	Digest*		dig1;
	Digest*		dig2;
	double		threshold, pcc;
	int			ret;

	CHECK_ARGS(3, "digest1 digest2 threshold");

	TEST_OK(Pixel_GetPHashDigestFromObj(interp, objv[1], &dig1));
	TEST_OK(Pixel_GetPHashDigestFromObj(interp, objv[2], &dig2));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &threshold));

	ret = ph_crosscorr(*dig1, *dig2, pcc, threshold);
	if (ret < 0)
		THROW_ERROR("Error computing peack cross correlation");

	Tcl_SetObjResult(interp, Tcl_NewDoubleObj(pcc));

	return TCL_OK;
}

//}}}
static int mh_imagehash(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	int				N;
	uint8_t*		hash = NULL;
	double			alpha, lvl;
	const char*		fn;

	// Use alpha=2.0, lvl=1.0
	CHECK_ARGS(3, "filename alpha lvl");

	fn = Tcl_GetString(objv[1]);
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[2], &alpha));
	TEST_OK(Tcl_GetDoubleFromObj(interp, objv[3], &lvl));

	hash = ph_mh_imagehash(fn, N, alpha, lvl);

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(hash, N));

	return TCL_OK;
}

//}}}
static int hammingdistance2(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) //{{{
{
	uint8_t*	hash1;
	uint8_t*	hash2;
	int			len1, len2;

	CHECK_ARGS(2, "hash1 hash2");

	hash1 = (uint8_t*)Tcl_GetByteArrayFromObj(objv[1], &len1);
	hash2 = (uint8_t*)Tcl_GetByteArrayFromObj(objv[2], &len2);

	Tcl_SetObjResult(interp, Tcl_NewDoubleObj(ph_hammingdistance2(hash1, len1, hash2, len2)));

	return TCL_OK;
}

//}}}
extern "C" int Pixel_phash_Init(Tcl_Interp* interp) //{{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.5", 0) == NULL) return TCL_ERROR;

	NEW_CMD("::pixel::phash::image_digest", image_digest);
	NEW_CMD("::pixel::phash::image_digest_filename", image_digest_filename);
	NEW_CMD("::pixel::phash::hamming_distance", hamming_distance);
	NEW_CMD("::pixel::phash::dct_imagehash", dct_imagehash);
	NEW_CMD("::pixel::phash::crosscorr", crosscorr);
	NEW_CMD("::pixel::phash::mh_imagehash", mh_imagehash);
	NEW_CMD("::pixel::phash::hammingdistance2", hammingdistance2);
	/*
	NEW_CMD("::pixel::phash::compare_images", compare_images);
	*/

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
