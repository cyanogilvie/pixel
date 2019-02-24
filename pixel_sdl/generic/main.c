// vim: ts=4 shiftwidth=4 tags=../tags

#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

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
enum {
	g_sdl_ev_active,
	g_sdl_ev_key,
	g_sdl_ev_motion,
	g_sdl_ev_button,
	g_sdl_ev_jaxis,
	g_sdl_ev_jball,
	g_sdl_ev_jhat,
	g_sdl_ev_jbutton,
	g_sdl_ev_resize,
	g_sdl_ev_expose,
	g_sdl_ev_quit,
	g_sdl_ev_user,
	g_sdl_ev_syswm,

	g_sdl_ev_last
};
static Tcl_Obj	*g_sdl_ev_handlers[g_sdl_ev_last];

// setup_screen xres yres bpp title flags {{{1
static int glue_setup_screen(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				i, xres, yres;
	gimp_image_t	*scr_pmap;
	sp_info			*sp;
	SDL_Surface		*new_surface;
	SDL_Surface		*new_console;
	sdl_console_inf	*new_sdl_console_inf;
	Tcl_Obj			*new_obj;
	Uint32			flags = 0;
	int				bpp, need_updaterects;
	
	if (objc < 4 || objc > 5)
		CHECK_ARGS(3, "xres yres bpp ?flags?");
	
	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &xres));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &yres));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &bpp));

	need_updaterects = 0;
	
	if (objc == 5) {
		int 	oc;
		Tcl_Obj	**ov;
		char	*flag;
		
		TEST_OK(Tcl_ListObjGetElements(interp, objv[4], &oc, &ov));

		for (i=0; i<oc; i++) {
			flag = Tcl_GetString(ov[i]);
			if (strcasecmp(flag, "SDL_SWSURFACE") == 0) {
				flags |= SDL_SWSURFACE;
			} else if (strcasecmp(flag, "SDL_HWSURFACE") == 0) {
				flags |= SDL_HWSURFACE;
			} else if (strcasecmp(flag, "SDL_ASYNCBLIT") == 0) {
				flags |= SDL_ASYNCBLIT;
			} else if (strcasecmp(flag, "SDL_ANYFORMAT") == 0) {
				flags |= SDL_ANYFORMAT;
			} else if (strcasecmp(flag, "SDL_HWPALETTE") == 0) {
				flags |= SDL_HWPALETTE;
			} else if (strcasecmp(flag, "SDL_DOUBLEBUF") == 0) {
				flags |= SDL_DOUBLEBUF;
			} else if (strcasecmp(flag, "SDL_FULLSCREEN") == 0) {
				flags |= SDL_FULLSCREEN;
			} else if (strcasecmp(flag, "SDL_OPENGL") == 0) {
				int ch_size = (bpp == 24 || bpp == 32) ? 8 : 5;
				int al_size = (bpp == 24 || bpp == 32) ? 8 : 0;
				SDL_GL_SetAttribute(SDL_GL_RED_SIZE, ch_size);
				SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, ch_size);
				SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, ch_size);
				SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, al_size);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
				need_updaterects = 1;
				flags |= SDL_OPENGL;
			} else if (strcasecmp(flag, "SDL_OPENGLBLIT") == 0) {
				need_updaterects = 1;
				flags |= SDL_OPENGLBLIT;
			} else if (strcasecmp(flag, "SDL_RESIZABLE") == 0) {
				flags |= SDL_RESIZABLE;
			} else if (strcasecmp(flag, "SDL_NOFRAME") == 0) {
				flags |= SDL_NOFRAME;
			} else {
				THROW_ERROR("Unrecognised flag: ", flag);
			}
		}
	}
	
	new_console = SDL_SetVideoMode(xres, yres, bpp, flags);
//	new_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, xres, yres, 32, MD_MASK_RED, MD_MASK_GREEN, MD_MASK_BLUE, 0);
	new_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, xres, yres, 32, MD_MASK_RED, MD_MASK_GREEN, MD_MASK_BLUE, 0);
	
	new_sdl_console_inf = (sdl_console_inf *)malloc(sizeof(sdl_console_inf));
	new_sdl_console_inf->surface = new_surface;
	new_sdl_console_inf->console = new_console;
	new_sdl_console_inf->need_updaterects = need_updaterects;

	scr_pmap = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	scr_pmap->width = xres;
	scr_pmap->height = yres;
	scr_pmap->bytes_per_pixel = 4;

	if (new_surface->pitch != new_surface->w * 4 || SDL_MUSTLOCK(new_surface)) {
		new_sdl_console_inf->prebuffer = scr_pmap;
		scr_pmap->pixel_data = (_pel *)malloc(xres * yres * 4);
	} else {
		new_sdl_console_inf->prebuffer = NULL;
		scr_pmap->pixel_data = (_pel *)new_surface->pixels;
	}

	sp = (sp_info *)malloc(sizeof(sp_info));
	sp->type = "SDL Screen";
	sp->info = new_sdl_console_inf;

	sdl_init_timestuff(new_sdl_console_inf);
	
	new_obj = Tcl_NewPMAPObj(scr_pmap);
	new_obj->internalRep.twoPtrValue.ptr2 = sp;
	
	Tcl_SetObjResult(interp, new_obj);
	
	return TCL_OK;
}


