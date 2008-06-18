#include <tcl.h>
#include <tk.h>
#include <tclstuff.h>
#include <Pixel/pixel.h>

/*
static int match_pmap(dataObj, format, widthPtr, heightPtr, interp) //<<<
	Tcl_Obj		*dataObj;
	Tcl_Obj		*format;
	int			*widthPtr;
	int			*heightPtr;
	Tcl_Interp	*interp;
{
	gimp_image_t	*pmap;
	fprintf(stderr, "match_pmap enter\n");
	if (format != NULL) {
		fprintf(stderr, "Requested format: %s, strcmp \"pmap\"? (%d)\n",
				Tcl_GetString(format),
				strcmp(Tcl_GetString(format), "pmap")
				);
	}

	if (format != NULL && strcmp(Tcl_GetString(format), "pmap") == 0) {
		if (dataObj->typePtr != &tcl_pmap) {
			fprintf(stderr, "Internal type is not pmap, refusing match\n");
			return 0;
		} else {
			fprintf(stderr, "Internal type is pmap\n");
		}
	}

	fprintf(stderr, "FOO\n");
	fprintf(stderr, "dataObj is currently of type: %s\n",
			dataObj->typePtr->name);
	if (Tcl_GetPMAPFromObj(interp, dataObj, &pmap) != TCL_OK) {
		fprintf(stderr, "Couldn't load pmap from object:");
		fprintf(stderr, "%s\n", Tcl_GetString(Tcl_GetObjResult(interp)));
		return 0;
	}

	fprintf(stderr, "BAR\n");
	*widthPtr = pmap->width;
	*heightPtr = pmap->height;

	fprintf(stderr, "BAZ\n");
	fprintf(stderr, "match_pmap leave\n");
	return 1;
}

//>>>
static int read_pmap(interp, dataObj, format, imageHandle, destX, destY, width, height, srcX, srcY) //<<<
	Tcl_Interp		*interp;
	Tcl_Obj			*dataObj;
	Tcl_Obj			*format;
	Tk_PhotoHandle	imageHandle;
	int				destX, destY, width, height, srcX, srcY;
{
	gimp_image_t		*pmap;
	int					outWidth, outHeight, old_w, old_h;
	Tk_PhotoImageBlock	block;
	fprintf(stderr, "read_pmap enter\n");

	if (format != NULL && strcmp(Tcl_GetString(format), "pmap") == 0) {
		if (dataObj->typePtr != &tcl_pmap) return 0;
	}


	TEST_OK(Tcl_GetPMAPFromObj(interp, dataObj, &pmap));

	// Clip <<<
	if ((srcX + width) > pmap->width) {
		outWidth = pmap->width - srcX;
	} else {
		outWidth = width;
	}
	if ((srcY + height) > pmap->height) {
		outHeight = pmap->height - srcY;
	} else {
		outHeight = height;
	}

	if (
			outWidth <= 0 ||
			outHeight <= 0 ||
			srcX >= pmap->width ||
			srcY >= pmap->height
	   ) return TCL_OK;
	// Clip >>>

	Tk_PhotoGetSize(imageHandle, &old_w, &old_h);
	if (destX + outWidth > old_w || destY + outHeight > old_h) {
		Tk_PhotoExpand(imageHandle, interp, destX + outWidth, destY + outHeight);
	}

	block.pixelPtr = (unsigned char *)pmap->pixel_data;
	block.width = outWidth;
	block.height = outHeight;
	block.pitch = pmap->width * 4;
	block.pixelSize = 4;
	block.offset[2] = 0;	// B
	block.offset[1] = 1;	// G
	block.offset[0] = 2;	// R
	block.offset[3] = 3;	// A

	TEST_OK(Tk_PhotoPutBlock(interp, imageHandle, &block, destX, destY,
				outWidth, outHeight, TK_PHOTO_COMPOSITE_SET));

	fprintf(stderr, "read_pmap leave\n");
	return TCL_OK;
}

//>>>
*/
static int glue_image2pmap(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	Tk_PhotoImageBlock	block;
	gimp_image_t		*pmap;
	Tk_PhotoHandle		imageHandle;
	_pel				init;
	_pel				*o;
	unsigned char		*i;
	int					x, y, strideskip;

	CHECK_ARGS(1, "image_handle");

	imageHandle = Tk_FindPhoto(interp, Tcl_GetString(objv[1]));

	Tk_PhotoGetImage(imageHandle, &block);

	init.c = 0;
	pmap = pmap_new(block.width, block.height, init);

	if (
			block.pitch == block.width * 4 &&
			block.pixelSize == 4 &&
			block.offset[2] == 0 &&
			block.offset[1] == 1 &&
			block.offset[0] == 2 &&
			block.offset[3] == 3
	   ) {
		memcpy(pmap->pixel_data, block.pixelPtr, block.width * block.height * block.pixelSize);
	} else {
		const int	rofs = block.offset[0];
		const int	gofs = block.offset[1];
		const int	bofs = block.offset[2];
		const int	aofs = block.offset[3];

		i = block.pixelPtr;
		o = pmap->pixel_data;
		strideskip = block.pitch - (block.pixelSize * block.width);
		for (y=0; y<block.height; y++) {
			for (x=0; x<block.width; x++, i+=block.pixelSize, o++) {
				o->ch.r = i[rofs];
				o->ch.g = i[gofs];
				o->ch.b = i[bofs];
				o->ch.a = i[aofs];
			}
			i += strideskip;
		}
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));

	return TCL_OK;
}

