// vim: foldmarker=<<<,>>> shiftwidth=4 ts=4

#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include <stdio.h>
#include <libxcf.h>
#include "2d.h"
#include "tcl_pmap.h"
#include <tclstuff.h>
#include <tcl.h>

#define LXCF_MAX_INF	1000

typedef struct {
	FILE	*fp;
	XcfInfo	*info;
} lxcf_file;

static lxcf_file	lxcf_inf[LXCF_MAX_INF];
static int			lxcf_inf_used[LXCF_MAX_INF];

#define SET_OUTPARAM(obj, newval) \
	(Tcl_ObjSetVar2(interp, obj, NULL, newval, \
					TCL_LEAVE_ERR_MSG) == NULL) ? TCL_ERROR : TCL_OK
		


gimp_image_t *xcf_image_render_pmap(XcfInfo *info) //<<<1
{
	unsigned char	*buffer;
	int				width, height, rowstride, bufsize, r, c;
	unsigned char	*s;
	_pel			*d;
	gimp_image_t	*new;

	xcf_image_get_size(info, &width, &height);
	rowstride = xcf_image_get_rowstride(info);
	
	bufsize = sizeof(guchar) * height * rowstride;
	buffer = (unsigned char *)malloc(bufsize);
	xcf_image_render(info, buffer);

	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	new->width = width;
	new->height = height;
	new->bytes_per_pixel = 4;
	new->pixel_data = (_pel *)malloc(width*height*4);

	s = buffer;
	d = new->pixel_data;
	
	if (xcf_image_get_bpp(info) == 4) {
		for (r=0; r<height; r++) {
			memcpy(d, s, width*4);
			s += rowstride;
			d += new->width;
		}
	} else {
		for (r=0; r<height; r++) {
			_pel	*t = d;
			for (c=0; c<width; c++, t++) {
				t->ch.r = *(s+c*3+0);
				t->ch.g = *(s+c*3+1);
				t->ch.b = *(s+c*3+2);
				t->ch.a = 0xff;
			}
			s += rowstride;
			d += new->width;
		}
	}

	free(buffer);
	
	return new;

}


gimp_image_t *xcf_layer_render_pmap(XcfInfo *info, int layer) //<<<1
{
	unsigned char	*buffer;
	int				width, height, rowstride, bufsize, r, c;
	unsigned char	*s;
	_pel			*d;
	gimp_image_t	*new;
	int				success;

	xcf_layer_get_size(info, layer, &width, &height);
	rowstride = xcf_layer_get_rowstride(info, layer);
	
	bufsize = sizeof(guchar) * height * rowstride;
	buffer = (unsigned char *)malloc(bufsize);
	memset(buffer, 0x0, bufsize);
	success = xcf_layer_render(info, layer, buffer);
	fprintf(stderr, "success: (%d)\n", success == TRUE);

	new = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	new->width = width;
	new->height = height;
	new->bytes_per_pixel = 4;
	new->pixel_data = (_pel *)malloc(width*height*4);

	fprintf(stderr, "buffer: (%x)\n", *buffer);
	s = buffer;
	d = new->pixel_data;
	
	if (xcf_image_get_bpp(info) == 4) {
		for (r=0; r<height; r++) {
			memcpy(d, s, width*4);
			s += rowstride;
			d += new->width;
		}
	} else {
		for (r=0; r<height; r++) {
			_pel	*t = d;
			for (c=0; c<width; c++, t++) {
				t->ch.r = *(s+c*3+0);
				t->ch.g = *(s+c*3+1);
				t->ch.b = *(s+c*3+2);
				t->ch.a = 0xff;
			}
			s += rowstride;
			d += new->width;
		}
	}

	free(buffer);
	
	return new;

}


static int find_unused_inf() //<<<1
{
	int		i, n=-1;

	for (i=0; i<LXCF_MAX_INF; i++) {
		if (!lxcf_inf_used[i]) {
			n = i;
			break;
		}
	}

	return n;
}


static int check_info_handle_open(Tcl_Interp *interp, int i) //<<<1
{
	if (i < 0 || i > LXCF_MAX_INF)
		THROW_ERROR("info_handle is out of range");
	
	if (!lxcf_inf_used[i])
		THROW_ERROR("info_handle is not open");
	
	return TCL_OK;
}


static int get_info_handle(Tcl_Interp *interp, Tcl_Obj *info_handle, //<<<1
		XcfInfo **info)
{
	int		n;
	
	TEST_OK(Tcl_GetIntFromObj(interp, info_handle, &n));
	TEST_OK(check_info_handle_open(interp, n));
	
	*info = lxcf_inf[n].info;
	
	return TCL_OK;
}