// get_caps sdl_pmap {{{1
static int glue_get_caps(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *		pmap;
	SDL_Surface *		surface;
	SDL_Surface *		console;
	sp_info *			sp;
	sdl_console_inf *	ci;
	Tcl_Obj	*			res;
	Tcl_Obj	*			sublist1;
	Tcl_Obj	*			sublist2;
	Uint32				fl;
	int					i;
	SDL_Surface *		surf;
	char *				label;
	
	CHECK_ARGS(1, "sdl_pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "SDL Screen") != 0)
		THROW_ERROR("Specified display buffer is not an SDL Screen");

	ci = (sdl_console_inf *)sp->info;

	surface = ci->surface;
	console = ci->console;
	
	res = Tcl_NewListObj(0, NULL);
	
	for (i=0; i<2; i++) {
		if (i==0) {
			surf = console;
			fl = surf->flags;
			label = "screen";
		} else {
			surf = surface;
			fl = surf->flags;
			label = "surface";
		}
		ADD_SUBLIST_LABEL(label, res);
	
		sublist1 = Tcl_NewListObj(0, NULL);

		// flags
		ADD_SUBLIST_LABEL("flags", sublist1);
		sublist2 = Tcl_NewListObj(0, NULL);
		ADD_FLAG_ELEMENT(fl, SDL_SWSURFACE,   "SDL_SWSURFACE",   sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_HWSURFACE,   "SDL_HWSURFACE",   sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_ASYNCBLIT,   "SDL_ASYNCBLIT",   sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_ANYFORMAT,   "SDL_ANYFORMAT",   sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_HWPALETTE,   "SDL_HWPALETTE",   sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_DOUBLEBUF,   "SDL_DOUBLEBUF",   sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_FULLSCREEN,  "SDL_FULLSCREEN",  sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_OPENGL,      "SDL_OPENGL",      sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_OPENGLBLIT,  "SDL_OPENGLBLIT",  sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_RESIZABLE,   "SDL_RESIZABLE",   sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_HWACCEL,     "SDL_HWACCEL",     sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_SRCCOLORKEY, "SDL_SRCCOLORKEY", sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_RLEACCEL,    "SDL_RLEACCEL",    sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_SRCALPHA,    "SDL_SRCALPHA",    sublist2);
		ADD_FLAG_ELEMENT(fl, SDL_PREALLOC,    "SDL_PREALLOC",    sublist2);
		TEST_OK(Tcl_ListObjAppendElement(interp, sublist1, sublist2));

		// format
		ADD_SUBLIST_LABEL("format", sublist1);
		sublist2 = Tcl_NewListObj(0, NULL);
		ADD_SUBLIST_LABEL("bpp", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->BitsPerPixel), sublist2);
		ADD_SUBLIST_LABEL("rmask", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Rmask), sublist2);
		ADD_SUBLIST_LABEL("gmask", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Gmask), sublist2);
		ADD_SUBLIST_LABEL("bmask", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Bmask), sublist2);
		ADD_SUBLIST_LABEL("amask", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Amask), sublist2);
		ADD_SUBLIST_LABEL("rshift", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Rshift), sublist2);
		ADD_SUBLIST_LABEL("gshift", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Gshift), sublist2);
		ADD_SUBLIST_LABEL("bshift", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Bshift), sublist2);
		ADD_SUBLIST_LABEL("ashift", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Ashift), sublist2);
		ADD_SUBLIST_LABEL("rloss", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Rloss), sublist2);
		ADD_SUBLIST_LABEL("gloss", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Gloss), sublist2);
		ADD_SUBLIST_LABEL("bloss", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Bloss), sublist2);
		ADD_SUBLIST_LABEL("aloss", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->Aloss), sublist2);
		ADD_SUBLIST_LABEL("colorkey", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->colorkey), sublist2);
		ADD_SUBLIST_LABEL("alpha", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->format->alpha), sublist2);
		TEST_OK(Tcl_ListObjAppendElement(interp, sublist1, sublist2));

		// width and height
		ADD_SUBLIST_LABEL("width", sublist1);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->w), sublist1);
		ADD_SUBLIST_LABEL("height", sublist1);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->h), sublist1);

		// pitch
		ADD_SUBLIST_LABEL("pitch", sublist1);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->pitch), sublist1);

		// clipping rectangle
		ADD_SUBLIST_LABEL("clip_rect", sublist1);
		sublist2 = Tcl_NewListObj(0, NULL);
		ADD_SUBLIST_LABEL("x", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->clip_rect.x), sublist2);
		ADD_SUBLIST_LABEL("y", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->clip_rect.y), sublist2);
		ADD_SUBLIST_LABEL("w", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->clip_rect.w), sublist2);
		ADD_SUBLIST_LABEL("h", sublist2);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->clip_rect.h), sublist2);
		TEST_OK(Tcl_ListObjAppendElement(interp, sublist1, sublist2));
		
		// refcount
		ADD_SUBLIST_LABEL("refcount", sublist1);
		ADD_SUBLIST_OBJ(Tcl_NewIntObj(surf->refcount), sublist1);

		TEST_OK(Tcl_ListObjAppendElement(interp, res, sublist1));
	}

	ADD_SUBLIST_LABEL("general", res);
	sublist1 = Tcl_NewListObj(0, NULL);
	ADD_SUBLIST_LABEL("prebuffer", sublist1);
	ADD_SUBLIST_OBJ(Tcl_NewBooleanObj(
				surface->pitch != surface->w * 4 
				|| SDL_MUSTLOCK(surface)), sublist1);
	ADD_SUBLIST_LABEL("pitch_w_mismatch", sublist1);
	ADD_SUBLIST_OBJ(Tcl_NewBooleanObj(
				surface->pitch != surface->w * 4), sublist1);
	ADD_SUBLIST_LABEL("mustlock", sublist1);
	ADD_SUBLIST_OBJ(Tcl_NewBooleanObj(SDL_MUSTLOCK(surface)), sublist1);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, sublist1));
	
	Tcl_SetObjResult(interp, res);
	
	return TCL_OK;
}