//>>>
static int glue_violate_photo(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	gimp_image_t		*pmap;
	Tk_PhotoImageBlock	block, newblock;
	Tk_PhotoHandle		imageHandle;
	/*
	_pel				*o;
	unsigned char		*i;
	int					x, y, strideskip;
	*/

	fprintf(stderr, "tkphoto foo\n");
	CHECK_ARGS(2, "image_handle pmap");

	imageHandle = Tk_FindPhoto(interp, Tcl_GetString(objv[1]));

	Tk_PhotoGetImage(imageHandle, &block);

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap));
	TEST_OK(Tk_PhotoSetSize(interp, imageHandle, pmap->width, pmap->height));

	newblock.pixelPtr = (unsigned char *)pmap->pixel_data;
	newblock.width = pmap->width;
	newblock.height = pmap->height;
	newblock.pitch = pmap->width * 4;
	newblock.pixelSize = 4;
	newblock.offset[2] = 0;	// B
	newblock.offset[1] = 1;	// G
	newblock.offset[0] = 2;	// R
	newblock.offset[3] = 3;	// A

	fprintf(stderr, "tkphoto bar\n");
	TEST_OK(Tk_PhotoPutBlock(interp, imageHandle, &newblock, 0, 0,
				pmap->width, pmap->height, TK_PHOTO_COMPOSITE_SET));

	/*
	if (
			block.pitch == block.width * 4 &&
			block.pixelSize == 4 &&
			block.offset[2] == 0 &&
			block.offset[1] == 1 &&
			block.offset[0] == 2 &&
			block.offset[3] == 3
	   ) {
		memcpy(block.pixelPtr, pmap->pixel_data, block.width * block.height * block.pixelSize);
	} else {
		const int	rofs = block.offset[0];
		const int	gofs = block.offset[1];
		const int	bofs = block.offset[2];
		const int	aofs = block.offset[3];

		i = block.pixelPtr;
		o = pmap->pixel_data;
		strideskip = block.pitch - (block.pixelSize * block.width);
		for (y=0; y<block.height; y++) {
			for (x=0; x<block.width; x++, i+=block.pixelSize, o++) {
				i[rofs] = o->ch.r;
				i[gofs] = o->ch.g;
				i[bofs] = o->ch.b;
				i[aofs] = o->ch.a;
			}
			i += strideskip;
		}
	}
	*/

	fprintf(stderr, "tkphoto baz\n");
	return TCL_OK;
}

//>>>
static int glue_vartype(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(1, "var");

	if (objv[1]->typePtr == NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj("", 0));
	} else {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(objv[1]->typePtr->name, -1));
	}

	return TCL_OK;
}

//>>>

/*
static Tk_PhotoImageFormat format = {
	"pmap",
	(Tk_ImageFileMatchProc *)	NULL,
	(Tk_ImageStringMatchProc *)	match_pmap,
	(Tk_ImageFileReadProc *)	NULL,
	(Tk_ImageStringReadProc *)	read_pmap,
	(Tk_ImageFileWriteProc *)	NULL,
	(Tk_ImageStringWriteProc *)	NULL
};
*/

int Pixel_tkphoto_Init(Tcl_Interp *interp) //<<<
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Tk_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	//Tk_CreatePhotoImageFormat(&format);

	NEW_CMD("pixel::tkphoto::image2pmap", glue_image2pmap);
	NEW_CMD("pixel::tkphoto::vartype", glue_vartype);
	NEW_CMD("pixel::tkphoto::violate_photo", glue_violate_photo);

	/*
	if (Tcl_PkgProvide(interp, "Pixel_tkphoto", "1.0.0") != TCL_OK)
		return TCL_ERROR;
	*/

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
