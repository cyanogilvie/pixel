#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include <stdio.h>
#include <jpeglib.h>
#include "2d.h"
#include "tcl_pmap.h"
#include <tclstuff.h>


#define JPEG_ENCODE_BUFSIZE		65520



gimp_image_t *loadjpeg(char *filename) // {{{1
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;
	gimp_image_t	*dest = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	FILE			*fp;
	JSAMPLE			*buffer;
	int				row_stride, i;
	_pel			*r;
	JSAMPLE			*j;
	

	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);
	(void)jpeg_read_header(&cinfo, TRUE);
	(void)jpeg_start_decompress(&cinfo);

	dest->width = cinfo.output_width;
	dest->height = cinfo.output_height;
	dest->bytes_per_pixel = 4;
	r = dest->pixel_data = (_pel *)malloc(dest->width*dest->height*4);
	
	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (JSAMPLE *)malloc(row_stride);

	while (cinfo.output_scanline < cinfo.output_height) {
		(void)jpeg_read_scanlines(&cinfo, &buffer, 1);
		j = (JSAMPLE *)buffer;
		for (i=0; i<dest->width; i++) {
			r->ch.r = (uint8)*j++;
			r->ch.g = (uint8)*j++;
			r->ch.b = (uint8)*j++;
			r->ch.a = 0xff;
			r++;
		}
	}

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	free(buffer);

	return dest;
}


int savejpeg(char *filename, gimp_image_t *pmap, int quality) // {{{1
{
	struct jpeg_compress_struct		cinfo;
	struct jpeg_error_mgr			jerr;
	FILE		*fp;
	JSAMPROW	row_pointer[1];
	int			row_stride, i;
	_pel		*s;
	JSAMPLE		*j;
	JSAMPLE		*buffer;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	if ((fp = fopen(filename, "wb")) == NULL)
		return -2;

	jpeg_stdio_dest(&cinfo, fp);

	cinfo.image_width = pmap->width;
	cinfo.image_height = pmap->height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = cinfo.image_width * cinfo.input_components;

	buffer = (JSAMPLE *)malloc(row_stride);
	
	s = pmap->pixel_data;
	
	while (cinfo.next_scanline < cinfo.image_height) {
		j = buffer;
		for (i=0; i<cinfo.image_width; i++) {
			*j++ = s->ch.r;
			*j++ = s->ch.g;
			*j++ = s->ch.b;
			s++;
		}
		(void)jpeg_write_scanlines(&cinfo, &buffer, 1);
	}

	jpeg_finish_compress(&cinfo);
	free(buffer);
	fclose(fp);
	jpeg_destroy_compress(&cinfo);

	return 0;
}


// Global vars for dest manager {{{1
static unsigned char	*g_buf;
static int				g_buf_size;
static unsigned char	*g_buf_tail;
static JOCTET			*g_buf_start;

static void init_dest(j_compress_ptr cinfo) //{{{1
{
	cinfo->dest->next_output_byte = g_buf_start = \
		(JOCTET *)malloc(JPEG_ENCODE_BUFSIZE);
	cinfo->dest->free_in_buffer = JPEG_ENCODE_BUFSIZE;
	g_buf_size = JPEG_ENCODE_BUFSIZE;
	g_buf_tail = g_buf = (unsigned char *)malloc(g_buf_size);
}


static void flush_remaining(j_compress_ptr cinfo) //{{{1
{
	int			avail = JPEG_ENCODE_BUFSIZE - cinfo->dest->free_in_buffer;

	cinfo->dest->next_output_byte -= avail;

	if (g_buf_tail - g_buf + avail > g_buf_size) {
		g_buf_size += JPEG_ENCODE_BUFSIZE;
		realloc(g_buf, g_buf_size);
	}
	memcpy(g_buf_tail, cinfo->dest->next_output_byte, avail);
	cinfo->dest->free_in_buffer = JPEG_ENCODE_BUFSIZE;
	g_buf_tail += avail;
}


static boolean empty_buffer(j_compress_ptr cinfo) //{{{1
{
	int			avail = JPEG_ENCODE_BUFSIZE;

	cinfo->dest->next_output_byte = g_buf_start;

	if (g_buf_tail - g_buf + avail > g_buf_size) {
		g_buf_size += JPEG_ENCODE_BUFSIZE;
		realloc(g_buf, g_buf_size);
	}
	memcpy(g_buf_tail, cinfo->dest->next_output_byte, avail);
	cinfo->dest->free_in_buffer = JPEG_ENCODE_BUFSIZE;
	g_buf_tail += avail;

	return TRUE;
}


static void term_dest(j_compress_ptr cinfo) //{{{1
{
	flush_remaining(cinfo);
}