// do_frame sdl_pmap ?rects? {{{1
static int glue_do_frame(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *		pmap;
	SDL_Surface *		surface;
	SDL_Surface *		console;
	sp_info *			sp;
	sdl_console_inf *	ci;
	int					rectsc = 0;
	Tcl_Obj				**rectsv;
	SDL_Rect			*rect = NULL;
	int					i;
	Tcl_Obj				**c;
	int					l;
	
	if (objc < 2 || objc > 3)
		CHECK_ARGS(1, "sdl_pmap ?update_rects?");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	if (objc == 3)
		TEST_OK(Tcl_ListObjGetElements(interp, objv[2], &rectsc, &rectsv));

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "SDL Screen") != 0)
		THROW_ERROR("Specified display buffer is not an SDL Screen");

	ci = (sdl_console_inf *)sp->info;

	surface = ci->surface;
	console = ci->console;
	
	if (ci->prebuffer != NULL) {
		if (SDL_MUSTLOCK(surface))
			SDL_LockSurface(surface);
		
		if (surface->w * 4 == surface->pitch) {
			memcpy(surface->pixels, ci->prebuffer->pixel_data, 
					surface->w * surface->h * 4);
		} else {
			//int		line = 0;
			_pel	*s = ci->prebuffer->pixel_data;
			_pel	*d = surface->pixels;
			int		w = ci->prebuffer->width;
			int		l = 0;
			
			//for (line = 0; line < surface->h; line++, s+=w, d += surface->pitch)
			//	memcpy(d, s, w);
			for (l = 0; l < surface->h; l++, s+=w, d += surface->pitch)
				memcpy(d, s, w);
		}

		if (SDL_MUSTLOCK(surface))
			SDL_UnlockSurface(surface);
	}
	if (rectsc > 0) {
		rect = (SDL_Rect *)malloc(sizeof(SDL_Rect) * rectsc);
		for (i=0; i<rectsc; i++) {
			int x,y,w,h;
			TEST_OK(Tcl_ListObjGetElements(interp, rectsv[i], &l, &c));
			TEST_OK(Tcl_GetIntFromObj(interp, c[0], &x));
			TEST_OK(Tcl_GetIntFromObj(interp, c[1], &y));
			TEST_OK(Tcl_GetIntFromObj(interp, c[2], &w));
			TEST_OK(Tcl_GetIntFromObj(interp, c[3], &h));
			rect[i].x = x;
			rect[i].y = y;
			rect[i].w = w;
			rect[i].h = h;
			if (x+w > pmap->width || y+h > pmap->height) {
				free(rect);
				THROW_ERROR("Rectangle ", Tcl_GetString(Tcl_NewIntObj(i)), " out of bounds");
			}
			//fprintf(stderr, "sdl do_frame rect %d %d %d %d\n",
			//		x, y, w, h);

			SDL_BlitSurface(surface, &rect[i], console, &rect[i]);
		}
	} else {
		SDL_BlitSurface(surface, NULL, console, NULL);
	}
	
	sdl_frame_time(ci);
	ci->frames++;
	if (rectsc == 0) {
		if (ci->need_updaterects) {
			SDL_Rect	r;
			r.x = 0;
			r.y = 0;
			r.w = console->w;
			r.h = console->h;
			SDL_UpdateRects(console, 1, &r);
			//fprintf(stderr, "Trivial updaterects\n");
		} else {
			SDL_UpdateRect(console, 0, 0, 0, 0);
			//fprintf(stderr, "No updaterects\n");
		}
	} else {
		//fprintf(stderr, "Fancy updaterects\n");
		SDL_UpdateRects(console, rectsc, rect);
	}

	if (rectsc > 0)
		free(rect);
	
	return TCL_OK;
}


