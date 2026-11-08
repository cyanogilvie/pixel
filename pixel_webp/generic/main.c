#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdint.h>
#include <webp/encode.h>
#include <webp/decode.h>
#include <tclstuff.h>
#include <pixel.h>
#include <parse_args.h>
#include <defer.h>		// parseargs_bind.def releases the values it binds with defer


// parseargs_bind.def out-type PMAP: a pmap argument, as the gimp_image_t* in
// its Tcl_Obj's internal rep (kept alive by the slot's hold on the Tcl_Obj)
static int conv_pmap(void* /*cdata*/, Tcl_Interp* interp, Tcl_Obj* val, void* addr) //{{{
{
	return Tcl_GetPMAPFromObj(interp, val, (gimp_image_t**)addr);
}

//}}}
#define PARSEARGS_OUTTYPE_PMAP(name)		gimp_image_t*	name;
#define PARSEARGS_SLOTFOR_PMAP(out, name)	{.type = PARSEARGS_SLOT_CUSTOM, .addr = &(out).name, .convert = conv_pmap}

static const char* encoding_error_name(WebPEncodingError err) //{{{
{
	switch (err) {
		case VP8_ENC_OK:							return "OK";
		case VP8_ENC_ERROR_OUT_OF_MEMORY:			return "OUT_OF_MEMORY";
		case VP8_ENC_ERROR_BITSTREAM_OUT_OF_MEMORY:	return "BITSTREAM_OUT_OF_MEMORY";
		case VP8_ENC_ERROR_NULL_PARAMETER:			return "NULL_PARAMETER";
		case VP8_ENC_ERROR_INVALID_CONFIGURATION:	return "INVALID_CONFIGURATION";
		case VP8_ENC_ERROR_BAD_DIMENSION:			return "BAD_DIMENSION";
		case VP8_ENC_ERROR_PARTITION0_OVERFLOW:		return "PARTITION0_OVERFLOW";
		case VP8_ENC_ERROR_PARTITION_OVERFLOW:		return "PARTITION_OVERFLOW";
		case VP8_ENC_ERROR_BAD_WRITE:				return "BAD_WRITE";
		case VP8_ENC_ERROR_FILE_TOO_BIG:			return "FILE_TOO_BIG";
		case VP8_ENC_ERROR_USER_ABORT:				return "USER_ABORT";
		default:									return "UNKNOWN";
	}
}

//}}}
static int encoding_error(Tcl_Interp* interp, const char* what, WebPEncodingError err) //{{{
{
	const char*	name = encoding_error_name(err);
	Tcl_SetErrorCode(interp, "PIXEL", "WEBP", "ENCODE", name, NULL);
	Tcl_SetObjResult(interp, Tcl_ObjPrintf("webp %s failed: %s", what, name));
	return TCL_ERROR;
}

//}}}
static int check_range(Tcl_Interp* interp, const char* name, double val, double min, double max) //{{{
{
	if (val >= min && val <= max) return TCL_OK;
	Tcl_SetErrorCode(interp, "PIXEL", "WEBP", "RANGE", name, NULL);
	Tcl_SetObjResult(interp, Tcl_ObjPrintf("bad %s \"%g\": must be between %g and %g", name, val, min, max));
	return TCL_ERROR;
}

//}}}
static Tcl_Obj* int_list(int n, const int* vals) //{{{
{
	Tcl_Obj*	res = Tcl_NewListObj(0, NULL);
	for (int i=0; i<n; i++) Tcl_ListObjAppendElement(NULL, res, Tcl_NewIntObj(vals[i]));
	return res;
}

