#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include <stdio.h>
#include <jpeglib.h>
#include "tclstuff.h"
#include <pixel.h>
#include <setjmp.h>


#define JPEG_ENCODE_BUFSIZE		65520


struct custom_error_mgr {
	struct jpeg_error_mgr pub;

	jmp_buf setjmp_buffer;
};

typedef struct custom_error_mgr	*custom_error_ptr;


void custom_error_exit(j_common_ptr cinfo)
{
	custom_error_ptr jerr = (custom_error_ptr)cinfo->err;
	(*cinfo->err->output_message)(cinfo);
	longjmp(jerr->setjmp_buffer, 1);
}


gimp_image_t *loadjpeg(char *filename) // {{{1
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;
	gimp_image_t	*dest = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	FILE			*fp;
	_pel			*r;
	JSAMPROW		row_pointer[1];

	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);
	(void)jpeg_read_header(&cinfo, TRUE);
	(void)jpeg_start_decompress(&cinfo);

	cinfo.output_components = 4;
	cinfo.out_color_space = JCS_EXT_BGRA;

	dest->width = cinfo.output_width;
	dest->height = cinfo.output_height;
	dest->bytes_per_pixel = 4;
	r = dest->pixel_data = (_pel *)malloc(dest->width*dest->height*4);

	while (cinfo.output_scanline < cinfo.output_height) {
		row_pointer[0] = (JSAMPLE*)r;
		(void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
		r += dest->width;
	}

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	return dest;
}


int savejpeg(char *filename, gimp_image_t *pmap, int quality) // {{{1
{
	struct jpeg_compress_struct		cinfo;
	struct jpeg_error_mgr			jerr;
	FILE		*fp;
	JSAMPROW	row_pointer[1];
	_pel		*s;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	if ((fp = fopen(filename, "wb")) == NULL)
		return -2;

	jpeg_stdio_dest(&cinfo, fp);

	cinfo.image_width = pmap->width;
	cinfo.image_height = pmap->height;
	cinfo.input_components = 4;
	cinfo.in_color_space = JCS_EXT_BGRA;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	s = pmap->pixel_data;
	
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = (JSAMPLE*)s;
		s += pmap->width;
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(fp);
	jpeg_destroy_compress(&cinfo);

	return 0;
}


unsigned char *encodejpeg(gimp_image_t *pmap, unsigned long *length, int quality) //{{{1
{
	struct jpeg_compress_struct		cinfo;
	struct jpeg_error_mgr			jerr;
	_pel		*s;
	JSAMPROW	row_pointer[1];
	unsigned char	*odata = NULL;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	jpeg_mem_dest(&cinfo, &odata, length);

	cinfo.image_width = pmap->width;
	cinfo.image_height = pmap->height;
	cinfo.input_components = 4;
	cinfo.in_color_space = JCS_EXT_BGRA;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	s = pmap->pixel_data;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = (JSAMPLE*)s;
		s += pmap->width;
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);

	jpeg_destroy_compress(&cinfo);

	return odata;
}


gimp_image_t *decodejpeg(unsigned char *jpeg_data, int length) // {{{1
{
	struct jpeg_decompress_struct	cinfo;
	struct custom_error_mgr			jerr;
	gimp_image_t	*dest = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	_pel			*r;
	JSAMPROW		row_pointer[1];

	if (length == 0)
		return NULL;

	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpeg_data, length);

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = custom_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	(void)jpeg_read_header(&cinfo, TRUE);
	(void)jpeg_start_decompress(&cinfo);

	if (cinfo.out_color_space == JCS_GRAYSCALE) {
		// Have to handle this manually.  According to the documentation,
		// changing out_color_space, out_color_components and output_components
		// to what we need should work, but doesn't.
	} else {
		cinfo.output_components = 4;
		cinfo.out_color_components = 4;
		cinfo.out_color_space = JCS_EXT_BGRA;
	}

	dest->width = cinfo.output_width;
	dest->height = cinfo.output_height;
	dest->bytes_per_pixel = 4;
	r = dest->pixel_data = (_pel *)malloc(dest->width*dest->height*4);

	if (cinfo.out_color_space == JCS_GRAYSCALE) {
		JSAMPLE*	scanline = NULL;
		JSAMPLE*	s = NULL;

		scanline = (JSAMPLE*)malloc(sizeof(JSAMPLE) * dest->width);

		while (cinfo.output_scanline < cinfo.output_height) {
			int	c, i=dest->width;

			row_pointer[0] = scanline;
			s = scanline;
			(void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
			while (i--) {
				for (c=0; c<3; c++)
					r->chan[c] = *s;

				r->ch.a = 0xff;
				s++;
				r++;
			}
		}

		free(scanline);  scanline=NULL; s=NULL;
	} else {
		while (cinfo.output_scanline < cinfo.output_height) {
			row_pointer[0] = (JSAMPLE*)r;
			(void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
			r += dest->width;
		}
	}

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return dest;
}


// loadjpeg filename {{{1
static int glue_loadjpeg(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t *	pmap;
	CHECK_ARGS(1, "filename");

	pmap = loadjpeg(Tcl_GetString(objv[1]));
	if (pmap == NULL)
		THROW_ERROR("Error loading jpeg file: ", Tcl_GetString(objv[1]));
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));

	return TCL_OK;
}