// gettimeofday {{{1
static int glue_gettimeofday(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				big;
//	Tcl_WideInt		big;
	
	CHECK_ARGS(0, "");

	big = SDL_GetTicks();

	big = big * 1000;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(big));
//	Tcl_SetObjResult(interp, Tcl_NewWideIntObj(big));
	
	return TCL_OK;
}


// getticks {{{1
static int glue_getticks(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	unsigned int	big;
//	Tcl_WideInt		big;
	
	CHECK_ARGS(0, "");

	big = SDL_GetTicks();

	Tcl_SetObjResult(interp, Tcl_NewIntObj(big));
//	Tcl_SetObjResult(interp, Tcl_NewWideIntObj(big));
	
	return TCL_OK;
}


static int glue_elapsed(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	sp_info		*sp;
	sdl_console_inf	*ci;
	
	CHECK_ARGS(1, "scr_pmap");

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "SDL Screen") != 0)
		THROW_ERROR("Specified display buffer is not an SDL Screen");

	ci = (sdl_console_inf *)sp->info;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(ci->elapsed));

	return TCL_OK;
}


static int glue_frames(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	sp_info		*sp;
	sdl_console_inf	*ci;
	
	CHECK_ARGS(1, "scr_pmap");

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "SDL Screen") != 0)
		THROW_ERROR("Specified display buffer is not an SDL Screen");

	ci = (sdl_console_inf *)sp->info;

	Tcl_SetObjResult(interp, Tcl_NewLongObj(ci->frames));

	return TCL_OK;
}


static int glue_fps(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	sp_info		*sp;
	sdl_console_inf	*ci;
	
	CHECK_ARGS(1, "scr_pmap");

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "SDL Screen") != 0)
		THROW_ERROR("Specified display buffer is not an SDL Screen");

	ci = (sdl_console_inf *)sp->info;

	Tcl_SetObjResult(interp, Tcl_NewDoubleObj(ci->fps));

	return TCL_OK;
}


static int glue_bind_events(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	char	*class;
	int		type, len;
	
	CHECK_ARGS(2, "class handler");

	class = Tcl_GetString(objv[1]);
	
	if (strcasecmp(class, "active") == 0) {
		type = g_sdl_ev_active;
	} else if (strcasecmp(class, "key") == 0) {
		type = g_sdl_ev_key;
	} else if (strcasecmp(class, "motion") == 0) {
		type = g_sdl_ev_motion;
	} else if (strcasecmp(class, "button") == 0) {
		type = g_sdl_ev_button;
	} else if (strcasecmp(class, "jaxis") == 0) {
		type = g_sdl_ev_jaxis;
	} else if (strcasecmp(class, "jball") == 0) {
		type = g_sdl_ev_jball;
	} else if (strcasecmp(class, "jhat") == 0) {
		type = g_sdl_ev_jhat;
	} else if (strcasecmp(class, "jbutton") == 0) {
		type = g_sdl_ev_jbutton;
	} else if (strcasecmp(class, "resize") == 0) {
		type = g_sdl_ev_resize;
	} else if (strcasecmp(class, "expose") == 0) {
		type = g_sdl_ev_expose;
	} else if (strcasecmp(class, "quit") == 0) {
		type = g_sdl_ev_quit;
	} else if (strcasecmp(class, "user") == 0) {
		type = g_sdl_ev_user;
	} else if (strcasecmp(class, "syswm") == 0) {
		type = g_sdl_ev_syswm;
	} else {
		THROW_ERROR("Unrecognised class: ", class);
	}

	if (g_sdl_ev_handlers[type] != NULL) {
		Tcl_DecrRefCount(g_sdl_ev_handlers[type]);
		g_sdl_ev_handlers[type] = NULL;
	}

	// If the handler was {}, just deregister the existing one
	Tcl_GetStringFromObj(objv[2], &len);
	if (len != 0) {
		g_sdl_ev_handlers[type] = objv[2];
		Tcl_IncrRefCount(g_sdl_ev_handlers[type]);
	}
	
	return TCL_OK;
}


