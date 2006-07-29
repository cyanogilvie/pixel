#include <tclstuff.h>
#include "2d.h"
#include "tcl_pmap.h"

#include <IL/il.h>
#include <IL/ilu.h>
//#include <IL/ilut.h>


static int glue_load_image(cdata, interp, objc, objv)
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


static int glue_load_image_from_var(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	ILuint				id, Error, size;
	_pel				init;
	gimp_image_t		*new;
	int					w, h, data_len;
	ILvoid				*data;
	
	CHECK_ARGS(1, "data");

	ilGenImages(1, &id);
	ilBindImage(id);
	data = (ILvoid *)Tcl_GetByteArrayFromObj(objv[1], &data_len);
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


int Pixel_devil_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
		return TCL_ERROR;
	}

	ilInit();
	iluInit();

	NEW_CMD("pixel::devil::load_image", glue_load_image);
	NEW_CMD("pixel::devil::load_image_from_var", glue_load_image_from_var);

	return TCL_OK;
}