//}}}
static Tcl_Obj* stats_dict(const WebPAuxStats* s) //{{{
{
	Tcl_Obj*	d = Tcl_NewDictObj();
	Tcl_Obj*	psnr = Tcl_NewDictObj();
	const char*	psnr_keys[] = {"y", "u", "v", "all", "alpha"};

#define PUT(key, obj)	Tcl_DictObjPut(NULL, d, Tcl_NewStringObj(key, -1), (obj))
	PUT("coded_size",					Tcl_NewIntObj(s->coded_size));
	PUT("alpha_data_size",				Tcl_NewIntObj(s->alpha_data_size));
	for (int i=0; i<5; i++)
		Tcl_DictObjPut(NULL, psnr, Tcl_NewStringObj(psnr_keys[i], -1), Tcl_NewDoubleObj(s->PSNR[i]));
	PUT("psnr",							psnr);
	// Lossy (VP8) colour data
	PUT("block_count",					int_list(3, s->block_count));	// intra4 intra16 skipped
	PUT("header_bytes",					int_list(2, s->header_bytes));	// header, mode partition 0
	PUT("segment_size",					int_list(4, s->segment_size));
	PUT("segment_quant",				int_list(4, s->segment_quant));
	PUT("segment_level",				int_list(4, s->segment_level));
	// Lossless (VP8L) data: the image when -lossless, otherwise the alpha plane
	PUT("lossless_features",			Tcl_NewWideIntObj(s->lossless_features));
	PUT("histogram_bits",				Tcl_NewIntObj(s->histogram_bits));
	PUT("transform_bits",				Tcl_NewIntObj(s->transform_bits));
	PUT("cross_color_transform_bits",	Tcl_NewIntObj(s->cross_color_transform_bits));
	PUT("cache_bits",					Tcl_NewIntObj(s->cache_bits));
	PUT("palette_size",					Tcl_NewIntObj(s->palette_size));
	PUT("lossless_size",				Tcl_NewIntObj(s->lossless_size));
	PUT("lossless_hdr_size",			Tcl_NewIntObj(s->lossless_hdr_size));
	PUT("lossless_data_size",			Tcl_NewIntObj(s->lossless_data_size));
#undef PUT

	return d;
}

//}}}
static void free_argspec_cache(void* cdata) //{{{
{
	Tcl_Obj**	cache = (Tcl_Obj**)cdata;
	release_tclobj(cache);
	ckfree(cache);
}

//}}}

/*
 * pixel::webp::encode ?-option value ...? pmap quality
 *
 * quality is the lossy quality (0-100), or with -lossless the effort spent
 * compressing (0 fastest, 100 smallest).  With no options the result is
 * what WebPEncodeBGRA gives: lossy, method 4, lossless alpha.  The alpha
 * channel is only written if some pixel isn't opaque, unless -alpha drop
 * (ignore the pmap's alpha) or -background (blend over that 0xRRGGBB colour)
 * remove it.  The other options are the WebPConfig fields of the same name;
 * -preset picks the WebPConfigPreset the configuration starts from.  -stats
 * names a variable to receive a dict of the encoder's WebPAuxStats.  The
 * boolean WebPConfig fields (-lossless, -exact, -use_sharp_yuv, -low_memory)
 * are flags: present sets them.
 */