static int construct_sdl_ev_list(Tcl_Interp *interp, //{{{1
		SDL_Event *event, Tcl_Obj **res)
{
	int			scratch;

	*res = Tcl_NewListObj(0, NULL);

	switch (event->type) {
		case SDL_ACTIVEEVENT:
			ADD_SUBLIST_LABEL("gain", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->active.gain), *res);
			ADD_SUBLIST_LABEL("state", *res);
			switch (event->active.state) {
				case SDL_APPMOUSEFOCUS:
					ADD_SUBLIST_LABEL("SDL_APPMOUSEFOCUS", *res);
					break;
				case SDL_APPINPUTFOCUS:
					ADD_SUBLIST_LABEL("SDL_APPINPUTFOCUS", *res);
					break;
				case SDL_APPACTIVE:
					ADD_SUBLIST_LABEL("SDL_APPACTIVE", *res);
					break;
				default:
					ADD_SUBLIST_LABEL("unknown", *res);
			}
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			ADD_SUBLIST_LABEL("state", *res);
			switch (event->key.state) {
				case SDL_PRESSED:
					ADD_SUBLIST_LABEL("SDL_PRESSED", *res);
					break;
				case SDL_RELEASED:
					ADD_SUBLIST_LABEL("SDL_RELEASED", *res);
					break;
				default:
					ADD_SUBLIST_LABEL("unknown", *res);
			}
			ADD_SUBLIST_LABEL("scancode", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->key.keysym.scancode), *res);
			ADD_SUBLIST_LABEL("sym", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->key.keysym.sym), *res);
			ADD_SUBLIST_LABEL("ascii", *res);
			ADD_SUBLIST_OBJ(Tcl_NewStringObj((char *)&event->key.keysym.sym, 1), *res);
			ADD_SUBLIST_LABEL("keyname", *res);
			ADD_SUBLIST_OBJ(Tcl_NewStringObj(SDL_GetKeyName(event->key.keysym.sym), -1), *res);
			ADD_SUBLIST_LABEL("mod", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->key.keysym.mod), *res);
			ADD_SUBLIST_LABEL("unicode", *res);
			if (event->key.keysym.unicode != 0) {
				ADD_SUBLIST_OBJ(Tcl_NewUnicodeObj(&event->key.keysym.unicode,1), *res);
			} else {
				ADD_SUBLIST_OBJ(Tcl_NewStringObj("", 0), *res);
			}
			break;

		case SDL_MOUSEMOTION:
			ADD_SUBLIST_LABEL("state", *res);
			scratch = 0;
			scratch += (SDL_BUTTON(1) & event->motion.state) ? 1 : 0;
			scratch += (SDL_BUTTON(2) & event->motion.state) ? 2 : 0;
			scratch += (SDL_BUTTON(3) & event->motion.state) ? 4 : 0;
			scratch += (SDL_BUTTON(4) & event->motion.state) ? 8 : 0;
			scratch += (SDL_BUTTON(5) & event->motion.state) ? 16 : 0;
			scratch += (SDL_BUTTON(6) & event->motion.state) ? 32 : 0;
			scratch += (SDL_BUTTON(7) & event->motion.state) ? 64 : 0;
			scratch += (SDL_BUTTON(8) & event->motion.state) ? 128 : 0;
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(scratch), *res);
			ADD_SUBLIST_LABEL("x", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->motion.x), *res);
			ADD_SUBLIST_LABEL("y", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->motion.y), *res);
			ADD_SUBLIST_LABEL("xrel", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->motion.xrel), *res);
			ADD_SUBLIST_LABEL("yrel", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->motion.yrel), *res);
			break;
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			ADD_SUBLIST_LABEL("button", *res);
			switch (event->button.button) {
				case SDL_BUTTON_LEFT:
					ADD_SUBLIST_LABEL("SDL_BUTTON_LEFT", *res);
					break;
				case SDL_BUTTON_MIDDLE:
					ADD_SUBLIST_LABEL("SDL_BUTTON_MIDDLE", *res);
					break;
				case SDL_BUTTON_RIGHT:
					ADD_SUBLIST_LABEL("SDL_BUTTON_RIGHT", *res);
					break;
				default:
					ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->button.button), *res);
			}
			ADD_SUBLIST_LABEL("state", *res);
			switch (event->button.state) {
				case SDL_PRESSED:
					ADD_SUBLIST_LABEL("SDL_PRESSED", *res);
					break;
				case SDL_RELEASED:
					ADD_SUBLIST_LABEL("SDL_RELEASED", *res);
					break;
				default:
					ADD_SUBLIST_LABEL("unknown", *res);
			}
			ADD_SUBLIST_LABEL("x", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->button.x), *res);
			ADD_SUBLIST_LABEL("y", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->button.y), *res);
			break;

		case SDL_JOYAXISMOTION:
			ADD_SUBLIST_LABEL("which", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jaxis.which), *res);
			ADD_SUBLIST_LABEL("axis", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jaxis.axis), *res);
			ADD_SUBLIST_LABEL("value", *res);
			ADD_SUBLIST_OBJ(Tcl_NewDoubleObj(event->jaxis.value / 32767.0), *res);
			break;
		case SDL_JOYBALLMOTION:
			ADD_SUBLIST_LABEL("which", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jball.which), *res);
			ADD_SUBLIST_LABEL("ball", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jball.ball), *res);
			ADD_SUBLIST_LABEL("xrel", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jball.xrel), *res);
			ADD_SUBLIST_LABEL("yrel", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jball.yrel), *res);
			break;
		case SDL_JOYHATMOTION:
			ADD_SUBLIST_LABEL("which", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jhat.which), *res);
			ADD_SUBLIST_LABEL("hat", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jhat.hat), *res);
			ADD_SUBLIST_LABEL("value", *res);
			if (event->jhat.value && SDL_HAT_RIGHTUP) {
				ADD_SUBLIST_LABEL("SDL_HAT_RIGHTUP", *res);
			} else if (event->jhat.value && SDL_HAT_RIGHTDOWN) {
				ADD_SUBLIST_LABEL("SDL_HAT_RIGHTDOWN", *res);
			} else if (event->jhat.value && SDL_HAT_LEFTUP) {
				ADD_SUBLIST_LABEL("SDL_HAT_LEFTUP", *res);
			} else if (event->jhat.value && SDL_HAT_LEFTDOWN) {
				ADD_SUBLIST_LABEL("SDL_HAT_LEFTDOWN", *res);
			} else if (event->jhat.value && SDL_HAT_CENTERED) {
				ADD_SUBLIST_LABEL("SDL_HAT_CENTERED", *res);
			} else if (event->jhat.value && SDL_HAT_UP) {
				ADD_SUBLIST_LABEL("SDL_HAT_UP", *res);
			} else if (event->jhat.value && SDL_HAT_RIGHT) {
				ADD_SUBLIST_LABEL("SDL_HAT_RIGHT", *res);
			} else if (event->jhat.value && SDL_HAT_DOWN) {
				ADD_SUBLIST_LABEL("SDL_HAT_DOWN", *res);
			} else if (event->jhat.value && SDL_HAT_LEFT) {
				ADD_SUBLIST_LABEL("SDL_HAT_LEFT", *res);
			} else {
				ADD_SUBLIST_LABEL("unknown", *res);
			}
			break;
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
			ADD_SUBLIST_LABEL("which", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jbutton.which), *res);
			ADD_SUBLIST_LABEL("button", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->jbutton.button), *res);
			ADD_SUBLIST_LABEL("state", *res);
			switch (event->jbutton.state) {
				case SDL_PRESSED:
					ADD_SUBLIST_LABEL("SDL_PRESSED", *res);
					break;
				case SDL_RELEASED:
					ADD_SUBLIST_LABEL("SDL_RELEASED", *res);
					break;
				default:
					ADD_SUBLIST_LABEL("unknown", *res);
			}
			break;

		case SDL_QUIT:
			break;
		case SDL_SYSWMEVENT:
			break;
		case SDL_VIDEORESIZE:
			ADD_SUBLIST_LABEL("w", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->resize.w), *res);
			ADD_SUBLIST_LABEL("h", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->resize.h), *res);
			break;
		case SDL_VIDEOEXPOSE:
			break;
		case SDL_USEREVENT:
			ADD_SUBLIST_LABEL("code", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj(event->user.code), *res);
			ADD_SUBLIST_LABEL("data1", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj((int)event->user.data1), *res);
			ADD_SUBLIST_LABEL("data2", *res);
			ADD_SUBLIST_OBJ(Tcl_NewIntObj((int)event->user.data2), *res);
			break;

		default:
			break;
	}

	return TCL_OK;
}


