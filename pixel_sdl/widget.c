// vim: foldmarker=<<<,>>> ts=4 shiftwidth=4

#include <tkPort.h>
#define __NO_OLD_CONFIG
#include <tk.h>
#include <tkInt.h>
#include <tclstuff.h>
#include <ptc/ptc.h>
#include "2d.h"
#include "tcl_pmap.h"
#include "main.h"
#include "timestuff.h"


typedef struct {
	Tk_Window		tkwin;
	Display			*display;
	Tcl_Interp		*interp;
	Tcl_Command		widgetCmd;
	Tk_OptionTable	optionTable;
	
	int				x, y;
	Tcl_Obj			*widthObjPtr;
	Tcl_Obj			*heightObjPtr;
//	Tcl_Obj			*swidthObjPtr;
//	Tcl_Obj			*sheightObjPtr;

	Tcl_Obj			*borderWidthPtr;
	Tcl_Obj			*bgBorderPtr;
	Tcl_Obj			*fgBorderPtr;
	Tcl_Obj			*reliefPtr;

	int				updatePending;

	Console			*console;
	Surface			*surface;

	Tcl_Obj			*pmap;
} PTC_widget;


// Forward ref prototypes
static int PTCWidgetObjCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
static int PTCConfigure(Tcl_Interp *interp, PTC_widget *widget);
static void PTCObjEventProc(ClientData clientData, XEvent *event);
static void PTCDeletedProc(ClientData clientData);
static void draw_console(ClientData clientData);
static void PTCDestroy(char *mem);



static Tk_OptionSpec optionSpecs[] = {
	{TK_OPTION_BORDER, "-background", "background", "Background",
		"#d9d9d9", Tk_Offset(PTC_widget, bgBorderPtr), -1, 0, 
		(ClientData) "white"},
	{TK_OPTION_SYNONYM, "-bd", (char *) NULL, (char *) NULL,
		(char *) NULL, 0, -1, 0, (ClientData) "-borderwidth"},
	{TK_OPTION_SYNONYM, "-bg", (char *) NULL, (char *) NULL,
		(char *) NULL, 0, -1 ,0, (ClientData) "-background"},
	{TK_OPTION_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
		"2", Tk_Offset(PTC_widget, borderWidthPtr), -1},
	{TK_OPTION_SYNONYM, "-fg", (char *) NULL, (char *) NULL,
		(char *) NULL, 0, -1, 0, (ClientData) "-foreground"},
	{TK_OPTION_BORDER, "-foreground", "foreground", "Foreground",
		"#b03060", Tk_Offset(PTC_widget, fgBorderPtr), -1, 0,
		(ClientData) "black"},
	{TK_OPTION_RELIEF, "-relief", "relief", "Relief", 
		"raised", Tk_Offset(PTC_widget, reliefPtr), -1},
	{TK_OPTION_PIXELS, "-width", "width", "Width",
		"320", Tk_Offset(PTC_widget, widthObjPtr), -1},
	{TK_OPTION_PIXELS, "-height", "height", "Height",
		"200", Tk_Offset(PTC_widget, heightObjPtr), -1},
//	{TK_OPTION_PIXELS, "-swidth", "width", "Width",
//		"0", Tk_Offset(PTC_widget, swidthObjPtr), -1},
//	{TK_OPTION_PIXELS, "-sheight", "height", "Height",
//		"0", Tk_Offset(PTC_widget, sheightObjPtr), -1},
	{TK_OPTION_END}
};



