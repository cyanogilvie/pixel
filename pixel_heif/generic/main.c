#include <stdint.h>
#include <libheif/heif.h>
#include "tclstuff.h"
#include <pixel.h>

static struct heif_error g_ok = {
	code: heif_error_Ok,
	subcode: heif_suberror_Unspecified,
	message: "Ok"
};

static int glue_decode(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	struct heif_error			res;
	struct heif_context*		ctx = NULL;
	int							len;
	unsigned char*				bytes;
	struct heif_image_handle*	handle = NULL;
	struct heif_image*			img = NULL;
	gimp_image_t*				new = NULL;
	const uint8_t*				data = NULL;
	int							width, height, stride, code=TCL_OK;

	CHECK_ARGS(1, "bytes");
	bytes = Tcl_GetByteArrayFromObj(objv[1], &len);

	ctx = heif_context_alloc();
	res = heif_context_read_from_memory_without_copy(ctx, bytes, len, NULL);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error decoding from memory: ", res.message);

	// get a handle to the primary image
	res = heif_context_get_primary_image_handle(ctx, &handle);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error getting handle to primary image: ", res.message);

	// decode the image and convert colorspace to RGBA, saved as 32bit interleaved
	res = heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, NULL);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error decoding image: ", res.message);

	width = heif_image_get_width(img, heif_channel_interleaved);
	height = heif_image_get_height(img, heif_channel_interleaved);

	data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);

	new = (gimp_image_t*)malloc(sizeof(gimp_image_t));
	new->bytes_per_pixel = 4;
	new->width = width;
	new->height = height;
	new->pixel_data = (_pel*)malloc(width*height*4);

	{
		_pel* restrict	d = new->pixel_data;
		int	x, y;
		const int	w=width, h=height;

		for (y=0; y<h; y++) {
			_pel* restrict	s = (_pel*)((uint8_t*)data + y*stride);

			for (x=0; x<w; x++, d++, s++) {
				d->chan[0] = s->chan[2];
				d->chan[1] = s->chan[1];
				d->chan[2] = s->chan[0];
				d->chan[3] = s->chan[3];
			}
		}
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(new));

finally:
	if (handle) {heif_image_handle_release(handle); handle = NULL;}
	if (img)    {heif_image_release(img); img = NULL;}
	if (ctx)    {heif_context_free(ctx); ctx = NULL;}

	return code;
}

//}}}
static struct heif_error _tclobj_writer(struct heif_context* ctx, const void* data, size_t size, void* userdata) //{{{
{
	Tcl_Obj*	bytes = (Tcl_Obj*)userdata;
	Tcl_Obj*	chunk = Tcl_NewByteArrayObj(data, size);

	Tcl_AppendObjToObj(bytes, chunk);

	return g_ok;
}

//}}}
static int glue_encode(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	struct heif_error		res;
	gimp_image_t*			pmap = NULL;
	int						quality;
	struct heif_context*	ctx = NULL;
	struct heif_encoder*	encoder = NULL;
	struct heif_image*		img = NULL;
	int						code = TCL_OK, stride;
	uint8_t*				data = NULL;
	Tcl_Obj*				bytes = NULL;
	struct heif_writer		writer;

	CHECK_ARGS(2, "pmap quality");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &quality));

	ctx = heif_context_alloc();

	// get the default encoder
	res = heif_context_get_encoder_for_format(ctx, heif_compression_HEVC, &encoder);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error getting encoder: ", res.message);

	// set the encoder parameters
	res = heif_encoder_set_lossy_quality(encoder, quality);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error setting quality: ", res.message);

	res = heif_image_create(pmap->width, pmap->height, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, &img);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error allocating image: ", res.message);

	res = heif_image_add_plane(img, heif_channel_interleaved, pmap->width, pmap->height, 32);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error adding plane: ", res.message);

	data = heif_image_get_plane(img, heif_channel_interleaved, &stride);

	{
		_pel* restrict	s = pmap->pixel_data;
		int	x, y;
		const int	w=pmap->width, h=pmap->height;

		for (y=0; y<h; y++) {
			_pel* restrict	d = (_pel*)((uint8_t*)data + y*stride);

			for (x=0; x<w; x++, d++, s++) {
				d->chan[0] = s->chan[2];
				d->chan[1] = s->chan[1];
				d->chan[2] = s->chan[0];
				d->chan[3] = s->chan[3];
			}
		}
	}

	// encode the img
	res = heif_context_encode_image(ctx, img, encoder, NULL, NULL);
	if (res.code != heif_error_Ok)
		THROW_ERROR_LABEL(finally, code, "Error encoding image: ", res.message);

	bytes = Tcl_NewByteArrayObj(NULL, 0);

	writer.writer_api_version = 1;
	writer.write = _tclobj_writer;

	res = heif_context_write(ctx, &writer, bytes);

	Tcl_SetObjResult(interp, bytes);

finally:
	if (encoder) {heif_encoder_release(encoder); encoder = NULL;}
	if (img)     {heif_image_release(img); img = NULL;}
	if (ctx)     {heif_context_free(ctx); ctx = NULL;}

	return code;
}

//}}}
int Pixel_heif_Init(Tcl_Interp* interp) //{{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	NEW_CMD("pixel::heif::decode", glue_decode);
	NEW_CMD("pixel::heif::encode", glue_encode);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