static int glue_dispatch_events(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	SDL_Event	event;
	int			type, i;
	Tcl_Obj		*o[4];
	Tcl_Obj		*tmp;
	char		*evname;
	
	CHECK_ARGS(0, "");

	while (SDL_PollEvent(&event)) {
		evname = NULL;
		switch (event.type) {
			case SDL_ACTIVEEVENT:
				if (evname == NULL) evname="SDL_ACTIVEEVENT";
				type = g_sdl_ev_active; break;

			case SDL_KEYDOWN:
				if (evname == NULL) evname="SDL_KEYDOWN";
			case SDL_KEYUP:
				if (evname == NULL) evname="SDL_KEYUP";
				type = g_sdl_ev_key; break;

			case SDL_MOUSEMOTION:
				if (evname == NULL) evname="SDL_MOUSEMOTION";
				type = g_sdl_ev_motion; break;
			case SDL_MOUSEBUTTONUP:
				if (evname == NULL) evname="SDL_MOUSEBUTTONUP";
			case SDL_MOUSEBUTTONDOWN:
				if (evname == NULL) evname="SDL_MOUSEBUTTONDOWN";
				type = g_sdl_ev_button; break;
				
			case SDL_JOYAXISMOTION:
				if (evname == NULL) evname="SDL_JOYAXISMOTION";
				type = g_sdl_ev_jaxis; break;
			case SDL_JOYBALLMOTION:
				if (evname == NULL) evname="SDL_JOYBALLMOTION";
				type = g_sdl_ev_jball; break;
			case SDL_JOYHATMOTION:
				if (evname == NULL) evname="SDL_JOYHATMOTION";
				type = g_sdl_ev_jhat; break;
			case SDL_JOYBUTTONUP:
				if (evname == NULL) evname="SDL_JOYBUTTONUP";
			case SDL_JOYBUTTONDOWN:
				if (evname == NULL) evname="SDL_JOYBUTTONDOWN";
				type = g_sdl_ev_jbutton; break;

			case SDL_QUIT:
				if (evname == NULL) evname="SDL_QUIT";
				type = g_sdl_ev_quit; break;
			case SDL_SYSWMEVENT:
				if (evname == NULL) evname="SDL_SYSWMEVENT";
				type = g_sdl_ev_syswm; break;
			case SDL_VIDEORESIZE:
				if (evname == NULL) evname="SDL_VIDEORESIZE";
				type = g_sdl_ev_resize; break;
			case SDL_VIDEOEXPOSE:
				if (evname == NULL) evname="SDL_VIDEOEXPOSE";
				type = g_sdl_ev_expose; break;
			case SDL_USEREVENT:
				if (evname == NULL) evname="SDL_USEREVENT";
				type = g_sdl_ev_user; break;
				
			default:
				fprintf(stderr, "Unknown event type: %x\n", event.type);
				continue;
				break;
		}

		if (g_sdl_ev_handlers[type] == NULL) continue;

		o[0] = Tcl_NewStringObj("uplevel", -1);
		o[1] = Tcl_NewStringObj("#0", -1);
		o[2] = g_sdl_ev_handlers[type];
		o[3] = Tcl_NewListObj(0, NULL);
		Tcl_ListObjAppendElement(interp, o[3], Tcl_NewStringObj(evname, -1));
		TEST_OK(construct_sdl_ev_list(interp, &event, &tmp));
		Tcl_ListObjAppendElement(interp, o[3], tmp);
		for (i=0; i<4; i++)
			Tcl_IncrRefCount(o[i]);

		if (Tcl_EvalObjv(interp, 4, o, TCL_EVAL_GLOBAL) == TCL_ERROR)
			Tcl_BackgroundError(interp);
		
		for (i=0; i<4; i++)
			Tcl_DecrRefCount(o[i]);
	}

	return TCL_OK;
}