int PTCCreateWidget(ClientData foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	PTC_widget		*widget;
	Tk_Window		tkwin;
	Tk_OptionTable	optionTable;
	gimp_image_t	*scr_pmap;
	sp_info			*sp;
	console_inf		*new_console_inf;
	Console			*new_console;
	int				xres, yres;
//	int				sxres, syres;
	//Format			format(32, (int32)MD_MASK_RED, (int32)MD_MASK_GREEN, (int32)MD_MASK_BLUE);
	Format			format(32, MD_MASK_RED, MD_MASK_GREEN, MD_MASK_BLUE, MD_MASK_ALPHA);
	//Format			format(32);
	Window			x_window;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "pathName ?options?");
		return TCL_ERROR;
	}

	tkwin = Tk_CreateWindowFromPath(interp, Tk_MainWindow(interp),
			Tcl_GetString(objv[1]), (char *) NULL);
	Tk_MakeWindowExist(tkwin);
	//Tk_MapWindow(tkwin);

	if (tkwin == NULL)
		return TCL_ERROR;

	Tk_SetClass(tkwin, "PTC");

	optionTable = Tk_CreateOptionTable(interp, optionSpecs);

	widget = (PTC_widget *) ckalloc(sizeof(PTC_widget));
	memset((void *) widget, 0, sizeof(PTC_widget));
	widget->tkwin = tkwin;
	widget->display = Tk_Display(tkwin);
	widget->interp = interp;
	widget->widgetCmd = Tcl_CreateObjCommand(interp, 
			Tk_PathName(tkwin), PTCWidgetObjCmd,
			(ClientData) widget, PTCDeletedProc);
	widget->optionTable = optionTable;

	if (Tk_InitOptions(interp, (char *) widget, optionTable, tkwin) != TCL_OK) {
		Tk_DestroyWindow(tkwin);
		ckfree((char *) widget);
		return TCL_ERROR;
	}

	Tk_CreateEventHandler(tkwin, ExposureMask|StructureNotifyMask,
			PTCObjEventProc, (ClientData) widget);
	if (Tk_SetOptions(interp, (char *) widget, optionTable, objc - 2,
				objv + 2, tkwin, NULL, (int *) NULL) != TCL_OK) {
		goto error;
	}

	TEST_OK(Tcl_GetIntFromObj(interp, widget->widthObjPtr, &xres));
	TEST_OK(Tcl_GetIntFromObj(interp, widget->heightObjPtr, &yres));
//	TEST_OK(Tcl_GetIntFromObj(interp, widget->widthObjPtr, &sxres));
//	TEST_OK(Tcl_GetIntFromObj(interp, widget->heightObjPtr, &syres));

//	fprintf(stderr, "Creating widget: xres: (%d) yres: (%d)\n",
//			xres, yres);

//	Tk_SetMinimumRequestSize(tkwin, xres, yres);
	Tk_GeometryRequest(tkwin, xres, yres);

	new_console = new Console;
	new_console->option("dga off");
	new_console->option("leave window open");
	new_console->option("leave display open");

	x_window = Tk_WindowId(tkwin);
	if (x_window == NULL)
		THROW_ERROR("Tk_WindowId returned NULL");

	//new_console->open(widget->display, DefaultScreen(widget->display), tkwin, format, 40, 50, xres, yres); 
	new_console->open(widget->display, DefaultScreen(widget->display), x_window, format, 0, 0, xres, yres); 
//	if (sxres == 0) sxres = xres;
//	if (syres == 0) syres = yres;
//	widget->surface = new Surface(sxres, syres, format);
	widget->surface = new Surface(xres, yres, format);
	widget->console = new_console;

	scr_pmap = (gimp_image_t *)malloc(sizeof(gimp_image_t));
//	scr_pmap->width = sxres;
//	scr_pmap->height = syres;
	scr_pmap->width = xres;
	scr_pmap->height = yres;
	scr_pmap->bytes_per_pixel = 4;
	scr_pmap->pixel_data = (_pel *)widget->surface->lock();

	new_console_inf = (console_inf *)malloc(sizeof(console_inf));
	new_console_inf->surface = widget->surface;
	new_console_inf->console = widget->console;
	init_timestuff(new_console_inf);

	sp = (sp_info *)malloc(sizeof(sp_info));
	sp->type = "OpenPTC Console";
	sp->info = new_console_inf;
	
	widget->pmap = Tcl_NewPMAPObj(scr_pmap);
	widget->pmap->internalRep.twoPtrValue.ptr2 = sp;
	Tcl_IncrRefCount(widget->pmap);
	
	if (PTCConfigure(interp, widget) != TCL_OK) {
		goto error;
	}

	Tcl_SetObjResult(interp,
			Tcl_NewStringObj(Tk_PathName(widget->tkwin), -1));
	return TCL_OK;

error:
	Tk_DestroyWindow(tkwin);
	return TCL_ERROR;
}


