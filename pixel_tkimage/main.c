// vim: ts=4 shiftwidth=4 tags=../tags
//   Heavily influenced by tkImgPhoto.c in the tk source

#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#include <sys/time.h>

#include <tcl.h>
#include <tk.h>
#include <tkInt.h>
#include <tkIntXlibDecls.h>
#include <tclstuff.h>
#include <Hermes.h>

#include "2d.h"
#include "tcl_pmap.h"

#include "main.h"

#define ADD_SUBLIST_LABEL(name, list) \
	TEST_OK(Tcl_ListObjAppendElement(interp, list, \
				Tcl_NewStringObj(name, -1)));

#define ADD_SUBLIST_OBJ(obj, list) \
	TEST_OK(Tcl_ListObjAppendElement(interp, list, obj));

#define ADD_FLAG_ELEMENT(flags, flag, name, list) \
	if (flags & flag) \
		TEST_OK(Tcl_ListObjAppendElement(interp, list, \
					Tcl_NewStringObj(name, -1)));


typedef struct pmap_master {
	Tk_ImageMaster			tkMaster;
	Tcl_Interp				*interp;
	Tcl_Command				imgCmd;
	Tcl_Obj					*pmapObj;
	gimp_image_t			*pmap;
	struct pmap_instance	*instancePtr;
	int						flags;
	int						width;
	int						height;
	TkRegion				validRegion;
} pmap_master;


#define TKIMAGE_IMAGE_CHANGED	2


typedef struct pmap_instance {
	pmap_master				*masterPtr;
	Display					*display;
	Colormap				colormap;
	struct pmap_instance	*nextPtr;
	int						refCount;
	Pixmap					pixels;
	int						width, height;
	XImage					*imagePtr;
	HermesFormat			*destformat;
	XVisualInfo				visualInfo;
	GC						gc;
} pmap_instance;


/*
static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_CUSTOM, "-pmap", (char *) NULL, (char *) NULL,
	 (char *) NULL, Tk_Offset(pmap_master, pmap), TK_CONFIG_NULL_OK},
    {TK_CONFIG_INT, "-height", (char *) NULL, (char *) NULL,
	 DEF_PHOTO_HEIGHT, Tk_Offset(pmap_master, height), 0},
    {TK_CONFIG_INT, "-width", (char *) NULL, (char *) NULL,
	 DEF_PHOTO_WIDTH, Tk_Offset(pmap_master, width), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	 (char *) NULL, 0, 0}
};
*/


static HermesHandle	g_hermes_handle;
static HermesFormat *g_hermes_pmap_format;

static int createproc(Tcl_Interp *interp, char *name,
		int objc, Tcl_Obj *CONST objv[], Tk_ImageType *typePtr,
		Tk_ImageMaster master, ClientData *clientDataPtr);
static void tkimage_configure_instance(pmap_instance *instancePtr);
static int img_pmap_configure(Tcl_Interp *interp, pmap_master *masterPtr,
		int objc, Tcl_Obj *CONST objv[], int flags);
static int img_pmap_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
static void img_pmap_cmd_deleted(ClientData clientData);
static void deleteproc(ClientData clientData);
static void tkimage_instance_set_size(pmap_instance *instancePtr);


static int createproc(Tcl_Interp *interp, char *name, //{{{1
		int objc, Tcl_Obj *CONST objv[], Tk_ImageType *typePtr,
		Tk_ImageMaster master, ClientData *clientDataPtr)
{
	pmap_master		*masterPtr;

	//fprintf(stderr, "createproc\n");
	masterPtr = (pmap_master *)ckalloc(sizeof(pmap_master));
	memset((void *)masterPtr, 0, sizeof(pmap_master));
	masterPtr->tkMaster = master;
	masterPtr->interp = interp;
	masterPtr->imgCmd = Tcl_CreateObjCommand(interp, name, img_pmap_cmd,
			(ClientData)masterPtr, img_pmap_cmd_deleted);
	masterPtr->instancePtr = NULL;
	masterPtr->validRegion = TkCreateRegion();

	if (img_pmap_configure(interp, masterPtr, objc, objv, 0) != TCL_OK) {
		deleteproc((ClientData)masterPtr);
		return TCL_ERROR;
	}
	
	*clientDataPtr = (ClientData)masterPtr;
	return TCL_OK;
}