static int glue_toggle_fullscreen(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *		pmap;
	SDL_Surface *		console;
	sp_info *			sp;
	sdl_console_inf *	ci;
	
	CHECK_ARGS(1, "sdl_pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

	sp = (sp_info *)objv[1]->internalRep.twoPtrValue.ptr2;
	
	if (sp == NULL)
		THROW_ERROR("Specified pmap is not a display buffer");

	if (strcmp(sp->type, "SDL Screen") != 0)
		THROW_ERROR("Specified display buffer is not an SDL Screen");

	ci = (sdl_console_inf *)sp->info;

	console = ci->console;
	
	Tcl_SetObjResult(interp, Tcl_NewIntObj(SDL_WM_ToggleFullScreen(console)));
	
	return TCL_OK;
}


static int glue_show_cursor(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int		vis;
	
	CHECK_ARGS(1, "visible?");

	TEST_OK(Tcl_GetBooleanFromObj(interp, objv[1], &vis));

	SDL_ShowCursor((vis) ? SDL_ENABLE : SDL_DISABLE);

	return TCL_OK;
}


static int glue_numjoysticks(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	CHECK_ARGS(0, "");

	Tcl_SetObjResult(interp, Tcl_NewIntObj(SDL_NumJoysticks()));

	return TCL_OK;
}


static int glue_joystickname(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int		idx;

	CHECK_ARGS(1, "index");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &idx));

	Tcl_SetObjResult(interp, Tcl_NewStringObj(SDL_JoystickName(idx), -1));
	
	return TCL_OK;
}


