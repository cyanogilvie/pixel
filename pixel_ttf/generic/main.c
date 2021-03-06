#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

//#include <Pixel/pixel.h>
#include <tcl.h>
#include <pixel.h>

typedef struct _ttf_feedback_data {
	Tcl_Interp	*interp;
	Tcl_Obj		*res;
	Tcl_Obj		*line_no;
} ttf_feedback_data;

#include "ttf.h"

static Tcl_Obj*		g_key_base_col = NULL;
static Tcl_Obj*		g_key_face = NULL;
static Tcl_Obj*		g_key_px_size = NULL;
static Tcl_Obj*		g_key_px_width = NULL;
uint8_t		fact[256][256];			// [a][b] = a * (b/255.0)

// render_ttf colour fft_face px_size text ?width? {{{1
static int glue_render_ttf(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int				px_size;
	pmap_list		*pmaps;
	pmap_list		*next;
	_pel			base_col;
	FT_Face			face;
	int				len, width;
	char			*utf8;
	Tcl_Obj			*res;
	
	
	if (objc < 5 || objc > 6)
		CHECK_ARGS(4, "colour fft_face px_size text ?width?");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], (int *)&base_col));
	TEST_OK(Tcl_GetTTFFaceFromObj(interp, objv[2], &face));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &px_size));
	utf8 = Tcl_GetStringFromObj(objv[4], &len);
	if (objc == 6) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[5], &width));
	} else {
		width = 0;
	}
	pmaps = render_ttf(base_col, face, px_size, 
			Tcl_GetString(objv[4]), width, NULL, NULL);

	if (pmaps == NULL)
		THROW_ERROR("Could not render text: ", Tcl_NewStringObj(ttf_last_error_txt(), -1));

	if (width == 0) {
		Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmaps->pmap));
		free(pmaps);
	} else {
		res = Tcl_NewListObj(0, NULL);

		while (1) {
			Tcl_ListObjAppendElement(interp, res, Tcl_NewPMAPObj(pmaps->pmap));
			if (pmaps->next == NULL) {
				break;
			} else {
				next = pmaps->next;
				free(pmaps);
				pmaps = next;
			}
		}
		free(pmaps);

		Tcl_SetObjResult(interp, res);
	}

	return TCL_OK;
}


// render_ttf_adv meta text {{{1

static void render_ttf_feedback(void *clientdata, int what, int value)
{
	ttf_feedback_data	*feedback = (ttf_feedback_data *)clientdata;

	switch (what) {
		case TTF_FEEDBACK_LINESTART:
			//fprintf(stderr, "render_ttf_feedback: TTF_FEEDBACK_LINESTART: %d\n", value);
			Tcl_ListObjAppendElement(feedback->interp, feedback->res,
					Tcl_NewIntObj(value));
			Tcl_ListObjAppendElement(feedback->interp, feedback->res,
					feedback->line_no);
			feedback->line_no = Tcl_NewListObj(0, NULL);
			break;

		case TTF_FEEDBACK_CHAR:
			//fprintf(stderr, "render_ttf_feedback: TTF_FEEDBACK_CHAR: %d\n", value);
			Tcl_ListObjAppendElement(feedback->interp, feedback->line_no,
					Tcl_NewIntObj(value));
			break;

		default:
			fprintf(stderr, "render_ttf_feedback called with bogus tag\n");
			break;
	}
}