static void cleanup_dest() //{{{1
{
	free(g_buf);
	g_buf_tail = g_buf = NULL;
	g_buf_size = 0;
}


unsigned char *encodejpeg(gimp_image_t *pmap, int *length, int quality) //{{{1
{
	struct jpeg_compress_struct		cinfo;
	struct jpeg_error_mgr			jerr;
	struct jpeg_destination_mgr		jdest;
	FILE		*fp;
	JSAMPROW	row_pointer[1];
	int			row_stride, i;
	_pel		*s;
	JSAMPLE		*j;
	JSAMPLE		*buffer;
	unsigned char	*odata;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	jdest.init_destination = init_dest;
	jdest.empty_output_buffer = empty_buffer;
	jdest.term_destination = term_dest;

	cinfo.dest = &jdest;
	cinfo.image_width = pmap->width;
	cinfo.image_height = pmap->height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = cinfo.image_width * cinfo.input_components;

	buffer = (JSAMPLE *)malloc(row_stride);
	
	s = pmap->pixel_data;
	
	while (cinfo.next_scanline < cinfo.image_height) {
		j = buffer;
		for (i=0; i<cinfo.image_width; i++) {
			*j++ = s->ch.r;
			*j++ = s->ch.g;
			*j++ = s->ch.b;
			s++;
		}
		(void)jpeg_write_scanlines(&cinfo, &buffer, 1);
	}

	jpeg_finish_compress(&cinfo);

	*length = g_buf_tail - g_buf;
	odata = (unsigned char *)malloc(*length);
	memcpy(odata, g_buf, *length);

	cleanup_dest();
	
	free(buffer);
	jpeg_destroy_compress(&cinfo);

	return odata;
}


// global vars for source manager {{{1
static JOCTET	*g_src_buf;
static int		g_src_buf_size;

static void init_source(j_decompress_ptr cinfo) //{{{1
{
	cinfo->src->next_input_byte = g_src_buf;
	cinfo->src->bytes_in_buffer = g_src_buf_size;
}


static boolean fill_buffer(j_decompress_ptr cinfo) //{{{1
{
	fprintf(stderr, "Error: fill_input_buffer called when it shouldn't have been");
	return TRUE;
}


static void skip_input_data(j_decompress_ptr cinfo, long num_bytes) // {{{1
{
	if (num_bytes <= 0) return;
	g_src_buf += num_bytes;
}


static void term_source(j_decompress_ptr cinfo) // {{{1
{
}


gimp_image_t *decodejpeg(unsigned char *jpeg_data, int *length) // {{{1
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;
	struct jpeg_source_mgr			jsrc;
	gimp_image_t	*dest = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	FILE			*fp;
	JSAMPLE			*buffer;
	int				row_stride, i;
	_pel			*r;
	JSAMPLE			*j;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	g_src_buf = jpeg_data;
	g_src_buf_size = *length;
	
	jsrc.init_source = init_source;
	jsrc.fill_input_buffer = fill_buffer;
	jsrc.skip_input_data = skip_input_data;
	jsrc.resync_to_restart = jpeg_resync_to_restart;
	jsrc.term_source = term_source;
	
	cinfo.src = &jsrc;
	(void)jpeg_read_header(&cinfo, TRUE);
	(void)jpeg_start_decompress(&cinfo);

	dest->width = cinfo.output_width;
	dest->height = cinfo.output_height;
	dest->bytes_per_pixel = 4;
	r = dest->pixel_data = (_pel *)malloc(dest->width*dest->height*4);
	
	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (JSAMPLE *)malloc(row_stride);

	while (cinfo.output_scanline < cinfo.output_height) {
		(void)jpeg_read_scanlines(&cinfo, &buffer, 1);
		j = (JSAMPLE *)buffer;
		for (i=0; i<dest->width; i++) {
			r->ch.r = (uint8)*j++;
			r->ch.g = (uint8)*j++;
			r->ch.b = (uint8)*j++;
			r->ch.a = 0xff;
			r++;
		}
	}

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	free(buffer);

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
	int				quality, length;
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

	new = decodejpeg(jpeg_data, &length);

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

	jpeg_destroy(&cinfo);

	return TCL_OK;
}


// Init {{{1
int Pixel_jpeg_Init(Tcl_Interp *interp)
{
	NEW_CMD("pixel::jpeg::loadjpeg", glue_loadjpeg);
	NEW_CMD("pixel::jpeg::savejpeg", glue_savejpeg);

	NEW_CMD("pixel::jpeg::encodejpeg", glue_encodejpeg);
	NEW_CMD("pixel::jpeg::decodejpeg", glue_decodejpeg);
	
	NEW_CMD("pixel::jpeg::jpeg_info", glue_jpeg_info);
	
	return TCL_OK;
}