static int glue_encodewebp(void* cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	enum {ALPHA_AUTO, ALPHA_DROP};
	WebPConfig			config;
	WebPPicture			pic;
	WebPMemoryWriter	writer;
	WebPAuxStats		stats;
	int					background = -1;
	int					ok;

	#define PARSEARGS_SPEC(OPT, ARG) \
		OPT( preset,			"-default default -enum {default picture photo drawing icon text}",	INDEX	) \
		OPT( method,			"-default 4",							INT		) \
		OPT( lossless,			"-boolean",								BOOL	) \
		OPT( near_lossless,		"-default 100",							INT		) \
		OPT( alpha,				"-default auto -enum {auto drop}",		INDEX	) \
		OPT( background,		"",										OBJ		) \
		OPT( alpha_quality,		"-default 100",							INT		) \
		OPT( alpha_filtering,	"-default fast -enum {none fast best}",	INDEX	) \
		OPT( exact,				"-boolean",								BOOL	) \
		OPT( use_sharp_yuv,		"-boolean",								BOOL	) \
		OPT( thread_level,		"-default 0",							INT		) \
		OPT( low_memory,		"-boolean",								BOOL	) \
		OPT( target_size,		"-default 0",							INT		) \
		OPT( target_psnr,		"-default 0",							DOUBLE	) \
		OPT( pass,				"-default 1",							INT		) \
		OPT( stats,				"",										OBJ		) \
		ARG( pmap,				"-required",							PMAP	) \
		ARG( quality,			"-required",							DOUBLE	) \
		/* line intentionally left blank */
	#include <parseargs_bind.def>

	TEST_OK(check_range(interp, "quality",			args.quality,		0, 100));
	TEST_OK(check_range(interp, "-method",			args.method,		0, 6));
	TEST_OK(check_range(interp, "-near_lossless",	args.near_lossless,	0, 100));
	TEST_OK(check_range(interp, "-alpha_quality",	args.alpha_quality,	0, 100));
	TEST_OK(check_range(interp, "-target_size",		args.target_size,	0, INT32_MAX));
	TEST_OK(check_range(interp, "-target_psnr",		args.target_psnr,	0, 99));
	TEST_OK(check_range(interp, "-pass",			args.pass,			1, 10));
	// libwebp 1.6 takes 0 or 1, but the field is a level: leave the upper bound
	// to WebPValidateConfig so that a libwebp with more levels can use them
	TEST_OK(check_range(interp, "-thread_level",	args.thread_level,	0, INT32_MAX));
	if (args.near_lossless < 100 && !args.lossless)
		THROW_ERROR("-near_lossless only applies with -lossless");
	if (args.background) {
		if (args.alpha == ALPHA_DROP)
			THROW_ERROR("-background and -alpha drop are exclusive");
		TEST_OK(Tcl_GetIntFromObj(interp, args.background, &background));
		TEST_OK(check_range(interp, "-background", background, 0, 0xffffff));
	}
	if (args.pmap->bytes_per_pixel != 4)
		THROW_PRINTF("pmap has %d bytes per pixel, expecting 4", (int)args.pmap->bytes_per_pixel);

	// Configuration
	if (!WebPConfigPreset(&config, (WebPPreset)args.preset, (float)args.quality))
		THROW_ERROR("libwebp version mismatch");
	config.method			= args.method;
	config.lossless			= args.lossless;
	config.near_lossless	= args.near_lossless;
	config.alpha_quality	= args.alpha_quality;
	config.alpha_filtering	= args.alpha_filtering;
	config.exact			= args.exact;
	config.use_sharp_yuv	= args.use_sharp_yuv;
	config.thread_level		= args.thread_level;
	config.low_memory		= args.low_memory;
	config.target_size		= args.target_size;
	config.target_PSNR		= (float)args.target_psnr;
	config.pass				= args.pass;
	if (!WebPValidateConfig(&config))
		return encoding_error(interp, "configuration", VP8_ENC_ERROR_INVALID_CONFIGURATION);

	// Picture
	if (!WebPPictureInit(&pic))
		THROW_ERROR("libwebp version mismatch");
	defer { WebPPictureFree(&pic); }
	WebPMemoryWriterInit(&writer);
	defer { WebPMemoryWriterClear(&writer); }

	pic.width		= args.pmap->width;
	pic.height		= args.pmap->height;
	pic.writer		= WebPMemoryWrite;
	pic.custom_ptr	= &writer;
	if (args.stats) pic.stats = &stats;
	// As cwebp does: lossless needs the ARGB samples, and sharp YUV is done by
	// WebPEncode from them, so don't convert to YUV on import
	pic.use_argb	= config.lossless || config.use_sharp_yuv;

	if (args.alpha == ALPHA_DROP) {
		ok = WebPPictureImportBGRX(&pic, (const uint8_t*)args.pmap->pixel_data, pic.width*4);
	} else {
		ok = WebPPictureImportBGRA(&pic, (const uint8_t*)args.pmap->pixel_data, pic.width*4);
	}
	if (!ok) return encoding_error(interp, "import", pic.error_code);
	if (background != -1) WebPBlendAlpha(&pic, (uint32_t)background);	// Leaves every alpha at 0xff

	if (!WebPEncode(&config, &pic))
		return encoding_error(interp, "encode", pic.error_code);

	if (args.stats) {
		if (Tcl_ObjSetVar2(interp, args.stats, NULL, stats_dict(&stats), TCL_LEAVE_ERR_MSG) == NULL)
			return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(writer.mem, writer.size));
	return TCL_OK;
}