static int glue_render_ttf_adv(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj				*base_col_o;
	Tcl_Obj				*face_o;
	Tcl_Obj				*px_size_o;
	Tcl_Obj				*width_o;
	ttf_feedback_data	feedback;
	int					px_size;
	pmap_list			*pmaps;
	pmap_list			*next;
	_pel				base_col;
	FT_Face				face;
	int					width;
	Tcl_Obj				*res;

	CHECK_ARGS(3, "meta text feedbackvar");

	width = 0;
	px_size = 14;
	base_col.c = 0xff000000;

	TEST_OK(Tcl_DictObjGet(interp, objv[1], g_key_base_col, &base_col_o));
	TEST_OK(Tcl_DictObjGet(interp, objv[1], g_key_face, &face_o));
	TEST_OK(Tcl_DictObjGet(interp, objv[1], g_key_px_size, &px_size_o));
	TEST_OK(Tcl_DictObjGet(interp, objv[1], g_key_px_width, &width_o));

	if (base_col_o != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, base_col_o, (int*)&base_col.c));
	if (face_o != NULL) {
		TEST_OK(Tcl_GetTTFFaceFromObj(interp, face_o, &face));
	} else {
		THROW_ERROR("Must specifiy a font face");
	}
	if (px_size_o != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, px_size_o, &px_size));
	if (width_o != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, width_o, &width));


	feedback.interp = interp;
	feedback.res = Tcl_NewListObj(0, NULL);
	feedback.line_no = Tcl_NewListObj(0, NULL);

	pmaps = render_ttf(base_col, face, px_size, 
			Tcl_GetString(objv[2]), width, render_ttf_feedback, (void *)&feedback);

	if (pmaps == NULL)
		THROW_ERROR("Could not render text: ", Tcl_NewStringObj(ttf_last_error_txt(), -1));

	if (width == 0) {
		Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmaps->pmap));
		free(pmaps);
	} else {
		res = Tcl_NewListObj(0, NULL);

		while (1) {
			Tcl_ListObjAppendElement(interp, res, Tcl_NewPMAPObj(pmaps->pmap));
			if (pmaps->next == NULL) {
				break;
			} else {
				next = pmaps->next;
				free(pmaps);
				pmaps = next;
			}
		}
		free(pmaps);

		Tcl_SetObjResult(interp, res);
	}

	Tcl_ObjSetVar2(interp, objv[3], NULL, feedback.res, 0);

	return TCL_OK;
}


// compile_face ttf_face {{{1
static int glue_compile_face(ClientData *foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	FT_Face			face;
	
	CHECK_ARGS(1, "ttf_face");

	TEST_OK(Tcl_GetTTFFaceFromObj(interp, objv[1], &face));

	return TCL_OK;
}