// savejpeg filename pmap quality {{{1
static int glue_savejpeg(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				err;
	gimp_image_t	*pmap;
	int				quality;
	
	CHECK_ARGS(3, "filename pmap quality");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &quality));
	
	err = savejpeg(Tcl_GetString(objv[1]), pmap, quality);

	if (err != 0)
		THROW_ERROR("Error saving jpeg file: ", Tcl_GetString(objv[1]));
	
	return TCL_OK;
}


// encodejpeg pmap quality {{{1
static int glue_encodejpeg(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap;
	int				quality;
	unsigned long	length;
	unsigned char	*jpeg_data;
	
	CHECK_ARGS(2, "pmap quality");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &quality));

	jpeg_data = encodejpeg(pmap, &length, quality);

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(jpeg_data, length));

	free(jpeg_data);
	
	return TCL_OK;
}


// decodejpeg jpeg_data {{{1
static int glue_decodejpeg(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	unsigned char	*jpeg_data;
	int				length;
	gimp_image_t	*new;
	
	CHECK_ARGS(1, "jpeg_data");

	jpeg_data = Tcl_GetByteArrayFromObj(objv[1], &length);

	new = decodejpeg(jpeg_data, length);
	if (new == NULL)
		THROW_ERROR("Error loading jpeg");

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

	return TCL_OK;
}


// jpeg_info filename {{{1
static int glue_jpeg_info(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;
	FILE			*fp;
	Tcl_Obj			*res;

	
	CHECK_ARGS(1, "filename");

	if ((fp = fopen(Tcl_GetString(objv[1]), "rb")) == NULL)
		THROW_ERROR("Cannot open file: (", Tcl_GetString(objv[1]), ")");

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);
	(void)jpeg_read_header(&cinfo, TRUE);
	res = Tcl_NewObj();