static void img_pmap_cmd_deleted(ClientData clientData) //{{{1
{
	pmap_master		*masterPtr = (pmap_master *)clientData;

	//fprintf(stderr, "img_pmap_cmd_deleted\n");
	masterPtr->imgCmd = NULL;
	if (masterPtr->tkMaster != NULL) {
		Tk_DeleteImage(masterPtr->interp, Tk_NameOfImage(masterPtr->tkMaster));
	}
}


static int img_pmap_master_set_size(pmap_master *masterPtr, int width, int height)
{
	pmap_instance		*instancePtr;

	for (instancePtr = masterPtr->instancePtr; instancePtr != NULL; instancePtr = instancePtr->nextPtr) {
		tkimage_instance_set_size(instancePtr);
	}
	
	return TCL_OK;
}


static int img_pmap_configure(Tcl_Interp *interp, pmap_master *masterPtr, //{{{1
		int objc, Tcl_Obj *CONST objv[], int flags)
{
	int				i;
	char			*tmp;
	int				width = 0;
	int				height = 0;
	pmap_instance	*instancePtr;
	XRectangle		rect;

	//fprintf(stderr, "img_pmap_configure\n");
	for (i=0; i<objc; i++) {
		tmp = Tcl_GetString(objv[i]);
		//fprintf(stderr, "Processing option word #%d\n", i);
		if (tmp[0] != '-') THROW_ERROR("Expected option, got ", tmp);
		if (i >= objc-1) THROW_ERROR("No value given for option: ", tmp);
		if (strcmp("-pmap", tmp) == 0) {
			//fprintf(stderr, "Got -pmap option, looking for value\n");
			if (masterPtr->pmapObj != NULL) {
				Tcl_DecrRefCount(masterPtr->pmapObj);
				masterPtr->pmapObj = NULL;
			}
			TEST_OK(Tcl_GetPMAPFromObj(interp, objv[i+1], &(masterPtr->pmap)));
			masterPtr->pmapObj = objv[i+1];
			Tcl_IncrRefCount(masterPtr->pmapObj);
			//fprintf(stderr, "Got value for -pmap option\n");
			i++;
		} else if (strcmp("-width", tmp) == 0) {
			//fprintf(stderr, "Got -width option, looking for value\n");
			TEST_OK(Tcl_GetIntFromObj(interp, objv[i+1], &width));
			//fprintf(stderr, "Got value for -width option\n");
			i++;
		} else if (strcmp("-height", tmp) == 0) {
			//fprintf(stderr, "Got -height option, looking for value\n");
			TEST_OK(Tcl_GetIntFromObj(interp, objv[i+1], &height));
			//fprintf(stderr, "Got value for -height option\n");
			i++;
		} else {
			THROW_ERROR("Unknown option: ", tmp);
		}
	}

	if (masterPtr->pmap != NULL) {
		if (width == 0) width = masterPtr->pmap->width;
		if (height == 0) height = masterPtr->pmap->height;
	}

	masterPtr->width = width;
	masterPtr->height = height;
	
	TEST_OK(img_pmap_master_set_size(masterPtr, masterPtr->width, masterPtr->height));
	
	for (instancePtr = masterPtr->instancePtr; instancePtr != NULL;
			instancePtr = instancePtr->nextPtr) {
		tkimage_configure_instance(instancePtr);
	}

	rect.x = 0;
	rect.y = 0;
	rect.width = masterPtr->width;
	rect.height = masterPtr->height;
	TkUnionRectWithRegion(&rect, masterPtr->validRegion, masterPtr->validRegion);
	Tk_ImageChanged(masterPtr->tkMaster, 0, 0, masterPtr->width,
			masterPtr->height, masterPtr->width, masterPtr->height);
	masterPtr->flags &= ~TKIMAGE_IMAGE_CHANGED;
	
	return TCL_OK;
}