//}}}
static const char* vp8_status_name(VP8StatusCode status) //{{{
{
	switch (status) {
		case VP8_STATUS_OK:					return "OK";
		case VP8_STATUS_OUT_OF_MEMORY:		return "OUT_OF_MEMORY";
		case VP8_STATUS_INVALID_PARAM:		return "INVALID_PARAM";
		case VP8_STATUS_BITSTREAM_ERROR:	return "BITSTREAM_ERROR";
		case VP8_STATUS_UNSUPPORTED_FEATURE:return "UNSUPPORTED_FEATURE";
		case VP8_STATUS_SUSPENDED:			return "SUSPENDED";
		case VP8_STATUS_USER_ABORT:			return "USER_ABORT";
		case VP8_STATUS_NOT_ENOUGH_DATA:	return "NOT_ENOUGH_DATA";
		default:							return "UNKNOWN";
	}
}

//}}}
static int decoding_error(Tcl_Interp* interp, const char* name, const char* msg) //{{{
{
	Tcl_SetErrorCode(interp, "PIXEL", "WEBP", "DECODE", name, NULL);
	Tcl_SetObjResult(interp, Tcl_ObjPrintf("webp decode failed: %s", msg));
	return TCL_ERROR;
}

//}}}
static int get_features(Tcl_Interp* interp, const uint8_t* data, size_t len, WebPBitstreamFeatures* features) //{{{
{
	const VP8StatusCode	status = WebPGetFeatures(data, len, features);
	if (status != VP8_STATUS_OK)
		return decoding_error(interp, vp8_status_name(status), vp8_status_name(status));
	return TCL_OK;
}

//}}}

/*
 * pixel::webp::decode ?-option value ...? webp_data
 *
 * Decode a (still) webp image into a pmap.  The options are the
 * WebPDecoderOptions fields of the same name, except -scale width height,
 * which sets use_scaling, scaled_width and scaled_height: a 0 dimension is
 * derived from the other, keeping the aspect ratio.
 */
static int glue_decodewebp(void* cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	WebPDecoderConfig	config;
	VP8StatusCode		status;
	gimp_image_t*		pmap;
	int					width, height;

	#define PARSEARGS_SPEC(OPT, ARG) \
		OPT( scale,						"-args 2",		LIST	) \
		OPT( use_threads,				"-boolean",		BOOL	) \
		OPT( dithering_strength,		"-default 0",	INT		) \
		OPT( alpha_dithering_strength,	"-default 0",	INT		) \
		ARG( webp_data,					"-required",	BYTES	) \
		/* line intentionally left blank */
	#include <parseargs_bind.def>

	TEST_OK(check_range(interp, "-dithering_strength",			args.dithering_strength,		0, 100));
	TEST_OK(check_range(interp, "-alpha_dithering_strength",	args.alpha_dithering_strength,	0, 100));

	if (!WebPInitDecoderConfig(&config))
		THROW_ERROR("libwebp version mismatch");
	TEST_OK(get_features(interp, args.webp_data, args.webp_data_len, &config.input));
	if (config.input.has_animation)
		return decoding_error(interp, "ANIMATION", "animated webp isn't supported");

	width	= config.input.width;
	height	= config.input.height;
	if (args.scalec) {
		int		sw, sh;
		TEST_OK(Tcl_GetIntFromObj(interp, args.scalev[0], &sw));
		TEST_OK(Tcl_GetIntFromObj(interp, args.scalev[1], &sh));
		TEST_OK(check_range(interp, "-scale width",		sw, 0, WEBP_MAX_DIMENSION));
		TEST_OK(check_range(interp, "-scale height",	sh, 0, WEBP_MAX_DIMENSION));
		if (sw == 0 && sh == 0)
			THROW_ERROR("-scale needs a width or a height");
		// The output size, as libwebp's WebPRescalerGetScaledDimensions derives it
		if (sw == 0) sw = (int)(((uint64_t)width * sh + height - 1) / height);
		if (sh == 0) sh = (int)(((uint64_t)height * sw + width - 1) / width);
		config.options.use_scaling		= 1;
		config.options.scaled_width		= width		= sw;
		config.options.scaled_height	= height	= sh;
	}
	config.options.use_threads				= args.use_threads;
	config.options.dithering_strength		= args.dithering_strength;
	config.options.alpha_dithering_strength	= args.alpha_dithering_strength;

	// Decode into a buffer Pixel allocated (aligned for its SIMD code, and freed
	// by pmap_free), rather than adopting one from libwebp's allocator
	pmap = pmap_new(width, height, (_pel){.c = 0});
	config.output.colorspace			= MODE_BGRA;
	config.output.is_external_memory	= 1;
	config.output.u.RGBA.rgba			= (uint8_t*)pmap->pixel_data;
	config.output.u.RGBA.stride			= width * 4;
	config.output.u.RGBA.size			= (size_t)width * height * 4;

	status = WebPDecode(args.webp_data, args.webp_data_len, &config);
	WebPFreeDecBuffer(&config.output);		// Nothing to free with external memory, but by the book
	if (status != VP8_STATUS_OK) {
		pmap_free(&pmap);
		return decoding_error(interp, vp8_status_name(status), status == VP8_STATUS_BITSTREAM_ERROR ? "corrupt webp data" : vp8_status_name(status));
	}

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));
	return TCL_OK;
}

