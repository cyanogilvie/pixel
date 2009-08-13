// vim: ts=4 shiftwidth=4 tags=../tags

#include <X11/Xlib.h>
#include <Imlib2.h>
#include <tcl.h>
#include <tclstuff.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <Pixel/pixel.h>

static Tcl_Interp *g_interp;
static Tcl_Obj *g_progress_cb = NULL;

static int progress_func(Imlib_Image image, char percent, //{{{1
		int update_x, int update_y, int update_w, int update_h)
{
	Tcl_Obj *o[2];
	
	if (g_progress_cb != NULL) {
		o[0] = g_progress_cb;
		o[1] = Tcl_NewIntObj(percent);
		Tcl_IncrRefCount(o[0]);
		Tcl_IncrRefCount(o[1]);
		
		if (Tcl_EvalObjv(g_interp, 2, o, TCL_EVAL_GLOBAL) == TCL_ERROR)
			Tcl_BackgroundError(g_interp);
		
		Tcl_DecrRefCount(o[0]);
		Tcl_DecrRefCount(o[1]);
	}
	return 1;
}


static char *lookup_load_error(Imlib_Load_Error error) //{{{1
{
	switch (error) {
		case IMLIB_LOAD_ERROR_NONE:
			return "ok";
		case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
		case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
		case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
		case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
			return "File not found";
		case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
			return "File is a directory";
		case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
			return "Permission denied reading file";
		case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
			return "Image format not recognised";
		case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
			return "Filename is too long";
		case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
			return "Too many symbolic links";
		case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
			return "Out of memory";
		case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
			return "Out of file descriptors";
		case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
			return "Permission denied writing to file";
		case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
			return "Out of disk space";
		default:
		case IMLIB_LOAD_ERROR_UNKNOWN:
			return "Unknown error";
	}
}


static int glue_set_cache_size(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int cache_size;
	
	CHECK_ARGS(1, "cache_size");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &cache_size));
	imlib_set_cache_size(cache_size);
	
	return TCL_OK;
}


static int glue_get_cache_size(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int cache_size;
	
	CHECK_ARGS(0, "");

	cache_size = imlib_get_cache_size();

	Tcl_SetObjResult(interp, Tcl_NewIntObj(cache_size));
	
	return TCL_OK;
}


static int glue_set_font_cache_size(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int cache_size;
	
	CHECK_ARGS(1, "cache_size");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &cache_size));
	imlib_set_font_cache_size(cache_size);
	
	return TCL_OK;
}


static int glue_add_path_to_font_path(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	CHECK_ARGS(1, "path");

	imlib_add_path_to_font_path(Tcl_GetString(objv[1]));
	
	return TCL_OK;
}


static int glue_set_color_usage(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int num_colours;
	
	CHECK_ARGS(1, "num_colours");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &num_colours));
	
	imlib_set_color_usage(num_colours);
	
	return TCL_OK;
}


static int glue_context_set_dither(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	int dither;
	
	CHECK_ARGS(1, "dither?");

	TEST_OK(Tcl_GetBooleanFromObj(interp, objv[1], &dither));
	
	imlib_context_set_dither(dither);
	
	return TCL_OK;
}


static int glue_load_image(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	Imlib_Image			image;
	Imlib_Load_Error	error;
	int					w, h;
	_pel				init;
	gimp_image_t		*new;
	
	if (objc < 2 || objc > 3)
		CHECK_ARGS(1, "filename ?progress_callback?");

	image = imlib_load_image_with_error_return(Tcl_GetString(objv[1]), &error);
	if (error != IMLIB_LOAD_ERROR_NONE)
		THROW_ERROR("Cannot load image ", Tcl_GetString(objv[1]), ": ", 
				lookup_load_error(error));

	imlib_context_set_image(image);
	
	if (objc == 3) {
		if (g_progress_cb != NULL) {
			Tcl_DecrRefCount(g_progress_cb);
			g_progress_cb = NULL;
		}
		g_interp = interp;
		g_progress_cb = objv[2];
		Tcl_IncrRefCount(g_progress_cb);
		imlib_context_set_progress_function(progress_func);
		imlib_context_set_progress_granularity(10);
	}

	imlib_image_set_changes_on_disk();
	w = imlib_image_get_width();
	h = imlib_image_get_height();
	init.c = 0x00000000;

	new = pmap_new(w, h, init);

	memcpy(new->pixel_data, imlib_image_get_data_for_reading_only(), w * h * 4);

	imlib_free_image();

	if (g_progress_cb != NULL) {
		Tcl_DecrRefCount(g_progress_cb);
		g_interp = NULL;
		g_progress_cb = NULL;
		imlib_context_set_progress_function(NULL);
	}
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	
	return TCL_OK;
}