static int img_pmap_cmd(ClientData clientData, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int				index;
	pmap_master		*masterPtr = (pmap_master *)clientData;
	static CONST char *pmap_options[] = {
		"pmap", "do_frame", "configure", (char *)NULL
	};
	enum options {
		PMAP_PMAP, PMAP_DO_FRAME, PMAP_CONFIGURE
	};
	
	//fprintf(stderr, "img_pmap_cmd\n");
	CHECK_ARGS(1, "command");

	if (Tcl_GetIndexFromObj(interp, objv[1], pmap_options, "command", 0, 
				&index) != TCL_OK)
		THROW_ERROR("Syntax error: should be ", Tcl_GetString(objv[0]), " option");

	switch ((enum options) index) {
		case PMAP_PMAP:
			Tcl_SetObjResult(interp, masterPtr->pmapObj);
			break;

		case PMAP_DO_FRAME:
			THROW_ERROR("Not supported yet");
			break;

		case PMAP_CONFIGURE:
			THROW_ERROR("Not supported yet");
			break;

		default:
			panic("Bad option index!");
	}
	
	return TCL_OK;
}


static void tkimage_instance_set_size(pmap_instance *instancePtr) //{{{1
{
	pmap_master		*masterPtr = (pmap_master *)instancePtr->masterPtr;
	Pixmap			newPixmap;
	int				mwidth, mheight;
	
	//fprintf(stderr, "img_pmap_cmd\n");
	mwidth = masterPtr->width;
	mheight = masterPtr->height;

	if (
			(instancePtr->pixels == None) 
			|| (instancePtr->width != mwidth)
			|| (instancePtr->height != mheight)
	   ) {
		newPixmap = Tk_GetPixmap(instancePtr->display,
				RootWindow(instancePtr->display,
					instancePtr->visualInfo.screen),
				(mwidth > 0) ? mwidth: 1,
				(mheight > 0) ? mheight: 1,
				instancePtr->visualInfo.depth);
		if (!newPixmap) {
			panic("Fail to create pixmap with Tk_GetPixmap in ImgPhotoInstanceSetSize.\n");
			return;
		}

		if (instancePtr->pixels != None) {
			Tk_FreePixmap(instancePtr->display, instancePtr->pixels);
		}

		instancePtr->pixels = newPixmap;
	}

	instancePtr->width = mwidth;
	instancePtr->height = mheight;
}


static void redraw(pmap_instance *instancePtr) //{{{1
{
	XImage			*imagePtr;
	gimp_image_t	*pmap = instancePtr->masterPtr->pmap;
	
	//fprintf(stderr, "redraw\n");
	imagePtr = instancePtr->imagePtr;
	if (imagePtr == NULL)
		return;

	imagePtr->width = instancePtr->width;
	imagePtr->height = instancePtr->height;
	imagePtr->bytes_per_line = ((imagePtr->bits_per_pixel * imagePtr->width + 31) >> 3) & ~3;
	imagePtr->data = (char *)ckalloc((unsigned)(imagePtr->bytes_per_line * imagePtr->height));

	//fprintf(stderr, "Hermes_ConverterCopy:\n\tsrc: %p\n\t(x, y): (%d, %d)\t(w, h): (%d, %d)\trowstride: %d\n\tdest: %p\n\t(x, y): (%d, %d)\t(w, h): (%d, %d)\trowstride: %d\n",
	//		pmap->pixel_data, 0, 0, pmap->width, pmap->height, pmap->width * 4,
	//		imagePtr->data, 0, 0, imagePtr->width, imagePtr->height, imagePtr->bytes_per_line);
	Hermes_ConverterRequest(g_hermes_handle, g_hermes_pmap_format, instancePtr->destformat);
	Hermes_ConverterCopy(g_hermes_handle,
			pmap->pixel_data, 0, 0, pmap->width, pmap->height, pmap->width * 4,
			imagePtr->data, 0, 0, imagePtr->width, imagePtr->height, imagePtr->bytes_per_line);

	XPutImage(instancePtr->display, instancePtr->pixels,
			instancePtr->gc, imagePtr, 0, 0, 0, 0,
			(unsigned)imagePtr->width, (unsigned)imagePtr->height);

	ckfree(imagePtr->data);
	imagePtr->data = NULL;
}


