#ifndef _TTF_H
#define _TTF_H

//#include <Pixel/2d.h>
EXTERN_C gimp_image_t *pmap_new_y(int x, int y, _pel colour);
//#include <2d.h>
#include <tcl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <freetype2/freetype/ttnameid.h>
#include <freetype2/freetype/ftsnames.h>

typedef void (* ttf_feedback_cb)(void *clientdata, int what, int value);

struct ttf_state {
	double			advance_x;
	double			advance_y;
	int				previous;
	int				use_kerning;
	FT_Face			face;
	FT_GlyphSlot	slot;
	_pel			base_col;
};

enum {
	TTF_FEEDBACK_LINESTART,
	TTF_FEEDBACK_CHAR
};
#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif

EXTERN_C FT_Library	ft_library;
EXTERN_C Tcl_ObjType	tcl_ttf_face;
EXTERN_C uint8_t	fact[256][256];

EXTERN_C pmap_list *render_ttf(_pel base_col, FT_Face face, int px_size, char *utf8_text, int wrap_width, ttf_feedback_cb cb, void *clientdata);
EXTERN_C int ttf_init_state(Tcl_Interp* interp, _pel base_col, FT_Face face, int px_size, struct ttf_state* state);;
EXTERN_C int ttf_next_char(Tcl_Interp* interp, struct ttf_state* state, const char* utf8_char, int char_byte_len, double* dx, double* dy, int* ox, int* oy, int* width, gimp_image_t** pmap);
EXTERN_C int new_face(char *ttf_file, int px_height);
EXTERN_C int init_ttf();

EXTERN_C void Tcl_SetTTFFaceObj(Tcl_Obj * obj, FT_Face face, char * filename);
EXTERN_C Tcl_Obj *Tcl_NewTTFFaceObj(FT_Face face, char * filename);
EXTERN_C int Tcl_GetTTFFaceFromObj(Tcl_Interp * interp, Tcl_Obj * obj, FT_Face * face);
EXTERN_C char *ttf_last_error_txt();


#endif