static int glue_save_image(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	Imlib_Image			image;
	Imlib_Load_Error	error;
	gimp_image_t		*pmap;
	char				*filename;
	char				*type;
	
	if (objc < 3 || objc > 5)
		CHECK_ARGS(2, "pmap filename ?type? ?progress_callback?");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	
//	fprintf(stderr, "pmap->width: %ld, pmap->height: %ld\n", pmap->width, pmap->height);
	image = imlib_create_image_using_data(pmap->width, pmap->height,
			(DATA32 *)pmap->pixel_data);
	if (image == NULL)
		THROW_ERROR("Failed to wrap pmap in an imlib2 image");

	imlib_context_set_image(image);
	
	filename = Tcl_GetString(objv[2]);
	type = NULL;
	if (objc >= 4) {
		type = Tcl_GetString(objv[3]);
		imlib_image_set_format(type);
	} else {
		type = strrchr(filename, '.');
		if (type)
			imlib_image_set_format(type + 1);
	}
	
	if (objc == 5) {
		if (g_progress_cb != NULL) {
			Tcl_DecrRefCount(g_progress_cb);
			g_progress_cb = NULL;
		}
		g_interp = interp;
		g_progress_cb = objv[4];
		Tcl_IncrRefCount(g_progress_cb);
		imlib_context_set_progress_function(progress_func);
		imlib_context_set_progress_granularity(10);
	}
	
	imlib_save_image_with_error_return(filename, &error);
	imlib_free_image();
	
	if (g_progress_cb != NULL) {
		Tcl_DecrRefCount(g_progress_cb);
		g_interp = NULL;
		g_progress_cb = NULL;
		imlib_context_set_progress_function(NULL);
	}

	if (error != IMLIB_LOAD_ERROR_NONE)
		THROW_ERROR("Error saving image to file ", filename, ": ", lookup_load_error(error));

	return TCL_OK;
}


static int glue_scale_pmap(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*src_pmap;
	gimp_image_t		*dest_pmap;
	Imlib_Border		border;
	Imlib_Image			*src;
	Imlib_Image			*dest;
	_pel				init;
	int					anti_alias = 1;
	int					w, h;
	
	if (objc < 4 || (objc > 6 && objc != 9 && objc != 10))
		CHECK_ARGS(3, "src_pmap w h ?smooth? ?border_l border_r border_t border_b? ?progress_callback?");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src_pmap));

	src = imlib_create_image_using_data(src_pmap->width, src_pmap->height,
			(DATA32 *)src_pmap->pixel_data);
	if (src == NULL)
		THROW_ERROR("Failed to wrap src_pmap in an imlib2 image");

	imlib_context_set_image(src);
	
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &w));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &h));
	if (objc >= 5)
		TEST_OK(Tcl_GetBooleanFromObj(interp, objv[4], &anti_alias));
	if (objc >= 9) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &border.left));
		TEST_OK(Tcl_GetIntFromObj(interp, objv[6], &border.right));
		TEST_OK(Tcl_GetIntFromObj(interp, objv[7], &border.top));
		TEST_OK(Tcl_GetIntFromObj(interp, objv[8], &border.bottom));
		imlib_image_set_border(&border);
	}

	if (objc == 10) {
		if (g_progress_cb != NULL) {
			Tcl_DecrRefCount(g_progress_cb);
			g_progress_cb = NULL;
		}
		g_interp = interp;
		g_progress_cb = objv[9];
		Tcl_IncrRefCount(g_progress_cb);
		imlib_context_set_progress_function(progress_func);
		imlib_context_set_progress_granularity(10);
	}
	
	imlib_context_set_anti_alias((char)anti_alias);

	dest = imlib_create_cropped_scaled_image(0, 0,
			src_pmap->width, src_pmap->height,
			w, h);
	imlib_free_image();

	if (g_progress_cb != NULL) {
		Tcl_DecrRefCount(g_progress_cb);
		g_interp = NULL;
		g_progress_cb = NULL;
		imlib_context_set_progress_function(NULL);
	}

	if (dest == NULL)
		THROW_ERROR("Failed to scale image");

	imlib_context_set_image(dest);
	init.c = 0x00000000;
	dest_pmap = pmap_new(w, h, init);
	memcpy(dest_pmap->pixel_data,
			imlib_image_get_data_for_reading_only(), w * h * 4);
	imlib_free_image();

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(dest_pmap));

	return TCL_OK;
}
	