static void tkimage_configure_instance(pmap_instance *instancePtr) //{{{1
{
	XImage		*imagePtr;
	int			bitsPerPixel;
	XRectangle	validBox;

	//fprintf(stderr, "tkimage_configure_instance\n");
	bitsPerPixel = instancePtr->visualInfo.depth;

	if ((instancePtr->imagePtr == NULL)
			|| (instancePtr->imagePtr->bits_per_pixel != bitsPerPixel)) {
		if (instancePtr->imagePtr != NULL) {
			XFree((char *)instancePtr->imagePtr);
		}
		imagePtr = XCreateImage(instancePtr->display,
				instancePtr->visualInfo.visual, (unsigned)bitsPerPixel,
				(bitsPerPixel > 1 ? ZPixmap : XYBitmap), 0, (char *)NULL,
				1, 1, 32, 0);
		instancePtr->imagePtr = imagePtr;

		/*
		 * Determine the endianness of this machine.
		 * We create images using the local host's endianness, rather
		 * than the endianness of the server; otherwise we would have
		 * to byte-swap any 16 or 32 bit values that we store in the
		 * image in those situations where the server's endianness
		 * is different from ours.
		 *
		 * Can't we use autoconf to figure this out?
		 */

		if (imagePtr != NULL) {
			union {
				int i;
				char c[sizeof(int)];
			} kludge;

			imagePtr->bitmap_unit = sizeof(unsigned int) * NBBY;
			kludge.i = 0;
			kludge.c[0] = 1;
			imagePtr->byte_order = (kludge.i == 1) ? LSBFirst : MSBFirst;
			_XInitImageFuncPtrs(imagePtr);
		}

		DBG("Dest instance format:\n\tbpp: %d\n\trmask: %08x\n\tgmask: %08x\n\tbmask: %08x\n",
				imagePtr->bits_per_pixel,
				imagePtr->red_mask,
				imagePtr->green_mask,
				imagePtr->blue_mask);
		instancePtr->destformat = Hermes_FormatNew(
				imagePtr->bits_per_pixel,
				imagePtr->red_mask,
				imagePtr->green_mask,
				imagePtr->blue_mask,
				0, 0);
	}
	
	/*
	 * If the user has specified a width and/or height for the master
	 * which is different from our current width/height, set the size
	 * to the values specified by the user.  If we have no pixmap, we
	 * do this also, since it has the side effect of allocating a
	 * pixmap for us.
	 */

	if (
			(instancePtr->pixels == None) 
			|| (instancePtr->width != instancePtr->masterPtr->width)
			|| (instancePtr->height != instancePtr->masterPtr->height)
		) {
		tkimage_instance_set_size(instancePtr);
	}

//	if (instancePtr->masterPtr->flags & TKIMAGE_IMAGE_CHANGED) {
//		redraw(instancePtr);
//	}
	TkClipBox(instancePtr->masterPtr->validRegion, &validBox);
	//fprintf(stderr, "validbox: x: %d y: %d w: %d h: %d\n",
	//		validBox.x, validBox.y, validBox.width, validBox.height);
	redraw(instancePtr);
}


