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

#include <ptc/ptc.h>

#include <sys/time.h>
#include <unistd.h>
#include "timestuff.h"

#include <tcl.h>
#include <tclstuff.h>

#include "2d.h"
#include "tcl_pmap.h"

#include "main.h"
#include "widget.h"


// open_console xres yres title {{{1
static int glue_open_console(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				xres, yres;
	Format			format(32, MD_MASK_RED, MD_MASK_GREEN, MD_MASK_BLUE, MD_MASK_ALPHA);
	gimp_image_t	*scr_pmap;
	sp_info			*sp;
	Surface			*new_surface;
	Console			*new_console;
	console_inf		*new_console_inf;
	Tcl_Obj			*new_obj;
	
	CHECK_ARGS(3, "xres yres title");
	
	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &xres));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &yres));

	new_surface = new Surface(xres, yres, format);
	new_console = new Console();
	
	new_console->option("dga off");
	new_console->open(Tcl_GetString(objv[3]), xres, yres, format);
	
	scr_pmap = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	scr_pmap->width = xres;
	scr_pmap->height = yres;
	scr_pmap->bytes_per_pixel = 4;
	scr_pmap->pixel_data = (_pel *)new_surface->lock();

	new_console_inf = (console_inf *)malloc(sizeof(console_inf));
	new_console_inf->surface = new_surface;
	new_console_inf->console = new_console;

	sp = (sp_info *)malloc(sizeof(sp_info));
	sp->type = "OpenPTC Console";
	sp->info = new_console_inf;

	init_timestuff(new_console_inf);
	
	new_obj = Tcl_NewPMAPObj(scr_pmap);
	new_obj->internalRep.twoPtrValue.ptr2 = sp;
	
	Tcl_SetObjResult(interp, new_obj);
	
	return TCL_OK;
}


// do_frame scr_pmap {{{1
static int glue_do_frame(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *		pmap;
	Surface *			surface;
	Console *			console;
	sp_info *			sp;
	console_inf *		ci;
	
	CHECK_ARGS(1, "scr_pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "OpenPTC Console") != 0)
		THROW_ERROR("Specified display buffer is not an OpenPTC Console");

	ci = (console_inf *)sp->info;

	surface = ci->surface;
	console = ci->console;
	
	surface->unlock();
	surface->copy(*console);
	pmap->pixel_data = (_pel *)surface->lock();

	frame_time(ci);
	ci->frames++;
	console->update();

	return TCL_OK;
}


// gettimeofday {{{1
static int glue_gettimeofday(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	struct timeval	tv;
	int				big;
//	Tcl_WideInt		big;
	
	CHECK_ARGS(0, "");

	gettimeofday(&tv, NULL);

	big = tv.tv_usec;
	big += tv.tv_sec * 1000000;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(big));
//	Tcl_SetObjResult(interp, Tcl_NewWideIntObj(big));
	
	return TCL_OK;
}


static int glue_elapsed(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	sp_info		*sp;
	console_inf	*ci;
	
	CHECK_ARGS(1, "scr_pmap");

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "OpenPTC Console") != 0)
		THROW_ERROR("Specified display buffer is not an OpenPTC Console");

	ci = (console_inf *)sp->info;

	update_elapsed(ci);
	Tcl_SetObjResult(interp, Tcl_NewIntObj(ci->elapsed));

	return TCL_OK;
}


static int glue_frames(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	sp_info		*sp;
	console_inf	*ci;
	
	CHECK_ARGS(1, "scr_pmap");

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "OpenPTC Console") != 0)
		THROW_ERROR("Specified display buffer is not an OpenPTC Console");

	ci = (console_inf *)sp->info;

	Tcl_SetObjResult(interp, Tcl_NewLongObj(ci->frames));

	return TCL_OK;
}


static int glue_fps(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	sp_info		*sp;
	console_inf	*ci;
	
	CHECK_ARGS(1, "scr_pmap");

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "OpenPTC Console") != 0)
		THROW_ERROR("Specified display buffer is not an OpenPTC Console");

	ci = (console_inf *)sp->info;

	Tcl_SetObjResult(interp, Tcl_NewDoubleObj(ci->fps));

	return TCL_OK;
}


// Init {{{1
int Pixel_ptc_Init(Tcl_Interp *interp)
{
	NEW_CMD("pixel::ptc::open_console", glue_open_console);
	NEW_CMD("pixel::ptc::do_frame", glue_do_frame);
	NEW_CMD("pixel::ptc::gettimeofday", glue_gettimeofday);
	NEW_CMD("pixel::ptc::pixel_ptc", PTCCreateWidget);
	NEW_CMD("pixel::ptc::elapsed", glue_elapsed);
	NEW_CMD("pixel::ptc::frames", glue_frames);
	NEW_CMD("pixel::ptc::fps", glue_fps);

	/*
	Tcl_LinkVar(interp, "pixel::ptc::elapsed", (char *)&elapsed, 
			TCL_LINK_INT);
	Tcl_LinkVar(interp, "pixel::ptc::fps", (char *)&fps, 
			TCL_LINK_DOUBLE | TCL_LINK_READ_ONLY);
//	Tcl_LinkVar(interp, "pixel::ptc::frames", (char *)&frames, TCL_LINK_WIDE_INT);
	Tcl_LinkVar(interp, "pixel::ptc::frames", (char *)&frames, TCL_LINK_INT);
	*/

	return TCL_OK;
}



