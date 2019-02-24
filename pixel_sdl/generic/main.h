#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <SDL/SDL.h>
#include <tcl.h>
#include "tclstuff.h"
#include <pixel.h>

typedef struct {
	SDL_Surface		*console;
	SDL_Surface		*surface;
	long			lastsec;
	long			lastusec;
	long			elapsed;
	double			fps;
	long long		frames;
	gimp_image_t	*prebuffer;
	int				need_updaterects;
} sdl_console_inf;

#include "sdl_timestuff.h"

/*
EXTERN_C int alloc_buf(Tcl_Interp *interp, int *buf_id);
EXTERN_C int alloc_pmap(Tcl_Interp *interp, int *pmap_id);
EXTERN_C int lookup_buf(Tcl_Interp *interp, Tcl_Obj *bufobj, _pel **bufptr);
EXTERN_C int lookup_pmap(Tcl_Interp *interp, Tcl_Obj *bufobj, gimp_image_t **pmap_ptr);
EXTERN_C int register_mouse_handler(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int register_kbd_handler(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int register_idle_handler(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C void mouse_handler(void);
EXTERN_C void kbd_handler(int kcode);
EXTERN_C void idle_handler(void);
EXTERN_C int glue_setup_gfx(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_loadtiff(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_new_buf(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_pmap_cpy(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_pmap_dup(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_buf_cpy(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_buf_clr(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_buf_filter(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_buf_displace(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_buf_scale(ClientData scale_method, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_do_frame(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_elapsed_time(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_gettimeofday(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
EXTERN_C int glue_main_loop(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
*/


EXTERN_C int Pixel_sdl_Init(Tcl_Interp *interp);

#endif