static ClientData getproc(Tk_Window tkwin, ClientData masterData) //{{{1
{
	pmap_master		*masterPtr = (pmap_master *)masterData;
	pmap_instance	*instancePtr;
	XVisualInfo		visualInfo, *visInfoPtr;
	XColor			*white, *black;
	int				numVisuals;
	XGCValues		gcValues;

	//fprintf(stderr, "getproc\n");
	instancePtr = (pmap_instance *)ckalloc(sizeof(pmap_instance));
	instancePtr->masterPtr = masterPtr;
	instancePtr->display = Tk_Display(tkwin);
	instancePtr->colormap = Tk_Colormap(tkwin);
	Tk_PreserveColormap(instancePtr->display, instancePtr->colormap);
	instancePtr->refCount = 1;
	instancePtr->pixels = None;
	instancePtr->width = 0;
	instancePtr->height = 0;
	instancePtr->imagePtr = 0;
	instancePtr->nextPtr = masterPtr->instancePtr;
	masterPtr->instancePtr = instancePtr;

	/*
	 * Obtain information about the visual and decide on the
	 * default palette.
	 */

	visualInfo.screen = Tk_ScreenNumber(tkwin);
	visualInfo.visualid = XVisualIDFromVisual(Tk_Visual(tkwin));
	visInfoPtr = XGetVisualInfo(Tk_Display(tkwin),
			VisualScreenMask | VisualIDMask, &visualInfo, &numVisuals);
	if (visInfoPtr != NULL) {
		instancePtr->visualInfo = *visInfoPtr;
		switch (visInfoPtr->class) {
			case DirectColor:
			case TrueColor:
				break;
			case PseudoColor:
			case StaticColor:
			case GrayScale:
			case StaticGray:
			default:
				Tcl_SetObjResult(masterPtr->interp, Tcl_NewStringObj("pixel::tkimage only supports truecolour displays", -1));
				Tcl_BackgroundError(masterPtr->interp);
				return NULL;
				break;
		}
		XFree((char *) visInfoPtr);

	} else {
		panic("ImgPhotoGet couldn't find visual for window");
	}

	/*
	 * Make a GC with background = black and foreground = white.
	 */

	white = Tk_GetColor(masterPtr->interp, tkwin, "white");
	black = Tk_GetColor(masterPtr->interp, tkwin, "black");
	gcValues.foreground = (white != NULL)? white->pixel:
		WhitePixelOfScreen(Tk_Screen(tkwin));
	gcValues.background = (black != NULL)? black->pixel:
		BlackPixelOfScreen(Tk_Screen(tkwin));
	gcValues.graphics_exposures = False;
	instancePtr->gc = Tk_GetGC(tkwin,
			GCForeground|GCBackground|GCGraphicsExposures, &gcValues);

	tkimage_configure_instance(instancePtr);

	if (instancePtr->nextPtr == NULL) {
		Tk_ImageChanged(masterPtr->tkMaster, 0, 0, 0, 0,
				masterPtr->width, masterPtr->height);
	}

	return (ClientData)instancePtr;
}


static void displayproc(ClientData clientData, Display *display, //{{{1
		Drawable drawable, int imageX, int imageY, int width, int height,
		int drawableX, int drawableY)
{
	pmap_instance	*instancePtr = (pmap_instance *)clientData;
	XRectangle		validBox;

	//fprintf(stderr, "displayproc\n");
	if (instancePtr->pixels == None)
		return;

	TkClipBox(instancePtr->masterPtr->validRegion, &validBox);
	//fprintf(stderr, "display validbox: x: %d y: %d w: %d h: %d\n",
	//		validBox.x, validBox.y, validBox.width, validBox.height);

	TkSetRegion(display, instancePtr->gc, instancePtr->masterPtr->validRegion);
	XSetClipOrigin(display, instancePtr->gc, drawableX - imageX,
			drawableY - imageY);
    XCopyArea(display, instancePtr->pixels, drawable, instancePtr->gc,
			imageX, imageY, (unsigned)width, (unsigned)height,
			drawableX, drawableY);
	XSetClipMask(display, instancePtr->gc, None);
	XSetClipOrigin(display, instancePtr->gc, 0, 0);
}