static int glue_blur_pmap(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*src_pmap;
	int					radius;
	Imlib_Border		border;
	Imlib_Image			*src;
	
	CHECK_ARGS(2, "pmap radius");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src_pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &radius));

	src = imlib_create_image_using_data(src_pmap->width, src_pmap->height,
			(DATA32 *)src_pmap->pixel_data);
	if (src == NULL)
		THROW_ERROR("Failed to wrap src_pmap in an imlib2 image");

	imlib_context_set_image(src);
	imlib_image_blur(radius);
	
	memcpy(src_pmap->pixel_data,
			imlib_image_get_data_for_reading_only(),
			src_pmap->width * src_pmap->height * 4);
	imlib_free_image();

	return TCL_OK;
}
	

static int glue_sharpen_pmap(ClientData foo, Tcl_Interp *interp, //{{{1
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t		*src_pmap;
	int					radius;
	Imlib_Border		border;
	Imlib_Image			*src;
	
	CHECK_ARGS(2, "pmap radius");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &src_pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &radius));

	src = imlib_create_image_using_data(src_pmap->width, src_pmap->height,
			(DATA32 *)src_pmap->pixel_data);
	if (src == NULL)
		THROW_ERROR("Failed to wrap src_pmap in an imlib2 image");

	imlib_context_set_image(src);
	imlib_image_sharpen(radius);

	memcpy(src_pmap->pixel_data,
			imlib_image_get_data_for_reading_only(),
			src_pmap->width * src_pmap->height * 4);
	imlib_free_image();

	return TCL_OK;
}
	

int Pixel_imlib2_Init(Tcl_Interp *interp) //{{{1
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) return TCL_ERROR;
#ifdef USE_PIXEL_STUBS
	if (Pixel_InitStubs(interp, "3.4", 0) == NULL) return TCL_ERROR;
#endif

	imlib_set_color_usage(128);
	imlib_set_cache_size(0);
	imlib_set_font_cache_size(0);

	NEW_CMD("pixel::imlib2::set_cache_size", glue_set_cache_size);
	NEW_CMD("pixel::imlib2::get_cache_size", glue_get_cache_size);
	NEW_CMD("pixel::imlib2::set_font_cache_size", glue_set_font_cache_size);
	NEW_CMD("pixel::imlib2::add_path_to_font_path", glue_add_path_to_font_path);
	NEW_CMD("pixel::imlib2::set_color_usage", glue_set_color_usage);
	NEW_CMD("pixel::imlib2::context_set_dither", glue_context_set_dither);
	NEW_CMD("pixel::imlib2::load_image", glue_load_image);
	NEW_CMD("pixel::imlib2::save_image", glue_save_image);
	NEW_CMD("pixel::imlib2::scale_pmap", glue_scale_pmap);
	NEW_CMD("pixel::imlib2::blur", glue_blur_pmap);
	NEW_CMD("pixel::imlib2::sharpen", glue_sharpen_pmap);

	TEST_OK(Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION));

	return TCL_OK;
}