static int glue_joystickopen(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int				idx;
	SDL_Joystick	*joy;
	Tcl_Obj			*res;

	CHECK_ARGS(1, "index");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &idx));

	joy = SDL_JoystickOpen(idx);
	if (joy == NULL)
		THROW_ERROR("Cannot open joystick");

	res = Tcl_NewListObj(0, NULL);

	ADD_SUBLIST_LABEL("name", res);
	ADD_SUBLIST_OBJ(Tcl_NewStringObj(SDL_JoystickName(idx), -1), res);

	ADD_SUBLIST_LABEL("axes", res);
	ADD_SUBLIST_OBJ(Tcl_NewIntObj(SDL_JoystickNumAxes(joy)), res);
	
	ADD_SUBLIST_LABEL("balls", res);
	ADD_SUBLIST_OBJ(Tcl_NewIntObj(SDL_JoystickNumBalls(joy)), res);
	
	ADD_SUBLIST_LABEL("hats", res);
	ADD_SUBLIST_OBJ(Tcl_NewIntObj(SDL_JoystickNumHats(joy)), res);
	
	ADD_SUBLIST_LABEL("buttons", res);
	ADD_SUBLIST_OBJ(Tcl_NewIntObj(SDL_JoystickNumButtons(joy)), res);

	Tcl_SetObjResult(interp, res);
	
	return TCL_OK;
}


static int glue_wm_grabinput(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int					i;
	SDL_GrabMode		mode, omode;
	
	if (objc >= 3) {
		CHECK_ARGS(1, "mode");
	}


	if (objc == 1) {
		mode = SDL_GRAB_QUERY;
	} else {
		TEST_OK(Tcl_GetBooleanFromObj(interp, objv[1], &i));
		if (i) {
			mode = SDL_GRAB_ON;
		} else {
			mode = SDL_GRAB_OFF;
		}
	}
	
	omode = SDL_WM_GrabInput(mode);

	if (mode == SDL_GRAB_QUERY) {
		switch (omode) {
			case SDL_GRAB_OFF:
				Tcl_SetObjResult(interp, Tcl_NewBooleanObj(0));
				break;

			case SDL_GRAB_ON:
				Tcl_SetObjResult(interp, Tcl_NewBooleanObj(1));
				break;

			default:
				THROW_ERROR("Unknown mode returned");
				break;
		}
	}
	
	return TCL_OK;
}


static int glue_gl_swapbuffers(cdata, interp, objc, objv) //{{{1
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(0, "");

	SDL_GL_SwapBuffers();

	return TCL_OK;
}


static void cleanup(ClientData cdata) //{{{1
{
	SDL_Quit();
}


// Init {{{1
int Pixel_sdl_Init(Tcl_Interp *interp)
{
	int i;

	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) return TCL_ERROR;
#ifdef USE_PIXEL_STUBS
	if (Pixel_InitStubs(interp, "3.4", 0) == NULL) return TCL_ERROR;
#endif

	if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE) < 0)
		THROW_ERROR("Couldn't initialize SDL", SDL_GetError());

	Tcl_CreateExitHandler(cleanup, NULL);
	//atexit(SDL_Quit);

	SDL_JoystickEventState(1);
	SDL_EnableUNICODE(1);
	SDL_JoystickEventState(1);

	for (i=0; i<g_sdl_ev_last; i++)
		g_sdl_ev_handlers[i] = NULL;

	for (i=0; i<SDL_NumJoysticks(); i++) {
		SDL_JoystickOpen(i);
	}

	NEW_CMD("pixel::sdl::setup_screen", glue_setup_screen);
	NEW_CMD("pixel::sdl::get_caps", glue_get_caps);
	NEW_CMD("pixel::sdl::do_frame", glue_do_frame);
	NEW_CMD("pixel::sdl::gettimeofday", glue_gettimeofday);
	NEW_CMD("pixel::sdl::getticks", glue_getticks);
	NEW_CMD("pixel::sdl::elapsed", glue_elapsed);
	NEW_CMD("pixel::sdl::frames", glue_frames);
	NEW_CMD("pixel::sdl::fps", glue_fps);
	NEW_CMD("pixel::sdl::bind_events", glue_bind_events);
	NEW_CMD("pixel::sdl::dispatch_events", glue_dispatch_events);
	NEW_CMD("pixel::sdl::toggle_fullscreen", glue_toggle_fullscreen);
	NEW_CMD("pixel::sdl::show_cursor", glue_show_cursor);
	NEW_CMD("pixel::sdl::numjoysticks", glue_numjoysticks);
	NEW_CMD("pixel::sdl::joystickname", glue_joystickname);
	NEW_CMD("pixel::sdl::joystickopen", glue_joystickopen);
	NEW_CMD("pixel::sdl::wm_grabinput", glue_wm_grabinput);

	// OpenGL
	NEW_CMD("pixel::sdl::gl_swapbuffers", glue_gl_swapbuffers);

	TEST_OK(Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION));

	return TCL_OK;
}