// ttf_info ttf_face {{{1
static int glue_ttf_info(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	FT_Face			face;
	FT_SfntName		finfo;
	Tcl_Obj			*res;
	int				i, count;
	FT_Error		err;
	Tcl_Obj			*item;

	CHECK_ARGS(1, "ttf_face");

	TEST_OK(Tcl_GetTTFFaceFromObj(interp, objv[1], &face));

	count = FT_Get_Sfnt_Name_Count(face);
	
	res = Tcl_NewObj();

	for (i=0; i<count; i++) {
		err = FT_Get_Sfnt_Name(face, i, &finfo);
		if (err != 0)
			THROW_ERROR("Error getting name info for idx: ", Tcl_GetString(Tcl_NewIntObj(i)));
		item = Tcl_NewObj();
		Tcl_ListObjAppendElement(interp, item, Tcl_NewStringObj("platform", -1));
		switch (finfo.platform_id) {
			case TT_PLATFORM_APPLE_UNICODE:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_APPLE_UNICODE", -1));
				
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_APPLE_ID_DEFAULT:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_DEFAULT", -1));
						break;

					case TT_APPLE_ID_UNICODE_1_1:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_UNICODE_1_1", -1));
						break;

					case TT_APPLE_ID_ISO_10646:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_ISO_10646", -1));
						break;

					case TT_APPLE_ID_UNICODE_2_0:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_UNICODE_2_0", -1));
						break;

#ifdef TT_APPLE_ID_UNICODE_32
					case TT_APPLE_ID_UNICODE_32:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_APPLE_ID_UNICODE_32", -1));
						break;
#endif

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_MACINTOSH:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_MACINTOSH", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_MAC_ID_ROMAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ROMAN", -1));
						break;

					case TT_MAC_ID_JAPANESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_JAPANESE", -1));
						break;

					case TT_MAC_ID_TRADITIONAL_CHINESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TRADITIONAL_CHINESE", -1));
						break;

					case TT_MAC_ID_KOREAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_KOREAN", -1));
						break;

					case TT_MAC_ID_ARABIC:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ARABIC", -1));
						break;

					case TT_MAC_ID_HEBREW:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_HEBREW", -1));
						break;

					case TT_MAC_ID_GREEK:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GREEK", -1));
						break;

					case TT_MAC_ID_RUSSIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_RUSSIAN", -1));
						break;

					case TT_MAC_ID_RSYMBOL:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_RSYMBOL", -1));
						break;

					case TT_MAC_ID_DEVANAGARI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_DEVANAGARI", -1));
						break;

					case TT_MAC_ID_GURMUKHI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GURMUKHI", -1));
						break;

					case TT_MAC_ID_GUJARATI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GUJARATI", -1));
						break;

					case TT_MAC_ID_ORIYA:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ORIYA", -1));
						break;

					case TT_MAC_ID_BENGALI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_BENGALI", -1));
						break;

					case TT_MAC_ID_TAMIL:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TAMIL", -1));
						break;

					case TT_MAC_ID_TELUGU:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TELUGU", -1));
						break;

					case TT_MAC_ID_KANNADA:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_KANNADA", -1));
						break;

					case TT_MAC_ID_MALAYALAM:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_MALAYALAM", -1));
						break;

					case TT_MAC_ID_SINHALESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SINHALESE", -1));
						break;

					case TT_MAC_ID_BURMESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_BURMESE", -1));
						break;

					case TT_MAC_ID_KHMER:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_KHMER", -1));
						break;

					case TT_MAC_ID_THAI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_THAI", -1));
						break;

					case TT_MAC_ID_LAOTIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_LAOTIAN", -1));
						break;

					case TT_MAC_ID_GEORGIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GEORGIAN", -1));
						break;

					case TT_MAC_ID_ARMENIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_ARMENIAN", -1));
						break;

					/* This is the same as SIMPLIFIED CHINESE
					case TT_MAC_ID_MALDIVIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_MALDIVIAN", -1));
						break;
						*/

					case TT_MAC_ID_SIMPLIFIED_CHINESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SIMPLIFIED_CHINESE", -1));
						break;

					case TT_MAC_ID_TIBETAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_TIBETAN", -1));
						break;

					case TT_MAC_ID_MONGOLIAN:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_MONGOLIAN", -1));
						break;

					case TT_MAC_ID_GEEZ:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_GEEZ", -1));
						break;

					case TT_MAC_ID_SLAVIC:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SLAVIC", -1));
						break;

					case TT_MAC_ID_VIETNAMESE:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_VIETNAMESE", -1));
						break;

					case TT_MAC_ID_SINDHI:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_SINDHI", -1));
						break;

					case TT_MAC_ID_UNINTERP:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MAC_ID_UNINTERP", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_ISO:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_ISO", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_ISO_ID_7BIT_ASCII:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ISO_ID_7BIT_ASCII", -1));
						break;

					case TT_ISO_ID_10646:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ISO_ID_10646", -1));
						break;

					case TT_ISO_ID_8859_1:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ISO_ID_8859_1", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_MICROSOFT:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_MICROSOFT", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_MS_ID_SYMBOL_CS:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_SYMBOL_CS", -1));
						break;

					case TT_MS_ID_UNICODE_CS:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_UNICODE_CS", -1));
						break;

					case TT_MS_ID_SJIS:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_SJIS", -1));
						break;

					case TT_MS_ID_GB2312:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_GB2312", -1));
						break;

					case TT_MS_ID_BIG_5:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_BIG_5", -1));
						break;

					case TT_MS_ID_WANSUNG:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_WANSUNG", -1));
						break;

					case TT_MS_ID_JOHAB:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_JOHAB", -1));
						break;

					case TT_MS_ID_UCS_4:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_MS_ID_UCS_4", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			case TT_PLATFORM_CUSTOM:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_CUSTOM", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("UNKNOWN", -1));

				break;

			case TT_PLATFORM_ADOBE:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_PLATFORM_ADOBE", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					case TT_ADOBE_ID_STANDARD:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ADOBE_ID_STANDARD", -1));
						break;

					case TT_ADOBE_ID_EXPERT:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ADOBE_ID_EXPERT", -1));
						break;

					case TT_ADOBE_ID_CUSTOM:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("TT_ADOBE_ID_CUSTOM", -1));
						break;

					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

			default:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("UNKNOWN", -1));

				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("encoding", -1));

				switch (finfo.encoding_id) {
					default:
						Tcl_ListObjAppendElement(interp, item,
								Tcl_NewStringObj("UNKNOWN", -1));
						break;
				}
				break;

		}
		Tcl_ListObjAppendElement(interp, item, Tcl_NewStringObj("type", -1));
		switch (finfo.name_id) {
			case TT_NAME_ID_COPYRIGHT:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_COPYRIGHT", -1));
				break;

			case TT_NAME_ID_FONT_FAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_FONT_FAMILY", -1));
				break;

			case TT_NAME_ID_FONT_SUBFAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_FONT_SUBFAMILY", -1));
				break;

			case TT_NAME_ID_UNIQUE_ID:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_UNIQUE_ID", -1));
				break;

			case TT_NAME_ID_FULL_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_FULL_NAME", -1));
				break;

			case TT_NAME_ID_VERSION_STRING:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_VERSION_STRING", -1));
				break;

			case TT_NAME_ID_PS_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_PS_NAME", -1));
				break;

			case TT_NAME_ID_TRADEMARK:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_TRADEMARK", -1));
				break;

			case TT_NAME_ID_MANUFACTURER:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_MANUFACTURER", -1));
				break;

			case TT_NAME_ID_DESIGNER:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_DESIGNER", -1));
				break;

			case TT_NAME_ID_DESCRIPTION:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_DESCRIPTION", -1));
				break;

			case TT_NAME_ID_VENDOR_URL:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_VENDOR_URL", -1));
				break;

			case TT_NAME_ID_DESIGNER_URL:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_DESIGNER_URL", -1));
				break;

			case TT_NAME_ID_LICENSE:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_LICENSE", -1));
				break;

			case TT_NAME_ID_LICENSE_URL:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_LICENSE_URL", -1));
				break;

			case TT_NAME_ID_PREFERRED_FAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_PREFERRED_FAMILY", -1));
				break;

			case TT_NAME_ID_PREFERRED_SUBFAMILY:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_PREFERRED_SUBFAMILY", -1));
				break;

			case TT_NAME_ID_MAC_FULL_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_MAC_FULL_NAME", -1));
				break;

			case TT_NAME_ID_SAMPLE_TEXT:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_SAMPLE_TEXT", -1));
				break;

			case TT_NAME_ID_CID_FINDFONT_NAME:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("TT_NAME_ID_CID_FINDFONT_NAME", -1));
				break;

			default:
				Tcl_ListObjAppendElement(interp, item,
						Tcl_NewStringObj("UNKNOWN", -1));
				break;
		}
		
		Tcl_ListObjAppendElement(interp, item,
				Tcl_NewStringObj("string", -1));
		
		Tcl_ListObjAppendElement(interp, item,
				Tcl_NewStringObj((const char*)finfo.string, finfo.string_len));

		Tcl_ListObjAppendElement(interp, res, item);
	}

	Tcl_SetObjResult(interp, res);
	
	return TCL_OK;
}