static int glue_xcf_open_file(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	int		i;
	FILE	*fp;
	XcfInfo	*info;
	
	CHECK_ARGS(1, "filename");

	i = find_unused_inf();

	if (i == -1)
		THROW_ERROR("No info handles available!");
	
	lxcf_inf[i].fp = fp = fopen(Tcl_GetString(objv[1]), "r");
	if (fp == 0 || ferror(fp) != 0)
		THROW_ERROR("Cannot open file: (", Tcl_GetString(objv[1]), ")");
	
	lxcf_inf[i].info = info = xcf_open_file(fp);
	if (info == 0) {
		fclose(fp);
		THROW_ERROR("Cannot parse .xcf file");
	}

	lxcf_inf_used[i] = 1;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(i));
	
	return TCL_OK;
}


static int glue_xcf_close_file(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	int i;
	
	CHECK_ARGS(1, "info_handle");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &i));
	TEST_OK(check_info_handle_open(interp, i));
	
	xcf_info_unref(lxcf_inf[i].info);
	fclose(lxcf_inf[i].fp);

	lxcf_inf_used[i] = 0;
	
	return TCL_OK;
}


static int glue_xcf_image_get_size(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo	*info;
	int		width, height;
	
	CHECK_ARGS(3, "info_handle width_var height_var");

	TEST_OK(get_info_handle(interp, objv[1], &info));

	xcf_image_get_size(info, &width, &height);

	TEST_OK(SET_OUTPARAM(objv[2], Tcl_NewIntObj(width)));
	TEST_OK(SET_OUTPARAM(objv[3], Tcl_NewIntObj(height)));

	return TCL_OK;
}

	
static int glue_xcf_image_get_num_layers(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	
	CHECK_ARGS(1, "info_handle");

	TEST_OK(get_info_handle(interp, objv[1], &info));
	
	Tcl_SetObjResult(interp, Tcl_NewIntObj(xcf_image_get_num_layers(info)));

	return TCL_OK;
}


static int glue_xcf_image_get_bpp(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	
	CHECK_ARGS(1, "info_handle");
	
	TEST_OK(get_info_handle(interp, objv[1], &info));

	Tcl_SetObjResult(interp, Tcl_NewIntObj(xcf_image_get_bpp(info)));

	return TCL_OK;
}


static int glue_xcf_image_get_rowstride(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	
	CHECK_ARGS(1, "info_handle");
	
	TEST_OK(get_info_handle(interp, objv[1], &info));

	Tcl_SetObjResult(interp, Tcl_NewIntObj(xcf_image_get_rowstride(info)));

	return TCL_OK;
}


static int glue_xcf_image_render(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	unsigned char	*buffer;
	XcfInfo			*info;
	int				width, height, rowstride, bufsize, r;
	_pel			*s;
	_pel			*d;
	gimp_image_t	*new;
	
	
	CHECK_ARGS(1, "info_handle");

	TEST_OK(get_info_handle(interp, objv[1], &info));

	new = xcf_image_render_pmap(info);

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


static int glue_xcf_layer_get_name(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	int			i;
	
	CHECK_ARGS(2, "info_handle layer_num");
	
	TEST_OK(get_info_handle(interp, objv[1], &info));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &i));
	
	Tcl_SetObjResult(interp, Tcl_NewStringObj(xcf_layer_get_name(info, i), -1));

	return TCL_OK;
}


static int glue_xcf_layer_get_size(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	int			i, width, height;
	
	CHECK_ARGS(4, "info_handle layer_num width_var height_var");
	
	TEST_OK(get_info_handle(interp, objv[1], &info));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &i));
	
	xcf_layer_get_size(info, i, &width, &height);

	TEST_OK(SET_OUTPARAM(objv[3], Tcl_NewIntObj(width)));
	TEST_OK(SET_OUTPARAM(objv[4], Tcl_NewIntObj(height)));
	
	return TCL_OK;
}


static int glue_xcf_layer_get_offset(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	int			i, ofs_x, ofs_y;
	
	CHECK_ARGS(4, "info_handle layer_num offset_x_var offset_y_var");
	
	TEST_OK(get_info_handle(interp, objv[1], &info));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &i));
	
	xcf_layer_get_offset(info, i, &ofs_x, &ofs_y);

	TEST_OK(SET_OUTPARAM(objv[3], Tcl_NewIntObj(ofs_x)));
	TEST_OK(SET_OUTPARAM(objv[4], Tcl_NewIntObj(ofs_y)));
	
	return TCL_OK;
}


static int glue_xcf_layer_get_bpp(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	int			i;
	
	CHECK_ARGS(2, "info_handle layer_num");
	
	TEST_OK(get_info_handle(interp, objv[1], &info));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &i));
	
	Tcl_SetObjResult(interp, Tcl_NewIntObj(xcf_layer_get_bpp(info, i)));

	return TCL_OK;
}


static int glue_xcf_layer_render(ClientData *foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	XcfInfo		*info;
	int			i;
	
	CHECK_ARGS(2, "info_handle layer_num");
	
	TEST_OK(get_info_handle(interp, objv[1], &info));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &i));
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(xcf_layer_render_pmap(info, i)));

	return TCL_OK;
}