//}}}

/*
 * pixel::webp::info webp_data
 *
 * A dict of what the bitstream header says: width, height, has_alpha,
 * has_animation and format (lossy, lossless or mixed: an animation's frames
 * can be either)
 */
static int glue_infowebp(void* cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	WebPBitstreamFeatures	features;
	const char*				formats[] = {"mixed", "lossy", "lossless"};

	#define PARSEARGS_SPEC(OPT, ARG) \
		ARG( webp_data,		"-required",	BYTES	) \
		/* line intentionally left blank */
	#include <parseargs_bind.def>

	TEST_OK(get_features(interp, args.webp_data, args.webp_data_len, &features));

	Tcl_Obj*	res = Tcl_NewDictObj();
#define PUT(key, obj)	Tcl_DictObjPut(NULL, res, Tcl_NewStringObj(key, -1), (obj))
	PUT("width",			Tcl_NewIntObj(features.width));
	PUT("height",			Tcl_NewIntObj(features.height));
	PUT("has_alpha",		Tcl_NewBooleanObj(features.has_alpha));
	PUT("has_animation",	Tcl_NewBooleanObj(features.has_animation));
	PUT("format",			Tcl_NewStringObj(features.format >= 0 && features.format <= 2 ? formats[features.format] : "unknown", -1));
#undef PUT
	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

//}}}
// A command binding its args with parseargs_bind.def: its cdata is the
// Tcl_Obj* slot (parseargs_bind.def's default PARSEARGS_CACHE) that caches the
// compiled argspec.  Per command, so per interp: the argspec Tcl_Obj can't be
// shared between threads
static void parseargs_cmd(Tcl_Interp* interp, const char* name, Tcl_ObjCmdProc* proc) //{{{
{
	Tcl_Obj**	cache = (Tcl_Obj**)ckalloc(sizeof *cache);
	*cache = NULL;
	Tcl_CreateObjCommand(interp, name, proc, cache, free_argspec_cache);
}

//}}}
int Pixel_webp_Init(Tcl_Interp* interp) //{{{
{
	if (!Tcl_InitStubs(interp, TCL_VERSION, 0)) return TCL_ERROR;
	if (!Pixel_InitStubs(interp, "4.0", 0)) return TCL_ERROR;
	if (!ParseArgs_InitStubs(interp)) return TCL_ERROR;

	parseargs_cmd(interp, "pixel::webp::encode",	glue_encodewebp);
	parseargs_cmd(interp, "pixel::webp::decode",	glue_decodewebp);
	parseargs_cmd(interp, "pixel::webp::info",		glue_infowebp);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}

// vim: foldmethod=marker foldmarker={{{,}}}