// init_state meta {{{1
static int glue_init_state(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	Tcl_Obj				*base_col_o;
	Tcl_Obj				*face_o;
	Tcl_Obj				*px_size_o;
	int					width, px_size;
	_pel				base_col;
	FT_Face				face;
	struct ttf_state*	state = NULL;

	CHECK_ARGS(1, "meta");

	width = 0;
	px_size = 14;
	base_col.c = 0xff000000;

	TEST_OK(Tcl_DictObjGet(interp, objv[1], g_key_base_col, &base_col_o));
	TEST_OK(Tcl_DictObjGet(interp, objv[1], g_key_face, &face_o));
	TEST_OK(Tcl_DictObjGet(interp, objv[1], g_key_px_size, &px_size_o));

	if (base_col_o != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, base_col_o, (int*)&base_col.c));
	if (face_o != NULL) {
		TEST_OK(Tcl_GetTTFFaceFromObj(interp, face_o, &face));
	} else {
		THROW_ERROR("Must specifiy a font face");
	}
	if (px_size_o != NULL)
		TEST_OK(Tcl_GetIntFromObj(interp, px_size_o, &px_size));

	state = (struct ttf_state*)malloc(sizeof(*state));

	if (ttf_init_state(interp, base_col, face, px_size, state) != TCL_OK)
		goto failed;

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj((const unsigned char*)state, sizeof(*state)));
	free(state); state = NULL;
	return TCL_OK;