static int PTCWidgetObjCmd(ClientData clientData, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	static CONST char *PTCOptions[] = {"cget", "configure", "update",
		"pmap", (char *) NULL};
	enum {PTC_CGET, PTC_CONFIGURE, PTC_UPDATE, PTC_PMAP};
	PTC_widget	*widget = (PTC_widget *)clientData;
	int			index;
	int			result = TCL_OK;
	Tcl_Obj		*res;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "option ?arg arg...?");
		return TCL_ERROR;
	}

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], (char **)PTCOptions,
				"command", 0, &index));

	Tcl_Preserve((ClientData) widget);

	switch (index) {
		case PTC_CGET:
			if (objc != 3) {
				Tcl_WrongNumArgs(interp, 2, objv, "option");
				goto error;
			}
			res = Tk_GetOptionValue(interp, (char *) widget,
					widget->optionTable, objv[2], widget->tkwin);
			if (res == NULL) {
				result = TCL_ERROR;
			} else {
				Tcl_SetObjResult(interp, res);
			}
			break;
			
		case PTC_CONFIGURE:
			res = NULL;
			if (objc == 2) {
				res = Tk_GetOptionInfo(interp, (char *) widget,
						widget->optionTable, (Tcl_Obj *) NULL,
						widget->tkwin);
				if (res == NULL) result = TCL_ERROR;
			} else {
				result = Tk_SetOptions(interp, (char *) widget,
						widget->optionTable, objc - 2, objv + 2,
						widget->tkwin, NULL, (int *) NULL);
				if (result == TCL_OK) {
					result = PTCConfigure(interp, widget);
				}
				if (!widget->updatePending) {
					Tcl_DoWhenIdle(draw_console, (ClientData)widget);
					widget->updatePending = 1;
				}
			}
			if (res != NULL) {
				Tcl_SetObjResult(interp, res);
			}
			break;
			
		case PTC_UPDATE:
			widget->updatePending = 1;
			draw_console((ClientData)widget);
			break;

		case PTC_PMAP:
			Tcl_SetObjResult(interp, widget->pmap);
			break;
	}
	Tcl_Release((ClientData) widget);
	return result;

error:
	Tcl_Release((ClientData) widget);
	return TCL_ERROR;
}


static int PTCConfigure(Tcl_Interp *interp, PTC_widget *widget) //<<<1
{
	// TODO:
	return TCL_OK;
}


static void PTCObjEventProc(ClientData clientData, XEvent *event) //<<<1
{
	PTC_widget		*widget = (PTC_widget *)clientData;

	if (event->type == Expose) {
		if (!widget->updatePending) {
			Tcl_DoWhenIdle(draw_console, (ClientData)widget);
			widget->updatePending = 1;
		}
	} else if (event->type == ConfigureNotify) {
		// TODO: Change console dims
		if (!widget->updatePending) {
			Tcl_DoWhenIdle(draw_console, (ClientData)widget);
			widget->updatePending = 1;
		}
	} else if (event->type == DestroyNotify) {
		if (widget->tkwin != NULL) {
			Tk_FreeConfigOptions((char *)widget, widget->optionTable,
					widget->tkwin);
			widget->tkwin = NULL;
			Tcl_DeleteCommandFromToken(widget->interp, widget->widgetCmd);
		}
		if (widget->updatePending) {
			Tcl_CancelIdleCall(draw_console, (ClientData)widget);
		}
		Tcl_EventuallyFree((ClientData)widget, PTCDestroy);
	}
}


static void PTCDeletedProc(ClientData clientData) //<<<1
{
	PTC_widget	*widget = (PTC_widget *)clientData;
	Tk_Window	tkwin = widget->tkwin;

	// TODO: Destroy Console and Surface
	
	if (tkwin != NULL) {
		Tk_DestroyWindow(tkwin);
	}
}


static void draw_console(ClientData clientData) //<<<1
{
	PTC_widget	*widget = (PTC_widget *)clientData;
	Tk_Window	tkwin = widget->tkwin;
	sp_info		*sp;
	console_inf	*ci;

	widget->updatePending = 0;
	if (!Tk_IsMapped(tkwin)) return;
	
	// TODO: Copy surface to console
//	fprintf(stderr, "Updating console start\n");
	widget->surface->unlock();
	widget->surface->copy(*(widget->console));
	widget->surface->lock();

	sp = (sp_info *)widget->pmap->internalRep.twoPtrValue.ptr2;
	ci = (console_inf *)sp->info;
	frame_time(ci);
	ci->frames++;
	widget->console->update();
//	fprintf(stderr, "Updating console end\n");
}


static void PTCDestroy(char *mem) //<<<1
{
	PTC_widget	*widget = (PTC_widget *)mem;

	ckfree((char *)widget);
}