static int glue_load_xcf(ClientData foo, Tcl_Interp *interp, //<<<1
		int objc, Tcl_Obj *CONST objv[])
{
	int				i;
	FILE			*fp;
	XcfInfo			*info;
	gimp_image_t	*new;
	Tcl_Obj			*maininfo;
	Tcl_Obj			*layers;
	Tcl_Obj			*row;
	int				l;
	int				img_width, img_height, img_numlayers, img_bpp, img_rowstride;
	
	fprintf(stderr, "objc: (%d)\n", objc);
	if (objc < 2 || objc > 3) {
		CHECK_ARGS(1, "filename ?infolistvar?");
	}

	fp = fopen(Tcl_GetString(objv[1]), "r");
	if (fp == 0 || ferror(fp) != 0)
		THROW_ERROR("Cannot open file: (", Tcl_GetString(objv[1]), ")");
	
	info = xcf_open_file(fp);
	if (info == 0) {
		fclose(fp);
		THROW_ERROR("Cannot parse .xcf file");
	}

	new = xcf_image_render_pmap(info);

	if (objc == 3) {
		layers = Tcl_NewListObj(0, NULL);
		xcf_image_get_size(info, &img_width, &img_height);
		img_numlayers = xcf_image_get_num_layers(info);
		//img_bpp = xcf_image_get_cpp(info);
		//img_rowstride = xcf_image_get_rowstride(info);
	
		for (l=0; l<img_numlayers; l++) {
			int		width, height, x, y, bpp;
			char	*name;
	
			name = xcf_layer_get_name(info, l);
			xcf_layer_get_size(info, l, &width, &height);
			xcf_layer_get_offset(info, l, &x, &y);
			bpp = xcf_layer_get_bpp(info, l);
	
			row = Tcl_NewListObj(0, NULL);
			TEST_OK(Tcl_ListObjAppendElement(interp, row, Tcl_NewStringObj(name, -1)));
			TEST_OK(Tcl_ListObjAppendElement(interp, row, Tcl_NewIntObj(x)));
			TEST_OK(Tcl_ListObjAppendElement(interp, row, Tcl_NewIntObj(y)));
			TEST_OK(Tcl_ListObjAppendElement(interp, row, Tcl_NewIntObj(width)));
			TEST_OK(Tcl_ListObjAppendElement(interp, row, Tcl_NewIntObj(height)));
			//TEST_OK(Tcl_ListObjAppendElement(interp, row, Tcl_NewIntObj(bpp)));
			
			TEST_OK(Tcl_ListObjAppendElement(interp, layers, row));
		}
		
		xcf_info_unref(info);
		fclose(fp);
	
		maininfo = Tcl_NewListObj(0, NULL);
		TEST_OK(Tcl_ListObjAppendElement(interp, maininfo, Tcl_NewIntObj(img_width)));
		TEST_OK(Tcl_ListObjAppendElement(interp, maininfo, Tcl_NewIntObj(img_height)));
		TEST_OK(Tcl_ListObjAppendElement(interp, maininfo, Tcl_NewIntObj(img_numlayers)));
		TEST_OK(Tcl_ListObjAppendElement(interp, maininfo, layers));
		
		TEST_OK(SET_OUTPARAM(objv[2], maininfo));
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));
	
	return TCL_OK;
}


int Pixel_xcf_Init(Tcl_Interp *interp) //<<<1
{
	memset((void *)lxcf_inf_used, 0, sizeof(int)*LXCF_MAX_INF);
	
	NEW_CMD("pixel::xcf::open_file", glue_xcf_open_file);
	NEW_CMD("pixel::xcf::close_file", glue_xcf_close_file);
	
	NEW_CMD("pixel::xcf::image_get_size", glue_xcf_image_get_size);
	NEW_CMD("pixel::xcf::image_get_num_layers", glue_xcf_image_get_num_layers);
	NEW_CMD("pixel::xcf::image_get_bpp", glue_xcf_image_get_bpp);
	NEW_CMD("pixel::xcf::image_get_rowstride", glue_xcf_image_get_rowstride);
	NEW_CMD("pixel::xcf::image_render", glue_xcf_image_render);
	
	NEW_CMD("pixel::xcf::layer_get_name", glue_xcf_layer_get_name);
	NEW_CMD("pixel::xcf::layer_get_size", glue_xcf_layer_get_size);
	NEW_CMD("pixel::xcf::layer_get_offset", glue_xcf_layer_get_offset);
	NEW_CMD("pixel::xcf::layer_get_bpp", glue_xcf_layer_get_bpp);
	NEW_CMD("pixel::xcf::layer_render", glue_xcf_layer_render);

	NEW_CMD("pixel::xcf::loadxcf", glue_load_xcf);

	return TCL_OK;
}