failed:
	free(state); state = NULL;
	return TCL_ERROR;
}


// next_char char {{{1
static int glue_next_char(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	struct ttf_state*	state = NULL;
	int					len;
	const char*			c;
	gimp_image_t*		glyph_pmap;
	double				dx, dy;
	int					ox, oy;
	int					width;
	Tcl_Obj*			res;

	CHECK_ARGS(2, "state char");

	state = (struct ttf_state*)Tcl_GetByteArrayFromObj(objv[1], &len);
	if (len != sizeof(*state))
		THROW_ERROR("Invalid state token");

	c = Tcl_GetStringFromObj(Tcl_GetRange(objv[2], 0, 0), &len);
	if (len == 0)
		THROW_ERROR("No characters specified");

	TEST_OK(ttf_next_char(interp, state, c, len, &dx, &dy, &ox, &oy, &width, &glyph_pmap));

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(dx)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewDoubleObj(dy)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(ox)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(oy)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(width)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewPMAPObj(glyph_pmap)));

	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}


// cleanup_state meta {{{1
static int glue_cleanup_state(cdata, interp, objc, objv)
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	CHECK_ARGS(1, "meta");

	return TCL_OK;
}


// Init {{{1
int Pixel_ttf_Init(Tcl_Interp *interp)
{
	double		foo;
	uint32_t	a,b,bar;

	if (Tcl_InitStubs(interp, "8.6", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.4", 0) == NULL) return TCL_ERROR;

	for( a=0; a<=255; a++ )
		for( b=0; b<=255; b++ ) {
			foo = a * (b/255.0);
			bar = (uint8)foo;
			if( foo - (uint8)(foo) > 0.5 ) bar++;
			fact[a][b] = bar;
		}

	Tcl_RegisterObjType(&tcl_ttf_face);

	if (g_key_base_col == NULL) g_key_base_col = Tcl_NewStringObj("base_col", -1);
	if (g_key_face == NULL) g_key_face = Tcl_NewStringObj("face", -1);
	if (g_key_px_size == NULL) g_key_px_size = Tcl_NewStringObj("px_size", -1);
	if (g_key_px_width == NULL) g_key_px_width = Tcl_NewStringObj("px_width", -1);

	Tcl_IncrRefCount(g_key_base_col);
	Tcl_IncrRefCount(g_key_face);
	Tcl_IncrRefCount(g_key_px_size);
	Tcl_IncrRefCount(g_key_px_width);

	init_ttf();

	// TrueType font rendering
	NEW_CMD("pixel::ttf::render_ttf", glue_render_ttf);
	NEW_CMD("pixel::ttf::render_ttf_adv", glue_render_ttf_adv);
	NEW_CMD("pixel::ttf::compile_face", glue_compile_face);
	NEW_CMD("pixel::ttf::ttf_info", glue_ttf_info);
	NEW_CMD("pixel::ttf::init_state", glue_init_state);
	NEW_CMD("pixel::ttf::next_char", glue_next_char);
	NEW_CMD("pixel::ttf::cleanup_state", glue_cleanup_state);

	return TCL_OK;
}
