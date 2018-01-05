#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include "all.h"

static int glue_loadpng(cdata, interp, objc, objv) // loadpng filename {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	gimp_image_t *	pmap;
	CHECK_ARGS(1, "filename");

	pmap = read_png(Tcl_GetString(objv[1]));

	if (pmap == NULL)
		THROW_ERROR("Error loading png file: ", Tcl_GetString(objv[1]));
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));

	return TCL_OK;
}

//}}}
/*
static int glue_savepng(cdata, interp, objc, objv) // savepng filename pmap {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int				err;
	gimp_image_t	*pmap;
	int				quality;
	
	CHECK_ARGS(2, "filename pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &quality));


	if (err != 0)
		THROW_ERROR("Error saving png file: ", Tcl_GetString(objv[1]));
	
	return TCL_OK;
}

//}}}
*/
static int glue_is_png(cdata, interp, objc, objv) // is_png filename {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	FILE			*fp;
	unsigned char	buf[8];

	CHECK_ARGS(1, "filename");

	fp = fopen(Tcl_GetString(objv[1]), "rb");
	if (fp == NULL)
		THROW_ERROR("Unable to open file: (", Tcl_GetString(objv[1]), ")");

	if (fread(buf, 1, 8, fp) != 8) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(0));
		return TCL_OK;
	}

	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(!png_sig_cmp(buf, (png_size_t)0, 8)));

	return TCL_OK;
}

//}}}
static int glue_png_dimensions(cdata, interp, objc, objv) // png_dimensions filename {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			x, y;
	Tcl_Obj		*res;

	CHECK_ARGS(1, "filename");

	if (get_png_dimensions(Tcl_GetString(objv[1]), &x, &y) != 0)
		THROW_ERROR("Error reading PNG dimensions");

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(x)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(y)));
	
	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}

//}}}
static int glue_encode(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	CHECK_ARGS(1, "pmap");
	return TCL_OK;
}

//}}}
static void mem_read(png_structp png_ptr, png_bytep out, png_size_t count) //{{{
{
	struct png_membuf*	pngdata = (struct png_membuf*)png_get_io_ptr(png_ptr);

	if (pngdata->ofs + count > pngdata->len) {
		// TODO: how to signal an error from here?
		//memset((uint8_t*)out + (ptrdiff_t)count, 0, (pngdata->ofs+count) - pngdata->len);
		count = pngdata->len - pngdata->ofs;
	}

	memcpy((uint8_t*)out, (uint8_t*)(pngdata->buf)+(ptrdiff_t)pngdata->ofs, count);
	pngdata->ofs += count;
	//fprintf(stderr, "read %ld bytes, ofs: %d\n", count, pngdata->ofs);
}

//}}}
static int glue_decode(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	png_structp			png_ptr;
	png_infop			info_ptr;
	png_uint_32			width, height;
	int					bitdepth=0, colourtype=-1;
	int					i, retval;
	png_bytep*			row_pointers = NULL;
	gimp_image_t*		pmap = NULL;
	_pel				init;
	struct png_membuf	pngdata;

	CHECK_ARGS(1, "pngdata");

	//if (get_png_dimensions(filename, &width, &height) != 0) return NULL;

	pngdata.ofs = 0;
	pngdata.buf = Tcl_GetByteArrayFromObj(objv[1], &pngdata.len);
	//fprintf(stderr, "pngdata.len: %d, pngdata.ofs: %d\n", pngdata.len, pngdata.ofs);
	if (pngdata.len < PNG_SIG_LEN || !png_check_sig(pngdata.buf, PNG_SIG_LEN)) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "NOT_A_PNG", NULL);
		THROW_ERROR("Not a PNG");
	}

	// First pass: find the width and height {{{
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_READ_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG read struct", -1));
		goto error;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_INFO_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG info struct", -1));
		goto error;
	}

	png_set_read_fn(png_ptr, &pngdata, mem_read);

	if (setjmp(png_ptr->jmpbuf)) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "READ", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Error reading PNG", -1));
		goto error;
	}

	png_read_info(png_ptr, info_ptr);

	//fprintf(stderr, "Reading IHDR\n");
	retval = png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitdepth, &colourtype, NULL, NULL, NULL);
	if (retval != 1) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "METADATA", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Error reading PNG metadata", -1));
		goto error;
	}
	//fprintf(stderr, "width: %ld, height: %ld, bitdepth: %d, colourtype: %d\n", width, height, bitdepth, colourtype);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  png_ptr = NULL; info_ptr = NULL;
	// First pass: find the width and height }}}

	// Have to re-init the context structs to use png_read_png for some reason {{{
	pngdata.ofs = 0;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_READ_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG read struct", -1));
		goto error;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_INFO_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG info struct", -1));
		goto error;
	}

	png_set_read_fn(png_ptr, &pngdata, mem_read);

	if (setjmp(png_ptr->jmpbuf)) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "READ", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Error reading PNG", -1));
		goto error;
	}
	// Have to re-init the context structs to use png_read_png for some reason }}}

	init.c = 0;
	pmap = pmap_new(width, height, init);

	pmap_clr(pmap, init);

	row_pointers = (png_bytep*)malloc(height * sizeof(png_bytep));
	for (i=0; i<height; i++)
		row_pointers[i] = (png_bytep)(pmap->pixel_data + (i * width));

	png_set_rows(png_ptr, info_ptr, row_pointers);

	png_read_png(png_ptr, info_ptr, 
			PNG_TRANSFORM_STRIP_16 |
			PNG_TRANSFORM_PACKING |
			PNG_TRANSFORM_SHIFT |
			PNG_TRANSFORM_BGR |
			PNG_TRANSFORM_EXPAND, NULL);

	free(row_pointers);  row_pointers = NULL;
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  png_ptr = NULL; info_ptr = NULL;
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));
	return TCL_OK;

error:
	if (row_pointers) {
		free(row_pointers);  row_pointers = NULL;
	}
	if (pmap) {
		free(pmap);  pmap = NULL;
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  png_ptr = NULL; info_ptr = NULL;
	return TCL_ERROR;
}

//}}}
int Pixel_png_Init(Tcl_Interp *interp) // Init {{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	NEW_CMD("pixel::png::loadpng", glue_loadpng);
	//NEW_CMD("pixel::png::savepng", glue_savepng);
	NEW_CMD("pixel::png::is_png", glue_is_png);
	NEW_CMD("pixel::png::png_dimensions", glue_png_dimensions);
	NEW_CMD("pixel::png::encode", glue_encode);
	NEW_CMD("pixel::png::decode", glue_decode);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}
