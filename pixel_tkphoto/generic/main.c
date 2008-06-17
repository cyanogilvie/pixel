#include <tcl.h>
#include <tk.h>
#include <tclstuff.h>
#include <Pixel/pixel.h>

static int match_pmap(dataObj, format, widthPtr, heightPtr, interp) //<<<
	Tcl_Obj		*dataObj;
	Tcl_Obj		*format;
	int			*widthPtr;
	int			*heightPtr;
	Tcl_Interp	*interp;
{
	gimp_image_t	*pmap;

	if (format == NULL || strcmp(Tcl_GetString(format), "pmap") != 0) {
		if (dataObj->typePtr != &tcl_pmap) return 0;
	}

	if (Tcl_GetPMAPFromObj(interp, dataObj, &pmap) != TCL_OK) return 0;

	*widthPtr = pmap->width;
	*heightPtr = pmap->height;

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

	if (format == NULL || strcmp(Tcl_GetString(format), "pmap") != 0) {
		if (dataObj->typePtr != &tcl_pmap)
			THROW_ERROR("Data is not a pmap");
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

	return TCL_OK;
}

//>>>

static Tk_PhotoImageFormat format = {
	"pmap",
	(Tk_ImageFileMatchProc *)	NULL,
	(Tk_ImageStringMatchProc *)	match_pmap,
	(Tk_ImageFileReadProc *)	NULL,
	(Tk_ImageStringReadProc *)	read_pmap,
	(Tk_ImageFileWriteProc *)	NULL,
	(Tk_ImageStringWriteProc *)	NULL
};

int Pixel_tkphoto_Init(Tcl_Interp *interp) //<<<
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Tk_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;

	Tk_CreatePhotoImageFormat(&format);

	/*
	if (Tcl_PkgProvide(interp, "Pixel_tkphoto", "1.0.0") != TCL_OK)
		return TCL_ERROR;
	*/

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
