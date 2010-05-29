// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
//
#include <tclstuff.h>
#include <Pixel/2d.h>
#include <Pixel/tcl_pmap.h>

#include <IL/il.h>
#include <IL/ilu.h>
//#include <IL/ilut.h>


static int glue_load_image(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	ILuint				id, Error;
	_pel				init;
	gimp_image_t		*new;
	int					w, h;
	
	CHECK_ARGS(1, "filename");

	ilGenImages(1, &id);
	ilBindImage(id);
	ilLoadImage(Tcl_GetString(objv[1]));
	Error = ilGetError();

	if (Error != IL_NO_ERROR) {
		Tcl_Obj		*errors = Tcl_NewStringObj(NULL, 0);

		Tcl_AppendStringsToObj(errors, 
				"Error loading image:\n\t", 
				iluErrorString(Error), NULL);

		while ((Error = ilGetError()) != IL_NO_ERROR) {
			Tcl_AppendStringsToObj(errors, "\n\t", iluErrorString(Error), NULL);
		}

		ilDeleteImages(1, &id);

		Tcl_SetObjResult(interp, errors);

		return TCL_ERROR;
	}

	init.c = 0;
	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	new = pmap_new(w, h, init);

	// TODO: catch errors
	ilCopyPixels(0, 0, 0, w, h, 1, IL_BGRA, IL_UNSIGNED_BYTE, new->pixel_data);

	ilDeleteImages(1, &id);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}

//>>>
static int glue_load_image_from_var(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	ILuint				id, Error, size;
	_pel				init;
	gimp_image_t		*new;
	int					w, h, data_len;
	const void			*data;
	
	CHECK_ARGS(1, "data");

	ilGenImages(1, &id);
	ilBindImage(id);
	data = (const void *)Tcl_GetByteArrayFromObj(objv[1], &data_len);
	size = data_len;
	ilLoadL(IL_TYPE_UNKNOWN, data, size);
	Error = ilGetError();

	if (Error != IL_NO_ERROR) {
		Tcl_Obj		*errors = Tcl_NewStringObj(NULL, 0);

		Tcl_AppendStringsToObj(errors, 
				"Error loading image:\n\t", 
				iluErrorString(Error), NULL);

		while ((Error = ilGetError()) != IL_NO_ERROR) {
			Tcl_AppendStringsToObj(errors, "\n\t", iluErrorString(Error), NULL);
		}

		ilDeleteImages(1, &id);

		Tcl_SetObjResult(interp, errors);

		return TCL_ERROR;
	}

	init.c = 0;
	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	new = pmap_new(w, h, init);

	// TODO: catch errors
	ilCopyPixels(0, 0, 0, w, h, 1, IL_BGRA, IL_UNSIGNED_BYTE, new->pixel_data);

	ilDeleteImages(1, &id);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}

//>>>
static int glue_scale_pmap(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	ILuint				id;
	_pel				init;
	gimp_image_t		*src;
	gimp_image_t		*new;
	int					new_w, new_h, index;
	ILenum				filter;		// unsigned int
	static CONST char *filters[] = {
		"ILU_NEAREST",
		"ILU_LINEAR",
		"ILU_BILINEAR",
		"ILU_SCALE_BOX",
		"ILU_SCALE_TRIANGLE",
		"ILU_SCALE_BELL",
		"ILU_SCALE_BSPLINE",
		"ILU_SCALE_LANCZOS3",
		"ILU_SCALE_MITCHELL",
		(char *)NULL
	};
	ILenum map[] = {
		ILU_NEAREST,
		ILU_LINEAR,
		ILU_BILINEAR,
		ILU_SCALE_BOX,
		ILU_SCALE_TRIANGLE,
		ILU_SCALE_BELL,
		ILU_SCALE_BSPLINE,
		ILU_SCALE_LANCZOS3,
		ILU_SCALE_MITCHELL
	};

	if (objc < 4 || objc > 5) {
		CHECK_ARGS(3, "src_pmap new_width new_height ?filter?");
	}

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &new_w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &new_h));
	if (objc == 5) {
		TEST_OK(Tcl_GetIndexFromObj(interp, objv[4], filters, "filter",
					TCL_EXACT, &index));
		filter = map[index];
	} else {
		filter = ILU_SCALE_LANCZOS3;	// Good but slow
	}

	ilGenImages(1, &id);
	ilBindImage(id);

	ilTexImage(src->width, src->height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE,
			src->pixel_data);
	iluImageParameter(ILU_FILTER, filter);
	iluScale(new_w, new_h, 1);

	init.c = 0;
	new = pmap_new(new_w, new_h, init);

	ilCopyPixels(0, 0, 0, new_w, new_h, 1, IL_BGRA, IL_UNSIGNED_BYTE,
			new->pixel_data);

	ilDeleteImages(1, &id);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}

//>>>
int Pixel_devil_Init(Tcl_Interp *interp) //<<<
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
		return TCL_ERROR;
	}

	ilInit();
	iluInit();

	NEW_CMD("pixel::devil::load_image", glue_load_image);
	NEW_CMD("pixel::devil::load_image_from_var", glue_load_image_from_var);
	NEW_CMD("pixel::devil::scale_pmap", glue_scale_pmap);

	return TCL_OK;
}

//>>>