#define ADD_RES_ITEM(name, obj) \
	Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj(name, -1)); \
	Tcl_ListObjAppendElement(interp, res, obj);

	ADD_RES_ITEM("image_width", Tcl_NewIntObj(cinfo.image_width));
	ADD_RES_ITEM("image_height", Tcl_NewIntObj(cinfo.image_height));
	ADD_RES_ITEM("num_components", Tcl_NewIntObj(cinfo.num_components));
	switch (cinfo.jpeg_color_space) {
		case JCS_GRAYSCALE:
			ADD_RES_ITEM("jpeg_color_space", Tcl_NewStringObj("JCS_GRAYSCALE", -1));
			break;

		case JCS_RGB:
			ADD_RES_ITEM("jpeg_color_space", Tcl_NewStringObj("JCS_RGB", -1));
			break;

		case JCS_YCbCr:
			ADD_RES_ITEM("jpeg_color_space", Tcl_NewStringObj("JCS_YCbCr", -1));
			break;

		case JCS_CMYK:
			ADD_RES_ITEM("jpeg_color_space", Tcl_NewStringObj("JCS_CMYK", -1));
			break;

		case JCS_YCCK:
			ADD_RES_ITEM("jpeg_color_space", Tcl_NewStringObj("JCS_YCCK", -1));
			break;

		case JCS_UNKNOWN:
		default:
			ADD_RES_ITEM("jpeg_color_space", Tcl_NewStringObj("JCS_UNKNOWN", -1));
			break;
	}
	switch (cinfo.out_color_space) {
		case JCS_GRAYSCALE:
			ADD_RES_ITEM("out_color_space", Tcl_NewStringObj("JCS_GRAYSCALE", -1));
			break;

		case JCS_RGB:
			ADD_RES_ITEM("out_color_space", Tcl_NewStringObj("JCS_RGB", -1));
			break;

		case JCS_YCbCr:
			ADD_RES_ITEM("out_color_space", Tcl_NewStringObj("JCS_YCbCr", -1));
			break;

		case JCS_CMYK:
			ADD_RES_ITEM("out_color_space", Tcl_NewStringObj("JCS_CMYK", -1));
			break;

		case JCS_YCCK:
			ADD_RES_ITEM("out_color_space", Tcl_NewStringObj("JCS_YCCK", -1));
			break;

		case JCS_UNKNOWN:
		default:
			ADD_RES_ITEM("out_color_space", Tcl_NewStringObj("JCS_UNKNOWN", -1));
			break;
	}
	ADD_RES_ITEM("scale_num", Tcl_NewIntObj(cinfo.scale_num));
	ADD_RES_ITEM("scale_denom", Tcl_NewIntObj(cinfo.scale_denom));
	ADD_RES_ITEM("output_gamma", Tcl_NewDoubleObj(cinfo.output_gamma));
	ADD_RES_ITEM("buffered_image", Tcl_NewBooleanObj(cinfo.buffered_image));
	ADD_RES_ITEM("raw_data_out", Tcl_NewBooleanObj(cinfo.raw_data_out));
	switch (cinfo.dct_method) {
		case JDCT_ISLOW:
			ADD_RES_ITEM("dct_method", Tcl_NewStringObj("JDCT_ISLOW", -1));
			break;

		case JDCT_IFAST:
			ADD_RES_ITEM("dct_method", Tcl_NewStringObj("JDCT_IFAST", -1));
			break;

		case JDCT_FLOAT:
			ADD_RES_ITEM("dct_method", Tcl_NewStringObj("JDCT_FLOAT", -1));
			break;

		default:
			ADD_RES_ITEM("dct_method", Tcl_NewStringObj("JDCT_UNKNOWN", -1));
			break;
	}
	ADD_RES_ITEM("do_fancy_upsampling", Tcl_NewBooleanObj(cinfo.do_fancy_upsampling));
	ADD_RES_ITEM("do_block_smoothing", Tcl_NewBooleanObj(cinfo.do_block_smoothing));
	ADD_RES_ITEM("quantize_colors", Tcl_NewBooleanObj(cinfo.quantize_colors));
	switch (cinfo.dither_mode) {
		case JDITHER_NONE:
			ADD_RES_ITEM("dither_mode", Tcl_NewStringObj("JDITHER_NONE", -1));
			break;

		case JDITHER_ORDERED:
			ADD_RES_ITEM("dither_mode", Tcl_NewStringObj("JDITHER_ORDERED", -1));
			break;

		case JDITHER_FS:
			ADD_RES_ITEM("dither_mode", Tcl_NewStringObj("JDITHER_FS", -1));
			break;

		default:
			ADD_RES_ITEM("dither_mode", Tcl_NewStringObj("JDITHER_UNKNOWN", -1));
			break;
	}
	ADD_RES_ITEM("two_pass_quantize", Tcl_NewBooleanObj(cinfo.two_pass_quantize));
	ADD_RES_ITEM("desired_number_of_colors", Tcl_NewIntObj(cinfo.desired_number_of_colors));
	ADD_RES_ITEM("enable_1pass_quant", Tcl_NewBooleanObj(cinfo.enable_1pass_quant));
	ADD_RES_ITEM("enable_external_quant", Tcl_NewBooleanObj(cinfo.enable_external_quant));
	ADD_RES_ITEM("enable_2pass_quant", Tcl_NewBooleanObj(cinfo.enable_2pass_quant));
	ADD_RES_ITEM("output_width", Tcl_NewIntObj(cinfo.output_width));
	ADD_RES_ITEM("output_height", Tcl_NewIntObj(cinfo.output_height));
	ADD_RES_ITEM("out_color_components", Tcl_NewIntObj(cinfo.out_color_components));
	ADD_RES_ITEM("output_components", Tcl_NewIntObj(cinfo.output_components));
	ADD_RES_ITEM("rec_outbuf_height", Tcl_NewIntObj(cinfo.rec_outbuf_height));
	ADD_RES_ITEM("actual_number_of_colors", Tcl_NewIntObj(cinfo.actual_number_of_colors));
//	ADD_RES_ITEM("colormap", Tcl_NewIntObj(cinfo.));
	ADD_RES_ITEM("input_scan_number", Tcl_NewIntObj(cinfo.input_scan_number));
	ADD_RES_ITEM("input_iMCU_row", Tcl_NewIntObj(cinfo.input_iMCU_row));
	ADD_RES_ITEM("output_scan_number", Tcl_NewIntObj(cinfo.output_scan_number));
	ADD_RES_ITEM("output_iMCU_row", Tcl_NewIntObj(cinfo.output_iMCU_row));
	ADD_RES_ITEM("data_precision", Tcl_NewIntObj(cinfo.data_precision));

	Tcl_SetObjResult(interp, res);

	jpeg_destroy_decompress(&cinfo);

	return TCL_OK;
}


// Init {{{1
int Pixel_jpeg_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	NEW_CMD("pixel::jpeg::loadjpeg", glue_loadjpeg);
	NEW_CMD("pixel::jpeg::savejpeg", glue_savejpeg);

	NEW_CMD("pixel::jpeg::encodejpeg", glue_encodejpeg);
	NEW_CMD("pixel::jpeg::decodejpeg", glue_decodejpeg);

	NEW_CMD("pixel::jpeg::jpeg_info", glue_jpeg_info);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}