static void disposeinstance(ClientData clientData) //{{{1
{
	pmap_instance	*instancePtr = (pmap_instance *)clientData;
	pmap_instance	*prevPtr;

	DBG("disposeinstance\n");
	if (instancePtr->pixels != None) {
		Tk_FreePixmap(instancePtr->display, instancePtr->pixels);
	}
	if (instancePtr->gc != None) {
		Tk_FreeGC(instancePtr->display, instancePtr->gc);
	}
	if (instancePtr->imagePtr != NULL) {
		XFree((char *)instancePtr->imagePtr);
	}

	if (instancePtr->masterPtr->instancePtr == instancePtr) {
		instancePtr->masterPtr->instancePtr = instancePtr->nextPtr;
	} else {
		for (prevPtr = instancePtr->masterPtr->instancePtr;
				prevPtr->nextPtr != instancePtr; prevPtr = prevPtr->nextPtr) {
			/* Empty loop body */
		}
		prevPtr->nextPtr = instancePtr->nextPtr;
	}
    ckfree((char *) instancePtr);
}


static void freeproc(ClientData clientData, Display *display) //{{{1
{
	pmap_instance	*instancePtr = (pmap_instance *)clientData;

	//fprintf(stderr, "freeproc\n");
	instancePtr->refCount -= 1;
	DBG("instance %p refcount-- %d\n", instancePtr, instancePtr->refCount);
	if (instancePtr->refCount > 0)
		return;

	Tcl_DoWhenIdle(disposeinstance, (ClientData)instancePtr);
}


static void deleteproc(ClientData clientData) //{{{1
{
	pmap_master		*masterPtr = (pmap_master *)clientData;
	pmap_instance	*instancePtr;

	DBG("deleteproc\n");
	while ((instancePtr = masterPtr->instancePtr) != NULL) {
		if (instancePtr->refCount > 0) {
			panic("tried to delete photo image when instances still exist");
		}
		Tcl_CancelIdleCall(disposeinstance, (ClientData)instancePtr);
		disposeinstance((ClientData)instancePtr);
	}
	masterPtr->tkMaster = NULL;
	if (masterPtr->imgCmd != NULL) {
		Tcl_DeleteCommandFromToken(masterPtr->interp, masterPtr->imgCmd);
	}
	if (masterPtr->validRegion != NULL) {
		TkDestroyRegion(masterPtr->validRegion);
	}

	// TODO: Do we free the original pmap, or deref it?

//	Tk_FreeOptions(configSpecs, (char *)masterPtr, (Display *)NULL, 0);
	ckfree((char *)masterPtr);
}


// do_frame tkimage_pmap {{{1
static int glue_do_frame(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *		pmap;
	sp_info *			sp;
	
	CHECK_ARGS(1, "tkimage_pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "Tk image") != 0)
		THROW_ERROR("Specified display buffer is not a Tk image");

	//ci = (sdl_console_inf *)sp->info;

	// TODO: Update the image

	return TCL_OK;
}


// Init {{{1
static Tk_ImageType g_tkimage_pmap_type = {
	"pmap",
	createproc,
	getproc,
	displayproc,
	freeproc,
	deleteproc
};

int Pixel_tkimage_Init(Tcl_Interp *interp)
{
	if (Hermes_Init() == 0)
		THROW_ERROR("Failed to initialize Hermes");
	g_hermes_handle = Hermes_ConverterInstance(HERMES_CONVERT_DITHER);
	//fprintf(stderr, "Dest instance format:\n\tbpp: %d\n\trmask: %08x\n\tgmask: %08x\n\tbmask: %08x\n",
	//		32,
	//		MD_MASK_RED,
	//		MD_MASK_GREEN,
	//		MD_MASK_BLUE);
	g_hermes_pmap_format = Hermes_FormatNew(32,
			MD_MASK_RED, MD_MASK_GREEN, MD_MASK_BLUE, MD_MASK_ALPHA, 0);

	Tk_CreateImageType(&g_tkimage_pmap_type);
	
	NEW_CMD("pixel::tkimage::do_frame", glue_do_frame);

	return TCL_OK;
}




